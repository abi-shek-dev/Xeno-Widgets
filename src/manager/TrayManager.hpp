#pragma once

#include "Common.hpp"
#include <shellapi.h>

namespace xe {

    class TrayManager {
    public:
        static TrayManager& Instance();

        bool Initialize(HWND messageWindow, HINSTANCE hInstance);
        void Shutdown();

        // Call this from WM_APP+1 handler in the message window
        void HandleTrayMessage(WPARAM wParam, LPARAM lParam);

    private:
        TrayManager() = default;
        ~TrayManager() = default;
        TrayManager(const TrayManager&) = delete;
        TrayManager& operator=(const TrayManager&) = delete;

        void ShowTrayMenu();
        void CreateTrayIcon();
        void RemoveTrayIcon();

        HWND m_messageWnd = nullptr;
        HINSTANCE m_hInstance = nullptr;
        NOTIFYICONDATAW m_nid{};
        bool m_initialized = false;
    };

}
