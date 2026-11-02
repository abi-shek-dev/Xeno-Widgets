#include "SMTCProvider.hpp"
#include "../core/Logger.hpp"

#ifdef XE_SMTC_ENABLED

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Media.Control.h>

namespace xe {

    SMTCProvider& SMTCProvider::Instance() {
        static SMTCProvider s_instance;
        return s_instance;
    }

    bool SMTCProvider::Initialize() {
        if (m_running.load()) {
            return true;
        }

        m_running = true;
        m_pollThread = std::thread(&SMTCProvider::PollThread, this);
        XE_LOG_INFO("SMTCProvider initialized with Windows Media Control integration");
        return true;
    }

    void SMTCProvider::Shutdown() {
        if (m_running.load()) {
            m_running = false;
            if (m_pollThread.joinable()) {
                m_pollThread.join();
            }
        }
        XE_LOG_INFO("SMTCProvider shutdown complete");
    }

    MediaInfo SMTCProvider::GetCurrentMedia() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_current;
    }

    void SMTCProvider::PollThread() {
        winrt::init_apartment(winrt::apartment_type::multi_threaded);

        winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSessionManager sessionManager{ nullptr };

        while (m_running.load()) {
            try {
                if (!sessionManager) {
                    sessionManager = winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSessionManager::RequestAsync().get();
                }

                auto session = sessionManager ? sessionManager.GetCurrentSession() : nullptr;
                if (session) {
                    auto mediaProps = session.TryGetMediaPropertiesAsync().get();
                    auto timeline = session.GetTimelineProperties();
                    auto playback = session.GetPlaybackInfo();

                    MediaInfo info{};
                    if (mediaProps) {
                        info.title = mediaProps.Title().c_str();
                        info.artist = mediaProps.Artist().c_str();
                        info.albumTitle = mediaProps.AlbumTitle().c_str();
                    }

                    if (!info.title.empty() || !info.artist.empty()) {
                        info.hasMedia = true;
                        info.isPlaying = (playback.PlaybackStatus() ==
                            winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing);

                        info.progressSeconds = std::chrono::duration<double>(timeline.Position()).count();
                        info.durationSeconds = std::chrono::duration<double>(timeline.EndTime()).count();

                        {
                            std::lock_guard<std::mutex> lock(m_mutex);
                            m_current = info;
                        }
                        m_available = true;
                    } else {
                        std::lock_guard<std::mutex> lock(m_mutex);
                        m_current.hasMedia = false;
                        m_available = false;
                    }
                } else {
                    std::lock_guard<std::mutex> lock(m_mutex);
                    m_current.hasMedia = false;
                    m_available = false;
                }
            } catch (...) {
                sessionManager = nullptr;
                std::lock_guard<std::mutex> lock(m_mutex);
                m_current.hasMedia = false;
                m_available = false;
            }

            for (int i = 0; i < 10 && m_running.load(); ++i) {
                Sleep(100);
            }
        }

        winrt::uninit_apartment();
    }

} // namespace xe

#else

namespace xe {

    SMTCProvider& SMTCProvider::Instance() {
        static SMTCProvider s_instance;
        return s_instance;
    }

    bool SMTCProvider::Initialize() {
        XE_LOG_INFO("SMTCProvider initialized in demo fallback mode (XE_SMTC_ENABLED not set)");
        return true;
    }

    void SMTCProvider::Shutdown() {
        XE_LOG_INFO("SMTCProvider shutdown complete");
    }

    MediaInfo SMTCProvider::GetCurrentMedia() const {
        return MediaInfo{}; // hasMedia = false
    }

    void SMTCProvider::PollThread() {
    }

} // namespace xe

#endif
