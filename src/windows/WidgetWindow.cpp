#include "WidgetWindow.hpp"
#include "WindowManager.hpp"
#include "../core/Logger.hpp"
#include <algorithm>

namespace xe {

    enum WidgetMenuId : UINT {
        ID_MENU_LOCK = 2001,
        ID_MENU_ALWAYSONTOP,
        ID_MENU_OPACITY_100,
        ID_MENU_OPACITY_75,
        ID_MENU_OPACITY_50,
        ID_MENU_CLICKTHROUGH,
        ID_MENU_OPEN_MANAGER,
        ID_MENU_CLOSE
    };

    WidgetWindow::WidgetWindow() = default;

    WidgetWindow::~WidgetWindow() {
        Close();
    }

    bool WidgetWindow::RegisterClass() {
        if (s_classRegistered) return true;

        WNDCLASSEXW wc{};
        wc.cbSize = sizeof(WNDCLASSEXW);
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = StaticWndProc;
        wc.hInstance = GetModuleHandleW(nullptr);
        wc.hCursor = LoadCursorW(nullptr, MAKEINTRESOURCEW(32512));
        wc.hbrBackground = nullptr;
        wc.lpszClassName = s_className;

        if (!RegisterClassExW(&wc)) {
            XE_LOG_ERROR("Failed to register Widget Window Class");
            return false;
        }

        s_classRegistered = true;
        return true;
    }

    bool WidgetWindow::Create(const Config& config, D2DRenderer* renderer) {
        if (!RegisterClass()) return false;

        m_config = config;
        m_renderer = renderer;
        m_locked = config.locked;

        DWORD dwStyle = WS_POPUP | WS_VISIBLE;
        DWORD dwExStyle = WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE;

        if (m_config.alwaysOnTop) {
            dwExStyle |= WS_EX_TOPMOST;
        }
        if (m_config.clickThrough) {
            dwExStyle |= WS_EX_TRANSPARENT;
        }

        m_hwnd = CreateWindowExW(
            dwExStyle,
            s_className,
            config.title.c_str(),
            dwStyle,
            config.x,
            config.y,
            config.width,
            config.height,
            nullptr,
            nullptr,
            GetModuleHandleW(nullptr),
            this
        );

        if (!m_hwnd) {
            XE_LOG_ERROR("Failed to create native widget HWND");
            return false;
        }

        // Enable Immersive Dark Mode
        BOOL useDarkMode = TRUE;
        DwmSetWindowAttribute(m_hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &useDarkMode, sizeof(useDarkMode));

        // Extend frame into client area for full Direct2D alpha compositing
        MARGINS margins = { -1, -1, -1, -1 };
        DwmExtendFrameIntoClientArea(m_hwnd, &margins);

        // Apply initial opacity
        SetOpacity(m_config.opacity);

        // Bind Direct2D resources to this HWND
        if (m_renderer) {
            if (!m_renderer->CreateDeviceResources(m_hwnd)) {
                XE_LOG_ERROR("Failed to initialize Direct2D device resources for widget");
                return false;
            }

            // Typography format for hover toolbar
            m_formatToolbar = m_renderer->CreateTextFormat(
                L"Geist Mono",
                10.0f,
                DWRITE_FONT_WEIGHT_MEDIUM,
                DWRITE_FONT_STYLE_NORMAL,
                DWRITE_TEXT_ALIGNMENT_CENTER,
                DWRITE_PARAGRAPH_ALIGNMENT_CENTER
            );
        }

        XE_LOG_INFO("WidgetWindow created successfully");
        return true;
    }

    void WidgetWindow::Show() {
        if (m_hwnd) {
            ShowWindow(m_hwnd, SW_SHOWNOACTIVATE);
            UpdateWindow(m_hwnd);
        }
    }

    void WidgetWindow::Hide() {
        if (m_hwnd) {
            ShowWindow(m_hwnd, SW_HIDE);
        }
    }

    void WidgetWindow::Close() {
        if (m_hwnd) {
            RECT r{};
            GetWindowRect(m_hwnd, &r);
            m_config.x = r.left;
            m_config.y = r.top;
            m_config.visible = false;
            HWND hwndToDestroy = m_hwnd;
            m_hwnd = nullptr;
            if (m_renderer) {
                m_renderer->DiscardDeviceResources(hwndToDestroy);
            }
            DestroyWindow(hwndToDestroy);
        }
    }

    void WidgetWindow::Destroy() {
        Close();
    }

    void WidgetWindow::SetAlwaysOnTop(bool value) {
        m_config.alwaysOnTop = value;
        if (m_hwnd) {
            SetWindowPos(
                m_hwnd,
                value ? HWND_TOPMOST : HWND_NOTOPMOST,
                0, 0, 0, 0,
                SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE
            );
            InvalidateRect(m_hwnd, nullptr, FALSE);
        }
    }

    void WidgetWindow::SetOpacity(float opacity) {
        m_config.opacity = std::clamp(opacity, 0.0f, 1.0f);
        if (m_hwnd) {
            BYTE alphaByte = static_cast<BYTE>(m_config.opacity * 255.0f);
            SetLayeredWindowAttributes(m_hwnd, 0, alphaByte, LWA_ALPHA);
        }
    }

    void WidgetWindow::SetClickThrough(bool value) {
        m_config.clickThrough = value;
        if (m_hwnd) {
            LONG_PTR exStyle = GetWindowLongPtrW(m_hwnd, GWL_EXSTYLE);
            if (value) {
                exStyle |= WS_EX_TRANSPARENT;
            } else {
                exStyle &= ~WS_EX_TRANSPARENT;
            }
            SetWindowLongPtrW(m_hwnd, GWL_EXSTYLE, exStyle);
        }
    }

    void WidgetWindow::SetLocked(bool locked) {
        m_locked = locked;
        m_config.locked = locked;
        if (m_hwnd) {
            InvalidateRect(m_hwnd, nullptr, FALSE);
        }
    }

    void WidgetWindow::DrawHoverToolbar(float widgetWidth) {
        if (!m_hoverToolbarVisible || !m_renderer) return;

        float pillW = 46.0f;
        float pillH = 18.0f;
        float pillX = widgetWidth - pillW - 12.0f;
        float pillY = 12.0f;

        // Save hit rects
        m_toolbarLockRect = {
            static_cast<LONG>(pillX),
            static_cast<LONG>(pillY),
            static_cast<LONG>(pillX + 23.0f),
            static_cast<LONG>(pillY + pillH)
        };
        m_toolbarCloseRect = {
            static_cast<LONG>(pillX + 23.0f),
            static_cast<LONG>(pillY),
            static_cast<LONG>(pillX + pillW),
            static_cast<LONG>(pillY + pillH)
        };

        // Draw pill background
        D2D1_ROUNDED_RECT pillRect{
            D2D1::RectF(pillX, pillY, pillX + pillW, pillY + pillH),
            3.0f,
            3.0f
        };
        m_renderer->FillRoundedRectangle(pillRect, Colors::BgLight);
        m_renderer->DrawRoundedRectangle(pillRect, Colors::BorderLight, 1.0f);

        // Divider line between Lock and Close
        m_renderer->DrawLine(
            D2D1::Point2F(pillX + 23.0f, pillY + 2.0f),
            D2D1::Point2F(pillX + 23.0f, pillY + pillH - 2.0f),
            Colors::BorderLight,
            1.0f
        );

        // Lock button glyph
        D2D1_RECT_F lockTextRect = D2D1::RectF(pillX, pillY, pillX + 23.0f, pillY + pillH);
        m_renderer->DrawText(
            m_locked ? L"L" : L"U",
            lockTextRect,
            m_locked ? Colors::FgLight : Colors::MutedLight,
            m_formatToolbar.Get()
        );

        // Close button glyph
        D2D1_RECT_F closeTextRect = D2D1::RectF(pillX + 23.0f, pillY, pillX + pillW, pillY + pillH);
        m_renderer->DrawText(
            L"\u00D7",
            closeTextRect,
            Colors::MutedLight,
            m_formatToolbar.Get()
        );
    }

    void WidgetWindow::ShowContextMenu(int screenX, int screenY) {
        HMENU hMenu = CreatePopupMenu();
        if (!hMenu) return;

        HMENU hSubOpacity = CreatePopupMenu();

        // Lock Position
        UINT lockFlags = MF_STRING | (m_locked ? MF_CHECKED : MF_UNCHECKED);
        AppendMenuW(hMenu, lockFlags, ID_MENU_LOCK, L"Lock Position");

        // Always on Top
        UINT aotFlags = MF_STRING | (m_config.alwaysOnTop ? MF_CHECKED : MF_UNCHECKED);
        AppendMenuW(hMenu, aotFlags, ID_MENU_ALWAYSONTOP, L"Always on Top");

        AppendMenuW(hMenu, MF_SEPARATOR, 0, nullptr);

        // Opacity Submenu
        UINT op100Flags = MF_STRING | (m_config.opacity >= 0.95f ? MF_CHECKED : MF_UNCHECKED);
        UINT op75Flags  = MF_STRING | (m_config.opacity >= 0.70f && m_config.opacity < 0.85f ? MF_CHECKED : MF_UNCHECKED);
        UINT op50Flags  = MF_STRING | (m_config.opacity >= 0.45f && m_config.opacity < 0.60f ? MF_CHECKED : MF_UNCHECKED);

        AppendMenuW(hSubOpacity, op100Flags, ID_MENU_OPACITY_100, L"100%");
        AppendMenuW(hSubOpacity, op75Flags, ID_MENU_OPACITY_75, L"75%");
        AppendMenuW(hSubOpacity, op50Flags, ID_MENU_OPACITY_50, L"50%");
        AppendMenuW(hMenu, MF_POPUP, reinterpret_cast<UINT_PTR>(hSubOpacity), L"Opacity");

        // Click-Through
        UINT ctFlags = MF_STRING | (m_config.clickThrough ? MF_CHECKED : MF_UNCHECKED);
        AppendMenuW(hMenu, ctFlags, ID_MENU_CLICKTHROUGH, L"Click-Through");

        AppendMenuW(hMenu, MF_SEPARATOR, 0, nullptr);
        AppendMenuW(hMenu, MF_STRING, ID_MENU_OPEN_MANAGER, L"Open XE Manager");
        AppendMenuW(hMenu, MF_STRING, ID_MENU_CLOSE, L"Close Widget");

        SetForegroundWindow(m_hwnd);
        UINT cmd = TrackPopupMenu(
            hMenu,
            TPM_RETURNCMD | TPM_RIGHTBUTTON | TPM_NONOTIFY,
            screenX, screenY,
            0,
            m_hwnd,
            nullptr
        );

        DestroyMenu(hSubOpacity);
        DestroyMenu(hMenu);

        switch (cmd) {
            case ID_MENU_LOCK:
                SetLocked(!m_locked);
                break;
            case ID_MENU_ALWAYSONTOP:
                SetAlwaysOnTop(!m_config.alwaysOnTop);
                break;
            case ID_MENU_OPACITY_100:
                SetOpacity(1.0f);
                break;
            case ID_MENU_OPACITY_75:
                SetOpacity(0.75f);
                break;
            case ID_MENU_OPACITY_50:
                SetOpacity(0.50f);
                break;
            case ID_MENU_CLICKTHROUGH:
                SetClickThrough(!m_config.clickThrough);
                break;
            case ID_MENU_OPEN_MANAGER: {
                HWND hManager = FindWindowW(L"XEWidgets_Native_Class", nullptr);
                if (hManager) {
                    ShowWindow(hManager, SW_SHOW);
                    SetForegroundWindow(hManager);
                }
                break;
            }
            case ID_MENU_CLOSE:
                Close();
                break;
            default:
                break;
        }
    }

    LRESULT CALLBACK WidgetWindow::StaticWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        WidgetWindow* widget = nullptr;

        if (uMsg == WM_NCCREATE) {
            auto* cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
            widget = reinterpret_cast<WidgetWindow*>(cs->lpCreateParams);
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(widget));
            if (widget) {
                widget->m_hwnd = hwnd;
            }
        } else {
            widget = reinterpret_cast<WidgetWindow*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
        }

        if (widget) {
            return widget->HandleMessage(uMsg, wParam, lParam);
        }

        return DefWindowProcW(hwnd, uMsg, wParam, lParam);
    }

    LRESULT WidgetWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
        // Allow subclass to intercept or handle specific widget messages
        LRESULT subResult = HandleWidgetMessage(uMsg, wParam, lParam);
        if (subResult != 0) {
            return subResult;
        }

        switch (uMsg) {
            case WM_NCHITTEST: {
                if (m_locked) {
                    return HTCLIENT;
                }
                POINT pt{ GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
                ScreenToClient(m_hwnd, &pt);

                if (m_hoverToolbarVisible) {
                    if (PtInRect(&m_toolbarLockRect, pt) || PtInRect(&m_toolbarCloseRect, pt)) {
                        return HTCLIENT;
                    }
                }

                if (IsInInteractiveArea(pt.x, pt.y)) {
                    return HTCLIENT;
                }

                return HTCAPTION;
            }

            case WM_LBUTTONDOWN: {
                int x = GET_X_LPARAM(lParam);
                int y = GET_Y_LPARAM(lParam);
                POINT pt{ x, y };

                if (m_hoverToolbarVisible) {
                    if (PtInRect(&m_toolbarCloseRect, pt)) {
                        Close();
                        return 0;
                    }
                    if (PtInRect(&m_toolbarLockRect, pt)) {
                        SetLocked(!m_locked);
                        return 0;
                    }
                }

                OnLButtonDown(x, y);
                return 0;
            }

            case WM_LBUTTONUP: {
                OnLButtonUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
                return 0;
            }

            case WM_MOUSEMOVE: {
                if (!m_mouseTracking) {
                    TRACKMOUSEEVENT tme{};
                    tme.cbSize = sizeof(TRACKMOUSEEVENT);
                    tme.dwFlags = TME_LEAVE;
                    tme.hwndTrack = m_hwnd;
                    TrackMouseEvent(&tme);
                    m_mouseTracking = true;
                }

                if (!m_hoverToolbarVisible) {
                    m_hoverToolbarVisible = true;
                    InvalidateRect(m_hwnd, nullptr, FALSE);
                }

                OnMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
                return 0;
            }

            case WM_NCMOUSEMOVE: {
                if (!m_hoverToolbarVisible) {
                    m_hoverToolbarVisible = true;
                    InvalidateRect(m_hwnd, nullptr, FALSE);
                }

                TRACKMOUSEEVENT tme{};
                tme.cbSize = sizeof(TRACKMOUSEEVENT);
                tme.dwFlags = TME_LEAVE | TME_NONCLIENT;
                tme.hwndTrack = m_hwnd;
                TrackMouseEvent(&tme);
                return DefWindowProcW(m_hwnd, uMsg, wParam, lParam);
            }

            case WM_MOUSELEAVE:
            case WM_NCMOUSELEAVE: {
                m_mouseTracking = false;
                if (m_hoverToolbarVisible) {
                    m_hoverToolbarVisible = false;
                    InvalidateRect(m_hwnd, nullptr, FALSE);
                }
                OnMouseLeave();
                return 0;
            }

            case WM_RBUTTONUP: {
                POINT pt{ GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
                ClientToScreen(m_hwnd, &pt);
                ShowContextMenu(pt.x, pt.y);
                return 0;
            }

            case WM_NCRBUTTONUP: {
                int screenX = GET_X_LPARAM(lParam);
                int screenY = GET_Y_LPARAM(lParam);
                ShowContextMenu(screenX, screenY);
                return 0;
            }

            case WM_TIMER: {
                OnTick(wParam);
                return 0;
            }

            case WM_PAINT: {
                PAINTSTRUCT ps{};
                BeginPaint(m_hwnd, &ps);
                if (m_renderer) {
                    m_renderer->SetActiveWindow(m_hwnd);
                    m_renderer->BeginDraw();
                    OnRender();
                    HRESULT hr = m_renderer->EndDraw();
                    if (hr == D2DERR_RECREATE_TARGET) {
                        m_renderer->CreateDeviceResources(m_hwnd);
                    }
                }
                EndPaint(m_hwnd, &ps);
                return 0;
            }

            case WM_SIZE: {
                UINT width = LOWORD(lParam);
                UINT height = HIWORD(lParam);
                if (m_renderer) {
                    m_renderer->Resize(m_hwnd, width, height);
                }
                InvalidateRect(m_hwnd, nullptr, FALSE);
                return 0;
            }

            case WM_DPICHANGED: {
                UINT dpi = HIWORD(wParam);
                if (m_renderer) {
                    m_renderer->UpdateDpi(dpi, m_hwnd);
                }
                auto* rect = reinterpret_cast<RECT*>(lParam);
                SetWindowPos(
                    m_hwnd,
                    nullptr,
                    rect->left,
                    rect->top,
                    rect->right - rect->left,
                    rect->bottom - rect->top,
                    SWP_NOZORDER | SWP_NOACTIVATE
                );
                return 0;
            }

            case WM_ERASEBKGND:
                return 1;

            case WM_WINDOWPOSCHANGED: {
                auto* wp = reinterpret_cast<WINDOWPOS*>(lParam);
                if (!(wp->flags & SWP_NOMOVE)) {
                    m_config.x = wp->x;
                    m_config.y = wp->y;
                }
                return 0;
            }

            case WM_DESTROY: {
                if (m_hwnd) {
                    RECT r{};
                    GetWindowRect(m_hwnd, &r);
                    m_config.x = r.left;
                    m_config.y = r.top;
                }
                if (m_renderer) {
                    m_renderer->DiscardDeviceResources(m_hwnd);
                }
                m_hwnd = nullptr;
                return 0;
            }

            default:
                break;
        }

        return DefWindowProcW(m_hwnd, uMsg, wParam, lParam);
    }

}
