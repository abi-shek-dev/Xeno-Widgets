#pragma once

#include "Common.hpp"
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>

namespace xe {

    struct TelemetryData {
        float cpuPercent       = 0.0f;  // 0–100
        float ramUsedGB        = 0.0f;
        float ramTotalGB       = 0.0f;
        float ramPercent       = 0.0f;  // 0–100
        float netDownMbps      = 0.0f;
        float netUpMbps        = 0.0f;
        float cpuHistory[60]   = {};    // ring buffer of last 60 CPU samples
        int   cpuHistoryIdx    = 0;
    };

    class TelemetryProvider {
    public:
        static TelemetryProvider& Instance();

        bool Initialize();
        void Shutdown();
        TelemetryData GetData() const;

    private:
        TelemetryProvider() = default;
        ~TelemetryProvider();
        TelemetryProvider(const TelemetryProvider&) = delete;
        TelemetryProvider& operator=(const TelemetryProvider&) = delete;

        void PollThread();

        std::thread m_thread;
        std::atomic<bool> m_running{false};
        std::condition_variable m_cv;
        mutable std::mutex m_mutex;
        TelemetryData m_data;
    };

}
