#include "TelemetryProvider.hpp"
#include "../core/Logger.hpp"
#include <iphlpapi.h>
#include <algorithm>
#include <vector>

#pragma comment(lib, "iphlpapi.lib")

namespace xe {

    TelemetryProvider& TelemetryProvider::Instance() {
        static TelemetryProvider s_instance;
        return s_instance;
    }

    TelemetryProvider::~TelemetryProvider() {
        Shutdown();
    }

    bool TelemetryProvider::Initialize() {
        if (m_running.load()) {
            return true;
        }

        m_running = true;
        m_thread = std::thread(&TelemetryProvider::PollThread, this);
        XE_LOG_INFO("TelemetryProvider initialized and background polling started");
        return true;
    }

    void TelemetryProvider::Shutdown() {
        if (!m_running.load()) {
            return;
        }

        m_running = false;
        m_cv.notify_all();

        if (m_thread.joinable()) {
            m_thread.join();
        }

        XE_LOG_INFO("TelemetryProvider shutdown completed");
    }

    TelemetryData TelemetryProvider::GetData() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_data;
    }

    void TelemetryProvider::PollThread() {
        auto ToULL = [](const FILETIME& ft) -> ULONGLONG {
            return (static_cast<ULONGLONG>(ft.dwHighDateTime) << 32) | ft.dwLowDateTime;
        };

        auto QueryNetBytes = [](uint64_t& inBytes, uint64_t& outBytes) -> bool {
            inBytes = 0;
            outBytes = 0;
            ULONG size = 0;
            if (GetIfTable(nullptr, &size, FALSE) == ERROR_INSUFFICIENT_BUFFER) {
                std::vector<BYTE> buffer(size);
                auto pTable = reinterpret_cast<MIB_IFTABLE*>(buffer.data());
                if (GetIfTable(pTable, &size, FALSE) == NO_ERROR) {
                    for (DWORD i = 0; i < pTable->dwNumEntries; ++i) {
                        const auto& row = pTable->table[i];
                        if (row.dwOperStatus == IF_OPER_STATUS_OPERATIONAL && row.dwType != MIB_IF_TYPE_LOOPBACK) {
                            inBytes += row.dwInOctets;
                            outBytes += row.dwOutOctets;
                        }
                    }
                    return true;
                }
            }
            return false;
        };

        uint64_t prevInBytes = 0;
        uint64_t prevOutBytes = 0;
        auto prevNetTime = std::chrono::steady_clock::now();
        bool hasPrevNet = false;

        // Warm up initial network reading
        QueryNetBytes(prevInBytes, prevOutBytes);
        hasPrevNet = true;

        while (m_running.load()) {
            // --- 1. CPU Sampling (200ms interval) ---
            FILETIME idle1{}, kernel1{}, user1{};
            GetSystemTimes(&idle1, &kernel1, &user1);

            {
                std::unique_lock<std::mutex> cvLock(m_mutex);
                m_cv.wait_for(cvLock, std::chrono::milliseconds(200), [this] {
                    return !m_running.load();
                });
            }

            if (!m_running.load()) {
                break;
            }

            FILETIME idle2{}, kernel2{}, user2{};
            GetSystemTimes(&idle2, &kernel2, &user2);

            ULONGLONG idleDelta = ToULL(idle2) - ToULL(idle1);
            ULONGLONG kernelDelta = ToULL(kernel2) - ToULL(kernel1);
            ULONGLONG userDelta = ToULL(user2) - ToULL(user1);
            ULONGLONG totalDelta = kernelDelta + userDelta;

            float cpuPercent = 0.0f;
            if (totalDelta > 0) {
                cpuPercent = 100.0f - (static_cast<float>(idleDelta) * 100.0f / static_cast<float>(totalDelta));
            }
            cpuPercent = std::clamp(cpuPercent, 0.0f, 100.0f);

            // --- 2. RAM Sampling ---
            MEMORYSTATUSEX ms{};
            ms.dwLength = sizeof(ms);
            float ramTotalGB = 0.0f;
            float ramUsedGB  = 0.0f;
            float ramPercent = 0.0f;
            if (GlobalMemoryStatusEx(&ms)) {
                ramTotalGB = static_cast<float>(ms.ullTotalPhys) / (1024.0f * 1024.0f * 1024.0f);
                ramUsedGB  = static_cast<float>(ms.ullTotalPhys - ms.ullAvailPhys) / (1024.0f * 1024.0f * 1024.0f);
                ramPercent = static_cast<float>(ms.dwMemoryLoad);
            }

            // --- 3. Network Sampling ---
            uint64_t currInBytes = 0;
            uint64_t currOutBytes = 0;
            float netDownMbps = 0.0f;
            float netUpMbps   = 0.0f;
            auto now = std::chrono::steady_clock::now();

            if (QueryNetBytes(currInBytes, currOutBytes)) {
                float elapsedSec = std::chrono::duration<float>(now - prevNetTime).count();
                if (hasPrevNet && elapsedSec > 0.05f) {
                    uint64_t deltaIn = (currInBytes >= prevInBytes) ? (currInBytes - prevInBytes) : currInBytes;
                    uint64_t deltaOut = (currOutBytes >= prevOutBytes) ? (currOutBytes - prevOutBytes) : currOutBytes;
                    netDownMbps = (static_cast<float>(deltaIn) * 8.0f) / (elapsedSec * 1000000.0f);
                    netUpMbps   = (static_cast<float>(deltaOut) * 8.0f) / (elapsedSec * 1000000.0f);
                }
                prevInBytes = currInBytes;
                prevOutBytes = currOutBytes;
                prevNetTime = now;
                hasPrevNet = true;
            }

            // --- 4. Thread-Safe Update ---
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                m_data.cpuPercent = cpuPercent;
                m_data.ramTotalGB = ramTotalGB;
                m_data.ramUsedGB  = ramUsedGB;
                m_data.ramPercent = ramPercent;
                m_data.netDownMbps = netDownMbps;
                m_data.netUpMbps   = netUpMbps;

                m_data.cpuHistory[m_data.cpuHistoryIdx] = cpuPercent;
                m_data.cpuHistoryIdx = (m_data.cpuHistoryIdx + 1) % 60;
            }

            // --- 5. Sleep remaining interval (approx 800ms) ---
            {
                std::unique_lock<std::mutex> cvLock(m_mutex);
                m_cv.wait_for(cvLock, std::chrono::milliseconds(800), [this] {
                    return !m_running.load();
                });
            }
        }
    }

}
