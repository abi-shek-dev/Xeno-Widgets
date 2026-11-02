#pragma once

#include "Common.hpp"
#include <atomic>
#include <mutex>
#include <thread>
#include <string>

namespace xe {

    struct MediaInfo {
        std::wstring title;
        std::wstring artist;
        std::wstring albumTitle;
        bool isPlaying = false;
        double progressSeconds = 0.0;
        double durationSeconds = 0.0;
        bool hasMedia = false;
    };

    class SMTCProvider {
    public:
        static SMTCProvider& Instance();

        // Start background polling thread
        bool Initialize();
        void Shutdown();

        // Thread-safe read of latest media info
        MediaInfo GetCurrentMedia() const;
        bool IsAvailable() const { return m_available.load(); }

    private:
        SMTCProvider() = default;
        ~SMTCProvider() = default;
        SMTCProvider(const SMTCProvider&) = delete;
        SMTCProvider& operator=(const SMTCProvider&) = delete;

        void PollThread();

        std::thread m_pollThread;
        std::atomic<bool> m_running{ false };
        std::atomic<bool> m_available{ false };

        mutable std::mutex m_mutex;
        MediaInfo m_current;
    };

}
