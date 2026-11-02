#include "TrayManager.hpp"
#include "../core/Logger.hpp"
#include "../windows/WindowManager.hpp"

namespace xe {

    TrayManager& TrayManager::Instance() {
        static TrayManager s_instance;
        return s_instance;
    }

    bool TrayManager::Initialize(HWND messageWindow, HINSTANCE hInstance) {
        if (m_initialized) {
            return true;
        }

        m_messageWnd = messageWindow;
        m_hInstance = hInstance;

        CreateTrayIcon();
        XE_LOG_INFO("TrayManager initialized");
        return m_initialized;
    }

    void TrayManager::Shutdown() {
        RemoveTrayIcon();
        m_messageWnd = nullptr;
        m_hInstance = nullptr;
        XE_LOG_INFO("TrayManager shutdown complete");
    }

    void TrayManager::CreateTrayIcon() {
        if (!m_messageWnd) return;

        ZeroMemory(&m_nid, sizeof(m_nid));
        m_nid.cbSize = sizeof(NOTIFYICONDATAW);
        m_nid.hWnd = m_messageWnd;
        m_nid.uID = 1;
        m_nid.uFlags = NIF_TIP | NIF_MESSAGE;
        m_nid.uCallbackMessage = WM_APP + 1;
        wcscpy_s(m_nid.szTip, L"XE Widgets (Hold Ctrl to interact)");

        m_nid.hIcon = LoadIconW(nullptr, IDI_APPLICATION);
        if (!m_nid.hIcon) {
            m_nid.hIcon = LoadIconW(m_hInstance, IDI_APPLICATION);
        }
        if (!m_nid.hIcon) {
            m_nid.hIcon = reinterpret_cast<HICON>(LoadImageW(nullptr, IDI_APPLICATION, IMAGE_ICON, 16, 16, LR_SHARED));
        }
        if (m_nid.hIcon) {
            m_nid.uFlags |= NIF_ICON;
        }

        // Try deleting any stale icon from previous run
        Shell_NotifyIconW(NIM_DELETE, &m_nid);

        if (!Shell_NotifyIconW(NIM_ADD, &m_nid)) {
            m_nid.cbSize = NOTIFYICONDATAW_V3_SIZE;
            if (!Shell_NotifyIconW(NIM_ADD, &m_nid)) {
                m_nid.cbSize = NOTIFYICONDATAW_V2_SIZE;
                if (!Shell_NotifyIconW(NIM_ADD, &m_nid)) {
                    DWORD err = GetLastError();
                    XE_LOG_ERROR(std::format("Failed to add system tray icon. Win32 Error: {}", err));
                    m_initialized = false;
                    return;
                }
            }
        }

        m_initialized = true;
    }

    void TrayManager::RemoveTrayIcon() {
        if (m_initialized) {
            Shell_NotifyIconW(NIM_DELETE, &m_nid);
            m_initialized = false;
        }
    }

    void TrayManager::HandleTrayMessage(WPARAM /*wParam*/, LPARAM lParam) {
        if (lParam == WM_RBUTTONUP) {
            ShowTrayMenu();
        } else if (lParam == WM_LBUTTONDBLCLK) {
            if (m_messageWnd) {
                ShowWindow(m_messageWnd, SW_SHOW);
                SetForegroundWindow(m_messageWnd);
            }
        }
    }

    void TrayManager::ShowTrayMenu() {
        HMENU hMenu = CreatePopupMenu();
        if (!hMenu) return;

        AppendMenuW(hMenu, MF_STRING | MF_GRAYED | MF_DISABLED, 0, L"XE Widgets");
        AppendMenuW(hMenu, MF_SEPARATOR, 0, nullptr);

        // Submenu: Manage Desktop Widgets
        HMENU hWidgetsSub = CreatePopupMenu();

        struct WidgetMeta {
            const wchar_t* id;
            const wchar_t* title;
            UINT baseCmd;
        };

        WidgetMeta metas[] = {
            { L"clock",    L"XE Precision Clock",         1000 },
            { L"monitor",  L"XE System Telemetry",        1010 },
            { L"music",    L"XE Media Player",            1020 },
            { L"weather",  L"XE Atmosphere",              1030 },
            { L"scratch",  L"XE Quick Scratchpad",        1040 },
            { L"calendar", L"XE Event Horizon Calendar",  1050 }
        };

        for (const auto& meta : metas) {
            bool running = WindowManager::Instance().IsWidgetRunning(meta.id);
            WidgetWindow* w = running ? WindowManager::Instance().FindWidgetById(meta.id) : nullptr;

            if (running && w) {
                HMENU hSubItem = CreatePopupMenu();
                bool ct = w->GetConfig().clickThrough;
                bool lk = w->GetConfig().locked;
                AppendMenuW(hSubItem, MF_STRING | (ct ? MF_CHECKED : MF_UNCHECKED), meta.baseCmd + 1, L"Click-Through");
                AppendMenuW(hSubItem, MF_STRING | (lk ? MF_CHECKED : MF_UNCHECKED), meta.baseCmd + 2, L"Lock Position");
                AppendMenuW(hSubItem, MF_SEPARATOR, 0, nullptr);
                AppendMenuW(hSubItem, MF_STRING, meta.baseCmd + 3, L"Close Widget");

                std::wstring label = std::wstring(L"[Active] ") + meta.title;
                AppendMenuW(hWidgetsSub, MF_POPUP, reinterpret_cast<UINT_PTR>(hSubItem), label.c_str());
            } else {
                std::wstring label = std::wstring(L"[Closed] Open ") + meta.title;
                AppendMenuW(hWidgetsSub, MF_STRING, meta.baseCmd + 0, label.c_str());
            }
        }

        AppendMenuW(hMenu, MF_POPUP, reinterpret_cast<UINT_PTR>(hWidgetsSub), L"Manage Widgets");
        AppendMenuW(hMenu, MF_STRING, 210, L"Disable Click-Through (All Widgets)");
        AppendMenuW(hMenu, MF_SEPARATOR, 0, nullptr);
        AppendMenuW(hMenu, MF_STRING, 201, L"Show All Widgets");
        AppendMenuW(hMenu, MF_STRING, 202, L"Hide All Widgets");
        AppendMenuW(hMenu, MF_SEPARATOR, 0, nullptr);
        AppendMenuW(hMenu, MF_STRING, 203, L"Open XE Manager");
        AppendMenuW(hMenu, MF_SEPARATOR, 0, nullptr);
        AppendMenuW(hMenu, MF_STRING, 299, L"Exit XE Widgets");

        POINT pt{};
        GetCursorPos(&pt);

        if (m_messageWnd) {
            SetForegroundWindow(m_messageWnd);
        }

        UINT cmd = TrackPopupMenu(
            hMenu,
            TPM_RIGHTBUTTON | TPM_RETURNCMD,
            pt.x,
            pt.y,
            0,
            m_messageWnd,
            nullptr
        );

        if (cmd == 201) {
            WindowManager::Instance().ShowAllWidgets();
        } else if (cmd == 202) {
            WindowManager::Instance().HideAllWidgets();
        } else if (cmd == 203) {
            if (m_messageWnd) {
                ShowWindow(m_messageWnd, SW_SHOW);
                SetForegroundWindow(m_messageWnd);
            }
        } else if (cmd == 210) {
            WindowManager::Instance().DisableAllClickThrough();
        } else if (cmd == 299) {
            PostQuitMessage(0);
        } else if (cmd >= 1000 && cmd < 1070) {
            for (const auto& meta : metas) {
                if (cmd >= meta.baseCmd && cmd < meta.baseCmd + 10) {
                    UINT sub = cmd - meta.baseCmd;
                    if (sub == 0) {
                        // Open closed widget
                        WindowManager::Instance().OpenWidgetById(meta.id);
                    } else {
                        WidgetWindow* w = WindowManager::Instance().FindWidgetById(meta.id);
                        if (w) {
                            if (sub == 1) {
                                w->SetClickThrough(!w->GetConfig().clickThrough);
                            } else if (sub == 2) {
                                w->SetLocked(!w->GetConfig().locked);
                            } else if (sub == 3) {
                                w->Close();
                            }
                        }
                    }
                    break;
                }
            }
        }

        DestroyMenu(hMenu);
    }

}
