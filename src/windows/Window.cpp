#include "Window.hpp"
#include "WindowManager.hpp"
#include "../core/Logger.hpp"
#include "../manager/TrayManager.hpp"
#include <algorithm>
#include <cmath>

namespace xe {

    Window::Window() = default;

    Window::~Window() {
        Close();
    }

    bool Window::RegisterWindowClass() {
        if (s_classRegistered) return true;

        WNDCLASSEXW wc{};
        wc.cbSize = sizeof(WNDCLASSEXW);
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = StaticWndProc;
        wc.hInstance = GetModuleHandleW(nullptr);
        wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        wc.hbrBackground = nullptr;
        wc.lpszClassName = s_className;

        if (!RegisterClassExW(&wc)) {
            XE_LOG_ERROR("Failed to register Win32 Window Class");
            return false;
        }

        s_classRegistered = true;
        return true;
    }

    bool Window::Create(const WindowConfig& config, D2DRenderer* renderer) {
        if (!RegisterWindowClass()) return false;

        m_config = config;
        m_renderer = renderer;

        DWORD dwStyle = WS_OVERLAPPEDWINDOW;
        DWORD dwExStyle = WS_EX_APPWINDOW;

        if (config.frameless) {
            dwStyle = WS_POPUP;
        }
        if (config.alwaysOnTop) {
            dwExStyle |= WS_EX_TOPMOST;
        }

        int screenW = GetSystemMetrics(SM_CXSCREEN);
        int screenH = GetSystemMetrics(SM_CYSCREEN);
        int posX = (screenW - config.width) / 2;
        int posY = (screenH - config.height) / 2;

        m_hwnd = CreateWindowExW(
            dwExStyle,
            s_className,
            config.title.c_str(),
            dwStyle,
            posX,
            posY,
            config.width,
            config.height,
            nullptr,
            nullptr,
            GetModuleHandleW(nullptr),
            this
        );

        if (!m_hwnd) {
            XE_LOG_ERROR("Failed to create native Win32 window");
            return false;
        }

        // Apply Windows 11/10 Immersive Dark Mode for window title bar
        BOOL useDarkMode = FALSE; // Manager window is Light Theme by default
        DwmSetWindowAttribute(m_hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &useDarkMode, sizeof(useDarkMode));

        // Start background input state & dead widget cleanup timer (40ms)
        SetTimer(m_hwnd, 7777, 40, nullptr);

        if (m_renderer) {
            if (!m_renderer->CreateDeviceResources(m_hwnd)) {
                XE_LOG_ERROR("Failed to bind Direct2D to window");
                return false;
            }
            InitTypography();
        }

        XE_LOG_INFO("Native Win32 manager window created successfully");
        return true;
    }

    void Window::InitTypography() {
        if (m_typographyInitialized || !m_renderer) return;

        m_formatHeroTitle = m_renderer->CreateTextFormat(
            L"Geist Pixel", 26.0f, DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_NEAR
        );

        m_formatLogo = m_renderer->CreateTextFormat(
            L"Geist Pixel", 22.0f, DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_NEAR
        );

        m_formatSectionTitle = m_renderer->CreateTextFormat(
            L"Geist Mono", 9.5f, DWRITE_FONT_WEIGHT_MEDIUM, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER
        );

        m_formatPageTitle = m_renderer->CreateTextFormat(
            L"Geist Sans", 14.0f, DWRITE_FONT_WEIGHT_SEMI_BOLD, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_NEAR
        );

        m_formatSubtitle = m_renderer->CreateTextFormat(
            L"Geist Sans", 11.5f, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_NEAR
        );

        m_formatBody = m_renderer->CreateTextFormat(
            L"Geist Sans", 11.5f, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_NEAR
        );

        m_formatMono = m_renderer->CreateTextFormat(
            L"Geist Mono", 9.0f, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER
        );

        m_formatMonoMedium = m_renderer->CreateTextFormat(
            L"Geist Mono", 9.5f, DWRITE_FONT_WEIGHT_MEDIUM, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER
        );

        m_formatStatNum = m_renderer->CreateTextFormat(
            L"Geist Mono", 24.0f, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER
        );

        m_formatStatLabel = m_renderer->CreateTextFormat(
            L"Geist Sans", 9.5f, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER
        );

        m_formatCardTitle = m_renderer->CreateTextFormat(
            L"Geist Sans", 12.0f, DWRITE_FONT_WEIGHT_SEMI_BOLD, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER
        );

        m_formatCardDesc = m_renderer->CreateTextFormat(
            L"Geist Sans", 10.5f, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_NEAR
        );

        m_formatCardTag = m_renderer->CreateTextFormat(
            L"Geist Mono", 8.5f, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER
        );

        m_formatButton = m_renderer->CreateTextFormat(
            L"Geist Mono", 9.5f, DWRITE_FONT_WEIGHT_MEDIUM, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER
        );

        m_formatButtonCenter = m_renderer->CreateTextFormat(
            L"Geist Mono", 9.5f, DWRITE_FONT_WEIGHT_MEDIUM, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER
        );

        m_typographyInitialized = true;
    }

    void Window::Show(int nCmdShow) {
        if (m_hwnd) {
            ShowWindow(m_hwnd, nCmdShow);
            UpdateWindow(m_hwnd);
        }
    }

    void Window::Close() {
        if (m_hwnd) {
            if (m_renderer) {
                m_renderer->DiscardDeviceResources(m_hwnd);
            }
            DestroyWindow(m_hwnd);
            m_hwnd = nullptr;
        }
    }

    LRESULT CALLBACK Window::StaticWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        Window* window = nullptr;

        if (uMsg == WM_NCCREATE) {
            auto* cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
            window = reinterpret_cast<Window*>(cs->lpCreateParams);
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
            if (window) {
                window->m_hwnd = hwnd;
            }
        } else {
            window = reinterpret_cast<Window*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
        }

        if (window) {
            return window->HandleMessage(uMsg, wParam, lParam);
        }

        return DefWindowProcW(hwnd, uMsg, wParam, lParam);
    }

    void Window::OnLButtonDown(int x, int y) {
        for (const auto& area : m_clickableAreas) {
            if (x >= area.rect.left && x <= area.rect.right &&
                y >= area.rect.top && y <= area.rect.bottom) {
                if (area.onClick) {
                    area.onClick();
                }
                InvalidateRect(m_hwnd, nullptr, FALSE);
                break;
            }
        }
    }

    void Window::OnMouseMove(int x, int y) {
        m_mousePos = { x, y };
        bool isHand = false;
        for (const auto& area : m_clickableAreas) {
            if (x >= area.rect.left && x <= area.rect.right &&
                y >= area.rect.top && y <= area.rect.bottom) {
                isHand = true;
                break;
            }
        }
        SetCursor(LoadCursorW(nullptr, isHand ? IDC_HAND : IDC_ARROW));
    }

    void Window::ToggleTheme() {
        m_isDarkMode = !m_isDarkMode;
        BOOL useDarkMode = m_isDarkMode ? TRUE : FALSE;
        DwmSetWindowAttribute(m_hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &useDarkMode, sizeof(useDarkMode));
        if (m_hwnd) InvalidateRect(m_hwnd, nullptr, FALSE);
    }

    LRESULT Window::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
        switch (uMsg) {
            case WM_PAINT: {
                PAINTSTRUCT ps{};
                BeginPaint(m_hwnd, &ps);
                if (m_renderer) {
                    m_renderer->SetActiveWindow(m_hwnd);
                }
                OnRender();
                EndPaint(m_hwnd, &ps);
                return 0;
            }

            case WM_KEYDOWN: {
                bool ctrl = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
                if (ctrl && wParam == '1') { m_activeTab = ManagerTab::Overview; InvalidateRect(m_hwnd, nullptr, FALSE); return 0; }
                if (ctrl && wParam == '2') { m_activeTab = ManagerTab::Library; InvalidateRect(m_hwnd, nullptr, FALSE); return 0; }
                if (ctrl && wParam == '3') { m_activeTab = ManagerTab::ActiveWidgets; InvalidateRect(m_hwnd, nullptr, FALSE); return 0; }
                if (ctrl && wParam == '4') { m_activeTab = ManagerTab::Settings; InvalidateRect(m_hwnd, nullptr, FALSE); return 0; }
                if (ctrl && wParam == 'K') { m_activeTab = ManagerTab::Library; InvalidateRect(m_hwnd, nullptr, FALSE); return 0; }
                if (wParam == VK_ESCAPE) { ShowWindow(m_hwnd, SW_HIDE); return 0; }
                break;
            }

            case WM_LBUTTONDOWN: {
                int x = LOWORD(lParam);
                int y = HIWORD(lParam);
                OnLButtonDown(x, y);
                return 0;
            }

            case WM_MOUSEMOVE: {
                int x = LOWORD(lParam);
                int y = HIWORD(lParam);
                OnMouseMove(x, y);
                return 0;
            }

            case WM_SETCURSOR: {
                if (LOWORD(lParam) == HTCLIENT) {
                    return TRUE;
                }
                break;
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

            case WM_CLOSE:
                ShowWindow(m_hwnd, SW_HIDE);
                return 0;

            case WM_TIMER:
                if (wParam == 7777) {
                    WindowManager::Instance().UpdateInputState();
                    return 0;
                }
                break;

            case WM_APP + 1:
                TrayManager::Instance().HandleTrayMessage(wParam, lParam);
                return 0;

            case WM_APP + 2:
                ShowWindow(m_hwnd, SW_SHOW);
                SetForegroundWindow(m_hwnd);
                WindowManager::Instance().ShowAllWidgets();
                return 0;

            case WM_DESTROY:
                KillTimer(m_hwnd, 7777);
                PostQuitMessage(0);
                return 0;

            default:
                break;
        }

        return DefWindowProcW(m_hwnd, uMsg, wParam, lParam);
    }

    void Window::RenderSidebar(float /*width*/, float height) {
        float sidebarW = 190.0f;

        // Background
        m_renderer->FillRectangle(D2D1::RectF(0.0f, 0.0f, sidebarW, height), Colors::SurfaceLight);
        m_renderer->DrawLine(D2D1::Point2F(sidebarW, 0.0f), D2D1::Point2F(sidebarW, height), Colors::BorderLight, 1.0f);

        // Logo
        D2D1_RECT_F logoR = D2D1::RectF(20.0f, 20.0f, sidebarW - 10.0f, 48.0f);
        m_renderer->DrawText(L"XE.", logoR, Colors::FgLight, m_formatLogo.Get());

        D2D1_RECT_F subLogoR = D2D1::RectF(20.0f, 48.0f, sidebarW - 10.0f, 64.0f);
        m_renderer->DrawText(L"XE WIDGETS", subLogoR, Colors::MutedLight, m_formatMono.Get());

        // Category label
        D2D1_RECT_F catR = D2D1::RectF(20.0f, 88.0f, sidebarW - 10.0f, 104.0f);
        m_renderer->DrawText(L"WORKSPACE", catR, Colors::FaintLight, m_formatSectionTitle.Get());

        // Nav tabs
        struct TabItem {
            ManagerTab tab;
            const wchar_t* num;
            const wchar_t* label;
        };

        TabItem items[] = {
            { ManagerTab::Overview,      L"01", L"Overview" },
            { ManagerTab::Library,       L"02", L"Widget Library" },
            { ManagerTab::ActiveWidgets, L"03", L"Active Widgets" },
            { ManagerTab::Settings,      L"04", L"Settings" }
        };

        float itemY = 112.0f;
        float itemH = 34.0f;

        for (const auto& it : items) {
            bool isActive = (m_activeTab == it.tab);
            D2D1_RECT_F itemRect = D2D1::RectF(0.0f, itemY, sidebarW, itemY + itemH);

            if (isActive) {
                // Active item background & black left indicator bar
                m_renderer->FillRectangle(itemRect, Colors::BgLight);
                m_renderer->FillRectangle(D2D1::RectF(0.0f, itemY, 2.5f, itemY + itemH), Colors::FgLight);
            }

            // Clickable area
            m_clickableAreas.push_back({
                itemRect,
                [this, t = it.tab]() {
                    m_activeTab = t;
                }
            });

            // Tab number
            D2D1_RECT_F numR = D2D1::RectF(20.0f, itemY, 40.0f, itemY + itemH);
            m_renderer->DrawText(it.num, numR, isActive ? Colors::FgLight : Colors::FaintLight, m_formatMonoMedium.Get());

            // Tab label
            D2D1_RECT_F labelR = D2D1::RectF(48.0f, itemY, sidebarW - 8.0f, itemY + itemH);
            m_renderer->DrawText(it.label, labelR, isActive ? Colors::FgLight : Colors::MutedLight, m_formatBody.Get());

            itemY += itemH + 2.0f;
        }

        // Sidebar Footer
        float footerY = height - 54.0f;
        D2D1_RECT_F f1 = D2D1::RectF(20.0f, footerY, sidebarW - 10.0f, footerY + 14.0f);
        m_renderer->DrawText(L"XE Widgets", f1, Colors::FaintLight, m_formatMono.Get());

        D2D1_RECT_F f2 = D2D1::RectF(20.0f, footerY + 14.0f, sidebarW - 10.0f, footerY + 28.0f);
        m_renderer->DrawText(L"v2.0.0", f2, Colors::FaintLight, m_formatMono.Get());

        D2D1_RECT_F f3 = D2D1::RectF(20.0f, footerY + 28.0f, sidebarW - 10.0f, footerY + 42.0f);
        m_renderer->DrawText(L"Development", f3, Colors::FaintLight, m_formatMono.Get());
    }

    void Window::RenderTopHeader(float leftX, float width) {
        float headerH = 52.0f;

        // Bottom border
        m_renderer->DrawLine(D2D1::Point2F(leftX, headerH), D2D1::Point2F(width, headerH), Colors::BorderLight, 1.0f);

        // Titles per tab
        const wchar_t* titleStr = L"Overview";
        const wchar_t* subStr = L"Your desktop workspace at a glance.";

        if (m_activeTab == ManagerTab::Library) {
            titleStr = L"Widget Library";
            subStr = L"Discover, activate, and launch native desktop widgets.";
        } else if (m_activeTab == ManagerTab::ActiveWidgets) {
            titleStr = L"Active Widgets";
            subStr = L"Manage active canvas widgets and independent floating desktop windows.";
        } else if (m_activeTab == ManagerTab::Settings) {
            titleStr = L"Settings";
            subStr = L"Configure appearance, behaviour, and preferences.";
        }

        D2D1_RECT_F titleR = D2D1::RectF(leftX + 24.0f, 10.0f, width - 260.0f, 28.0f);
        m_renderer->DrawText(titleStr, titleR, Colors::FgLight, m_formatPageTitle.Get());

        D2D1_RECT_F subR = D2D1::RectF(leftX + 24.0f, 28.0f, width - 260.0f, 46.0f);
        m_renderer->DrawText(subStr, subR, Colors::MutedLight, m_formatSubtitle.Get());

        // Right side search box & icon buttons
        float curRight = width - 20.0f;

        // Theme button (Moon)
        float moonX = curRight - 28.0f;
        D2D1_ROUNDED_RECT moonR{ D2D1::RectF(moonX, 12.0f, moonX + 26.0f, 38.0f), 3.0f, 3.0f };
        m_renderer->FillRoundedRectangle(moonR, Colors::SurfaceLight);
        m_renderer->DrawRoundedRectangle(moonR, Colors::BorderLight, 1.0f);
        D2D1_RECT_F moonTxtR = D2D1::RectF(moonX, 12.0f, moonX + 26.0f, 38.0f);
        m_renderer->DrawText(L"◑", moonTxtR, Colors::MutedLight, m_formatButtonCenter.Get());
        m_clickableAreas.push_back({ D2D1::RectF(moonX, 12.0f, moonX + 26.0f, 38.0f), [this]() { ToggleTheme(); } });
        curRight = moonX - 8.0f;

        // Notification Bell button
        float bellX = curRight - 28.0f;
        D2D1_ROUNDED_RECT bellR{ D2D1::RectF(bellX, 12.0f, bellX + 26.0f, 38.0f), 3.0f, 3.0f };
        m_renderer->FillRoundedRectangle(bellR, Colors::SurfaceLight);
        m_renderer->DrawRoundedRectangle(bellR, Colors::BorderLight, 1.0f);
        D2D1_RECT_F bellTxtR = D2D1::RectF(bellX, 12.0f, bellX + 26.0f, 38.0f);
        m_renderer->DrawText(L"🔔", bellTxtR, Colors::MutedLight, m_formatButtonCenter.Get());
        curRight = bellX - 8.0f;

        // Search box: "Search widgets... ⌘K"
        float searchW = 160.0f;
        float searchX = curRight - searchW;
        D2D1_ROUNDED_RECT searchR{ D2D1::RectF(searchX, 12.0f, searchX + searchW, 38.0f), 3.0f, 3.0f };
        m_renderer->FillRoundedRectangle(searchR, Colors::SurfaceLight);
        m_renderer->DrawRoundedRectangle(searchR, Colors::BorderLight, 1.0f);

        D2D1_RECT_F searchTxtR = D2D1::RectF(searchX + 8.0f, 12.0f, searchX + searchW - 8.0f, 38.0f);
        m_renderer->DrawText(L"🔍 Search widgets...   ⌘K", searchTxtR, Colors::FaintLight, m_formatMono.Get());
        m_clickableAreas.push_back({ D2D1::RectF(searchX, 12.0f, searchX + searchW, 38.0f), [this]() { m_activeTab = ManagerTab::Library; } });
    }

    void Window::RenderOverviewTab(float leftX, float topY, float width, float /*height*/) {
        float contentX = leftX + 28.0f;
        float contentW = width - contentX - 28.0f;

        // 1. Hero Title
        float curY = topY + 28.0f;
        D2D1_RECT_F heroR = D2D1::RectF(contentX, curY, contentX + contentW, curY + 68.0f);
        m_renderer->DrawText(L"YOUR DESKTOP.\nYOUR RULES.", heroR, Colors::FgLight, m_formatHeroTitle.Get());
        curY += 74.0f;

        // 2. Hero Subtitle
        D2D1_RECT_F descR = D2D1::RectF(contentX, curY, contentX + contentW, curY + 36.0f);
        m_renderer->DrawText(
            L"A minimal companion for your workflow. Precision widgets, zero distractions, complete control.",
            descR,
            Colors::MutedLight,
            m_formatSubtitle.Get()
        );
        curY += 38.0f;

        // 3. Action Buttons
        float btnW1 = 120.0f;
        float btnH = 30.0f;
        D2D1_ROUNDED_RECT btn1R{ D2D1::RectF(contentX, curY, contentX + btnW1, curY + btnH), 3.0f, 3.0f };
        m_renderer->FillRoundedRectangle(btn1R, Colors::FgLight);
        m_renderer->DrawText(L"+ Browse Widgets", D2D1::RectF(contentX, curY, contentX + btnW1, curY + btnH), Colors::BgLight, m_formatButtonCenter.Get());
        m_clickableAreas.push_back({ D2D1::RectF(contentX, curY, contentX + btnW1, curY + btnH), [this]() { m_activeTab = ManagerTab::Library; } });

        float btnW2 = 124.0f;
        float btn2X = contentX + btnW1 + 8.0f;
        D2D1_ROUNDED_RECT btn2R{ D2D1::RectF(btn2X, curY, btn2X + btnW2, curY + btnH), 3.0f, 3.0f };
        m_renderer->DrawRoundedRectangle(btn2R, Colors::BorderLight, 1.0f);
        m_renderer->DrawText(L"Manage Active →", D2D1::RectF(btn2X, curY, btn2X + btnW2, curY + btnH), Colors::FgLight, m_formatButtonCenter.Get());
        m_clickableAreas.push_back({ D2D1::RectF(btn2X, curY, btn2X + btnW2, curY + btnH), [this]() { m_activeTab = ManagerTab::ActiveWidgets; } });

        curY += btnH + 34.0f;

        // Divider Line
        m_renderer->DrawLine(D2D1::Point2F(contentX, curY), D2D1::Point2F(contentX + contentW, curY), Colors::BorderLight, 1.0f);
        curY += 16.0f;

        // 4. Section 01 — STATISTICS
        D2D1_RECT_F sec1R = D2D1::RectF(contentX, curY, contentX + contentW, curY + 16.0f);
        m_renderer->DrawText(L"01 — STATISTICS", sec1R, Colors::MutedLight, m_formatSectionTitle.Get());
        curY += 22.0f;

        // 4 Stat Cards in a row
        int runningCount = 0;
        for (const auto& id : { L"clock", L"monitor", L"music", L"weather" }) {
            if (WindowManager::Instance().IsWidgetRunning(id)) runningCount++;
        }

        struct StatBox {
            std::wstring num;
            const wchar_t* label;
        };
        StatBox stats[4] = {
            { L"6", L"Total Installed" },
            { L"5", L"Active Widgets" },
            { std::to_wstring(runningCount), L"Open on Desktop" },
            { L"1", L"Available Widgets" }
        };

        float statCardW = contentW / 4.0f;
        float statCardH = 74.0f;

        for (int i = 0; i < 4; ++i) {
            float sx = contentX + i * statCardW;
            D2D1_RECT_F boxR = D2D1::RectF(sx, curY, sx + statCardW, curY + statCardH);
            m_renderer->DrawRectangle(boxR, Colors::BorderLight, 1.0f);

            D2D1_RECT_F numR = D2D1::RectF(sx + 14.0f, curY + 10.0f, sx + statCardW - 10.0f, curY + 44.0f);
            m_renderer->DrawText(stats[i].num, numR, Colors::FgLight, m_formatStatNum.Get());

            D2D1_RECT_F lblR = D2D1::RectF(sx + 14.0f, curY + 46.0f, sx + statCardW - 10.0f, curY + 66.0f);
            m_renderer->DrawText(stats[i].label, lblR, Colors::MutedLight, m_formatStatLabel.Get());
        }

        curY += statCardH + 34.0f;

        // 5. Section 02 — ACTIVE WIDGETS
        D2D1_RECT_F sec2R = D2D1::RectF(contentX, curY, contentX + 200.0f, curY + 16.0f);
        m_renderer->DrawText(L"02 — ACTIVE WIDGETS", sec2R, Colors::MutedLight, m_formatSectionTitle.Get());

        D2D1_RECT_F manageR = D2D1::RectF(contentX + contentW - 120.0f, curY, contentX + contentW, curY + 16.0f);
        m_renderer->DrawText(L"MANAGE ALL →", manageR, Colors::FaintLight, m_formatSectionTitle.Get());
        m_clickableAreas.push_back({ manageR, [this]() { m_activeTab = ManagerTab::ActiveWidgets; } });
        curY += 24.0f;

        // Active widget list items
        struct WidgetRow {
            const wchar_t* id;
            const wchar_t* title;
            const wchar_t* tag;
            const wchar_t* icon;
        };

        WidgetRow rows[] = {
            { L"clock",   L"XE Precision Clock",       L"PRODUCTIVITY", L"🕒" },
            { L"monitor", L"XE System Telemetry",      L"SYSTEM",       L"⚡" },
            { L"scratch", L"XE Quick Scratchpad",      L"PRODUCTIVITY", L"📝" },
            { L"music",   L"XE Ambient Sound & Media", L"MEDIA",        L"🎵" }
        };

        float rowH = 38.0f;
        for (const auto& r : rows) {
            D2D1_RECT_F rowRect = D2D1::RectF(contentX, curY, contentX + contentW, curY + rowH);
            m_renderer->DrawLine(D2D1::Point2F(contentX, curY), D2D1::Point2F(contentX + contentW, curY), Colors::BorderLight, 1.0f);

            // Icon + Title
            std::wstring fullTitle = std::wstring(L"• ") + r.icon + L"  " + r.title;
            D2D1_RECT_F titleR = D2D1::RectF(contentX + 8.0f, curY, contentX + 300.0f, curY + rowH);
            m_renderer->DrawText(fullTitle, titleR, Colors::FgLight, m_formatBody.Get());

            // Tag
            D2D1_RECT_F tagR = D2D1::RectF(contentX + contentW - 180.0f, curY, contentX + contentW - 80.0f, curY + rowH);
            m_renderer->DrawText(r.tag, tagR, Colors::FaintLight, m_formatMono.Get());

            // Float button
            bool isRunning = WindowManager::Instance().IsWidgetRunning(r.id);
            D2D1_RECT_F floatR = D2D1::RectF(contentX + contentW - 74.0f, curY, contentX + contentW, curY + rowH);
            m_renderer->DrawText(isRunning ? L"↗ ACTIVE" : L"↗ FLOAT", floatR, isRunning ? Colors::FgLight : Colors::MutedLight, m_formatMonoMedium.Get());

            m_clickableAreas.push_back({
                rowRect,
                [id = r.id]() {
                    WindowManager::Instance().ToggleWidgetById(id);
                }
            });

            curY += rowH;
        }
        m_renderer->DrawLine(D2D1::Point2F(contentX, curY), D2D1::Point2F(contentX + contentW, curY), Colors::BorderLight, 1.0f);
    }

    void Window::RenderLibraryTab(float leftX, float topY, float width, float /*height*/) {
        float contentX = leftX + 28.0f;
        float contentW = width - contentX - 28.0f;

        // 1. Filter tabs bar
        float curY = topY + 16.0f;
        const wchar_t* filters[] = { L"All", L"Productivity", L"System", L"Media", L"Utility" };
        float fx = contentX;

        for (const auto* flt : filters) {
            bool isSel = (m_libraryFilter == flt);
            float tabW = 76.0f;
            D2D1_RECT_F tabR = D2D1::RectF(fx, curY, fx + tabW, curY + 26.0f);
            m_renderer->DrawText(flt, tabR, isSel ? Colors::FgLight : Colors::MutedLight, m_formatBody.Get());

            if (isSel) {
                // Bottom underline
                m_renderer->DrawLine(D2D1::Point2F(fx, curY + 24.0f), D2D1::Point2F(fx + 24.0f, curY + 24.0f), Colors::FgLight, 2.0f);
            }

            m_clickableAreas.push_back({
                tabR,
                [this, s = std::wstring(flt)]() {
                    m_libraryFilter = s;
                }
            });

            fx += tabW;
        }

        // Filter catalog search input on the right
        float catW = 160.0f;
        float catX = contentX + contentW - catW;
        D2D1_ROUNDED_RECT catR{ D2D1::RectF(catX, curY, catX + catW, curY + 24.0f), 3.0f, 3.0f };
        m_renderer->FillRoundedRectangle(catR, Colors::SurfaceLight);
        m_renderer->DrawRoundedRectangle(catR, Colors::BorderLight, 1.0f);
        m_renderer->DrawText(L"🔍 Filter catalog...", D2D1::RectF(catX + 8.0f, curY, catX + catW - 8.0f, curY + 24.0f), Colors::FaintLight, m_formatMono.Get());

        curY += 40.0f;

        // 2. Sub-bar: 6 WIDGETS — ALL
        D2D1_RECT_F countR = D2D1::RectF(contentX, curY, contentX + 200.0f, curY + 16.0f);
        m_renderer->DrawText(L"6 WIDGETS — ALL", countR, Colors::FaintLight, m_formatSectionTitle.Get());
        curY += 24.0f;

        // 3. 3-Column x 2-Row Grid of Catalog Cards
        struct CatalogCard {
            const wchar_t* id;
            const wchar_t* title;
            const wchar_t* ver;
            const wchar_t* badge;
            const wchar_t* desc;
            const wchar_t* tags;
            const wchar_t* icon;
            bool isAvailable;
        };

        CatalogCard cards[] = {
            { L"clock",    L"XE Precision Clock",         L"v2.4.0", L"ACTIVE",    L"Multi-timezone desktop clock with stopwatch & focus pomodoro.",          L"Time Pomodoro World Clock Timer",        L"🕒", false },
            { L"monitor",  L"XE System Telemetry",        L"v3.1.2", L"ACTIVE",    L"Live CPU, memory, GPU, disk, and real-time network throughput graphs.",  L"Hardware CPU RAM Network GPU",           L"⚡", false },
            { L"scratch",  L"XE Quick Scratchpad",        L"v1.9.0", L"ACTIVE",    L"Minimalist sticky notes and task checklist.",                            L"Notes Checklist Tasks Productivity",     L"📝", false },
            { L"music",    L"XE Ambient Sound & Media",   L"v2.0.1", L"ACTIVE",    L"Desktop media controller with playback controls.",                       L"Audio Player Media",                     L"🎵", false },
            { L"weather",  L"XE Atmosphere & Forecast",   L"v1.5.0", L"ACTIVE",    L"Real-time meteorological weather report with 4-day forecast.",           L"Weather Climate Forecast Atmosphere",    L"☀️", false },
            { L"calendar", L"XE Event Horizon Calendar",  L"v1.2.0", L"AVAILABLE", L"Compact desktop agenda showing upcoming milestones and schedule.",      L"Calendar Agenda Events Schedule",        L"📅", true }
        };

        float colGap = 12.0f;
        float rowGap = 12.0f;
        float cardW = (contentW - (colGap * 2.0f)) / 3.0f;
        float cardH = 156.0f;

        for (int i = 0; i < 6; ++i) {
            int col = i % 3;
            int row = i / 3;
            float cx = contentX + col * (cardW + colGap);
            float cy = curY + row * (cardH + rowGap);

            D2D1_RECT_F cardR = D2D1::RectF(cx, cy, cx + cardW, cy + cardH);
            m_renderer->DrawRectangle(cardR, Colors::BorderLight, 1.0f);

            // Icon + Badge row
            float topY2 = cy + 10.0f;
            D2D1_RECT_F iconR = D2D1::RectF(cx + 12.0f, topY2, cx + 32.0f, topY2 + 18.0f);
            m_renderer->DrawText(cards[i].icon, iconR, Colors::MutedLight, m_formatBody.Get());

            // Badge
            float badgeW = cards[i].isAvailable ? 62.0f : 50.0f;
            float badgeX = cx + cardW - badgeW - 12.0f;
            D2D1_ROUNDED_RECT badgeR{ D2D1::RectF(badgeX, topY2, badgeX + badgeW, topY2 + 16.0f), 2.0f, 2.0f };
            m_renderer->DrawRoundedRectangle(badgeR, Colors::BorderLight, 1.0f);
            m_renderer->DrawText(cards[i].badge, D2D1::RectF(badgeX, topY2, badgeX + badgeW, topY2 + 16.0f), Colors::FgLight, m_formatButtonCenter.Get());

            // Title + Version
            float textY = topY2 + 22.0f;
            D2D1_RECT_F titleR = D2D1::RectF(cx + 12.0f, textY, cx + cardW - 12.0f, textY + 16.0f);
            std::wstring titleWithVer = std::wstring(cards[i].title) + L" " + cards[i].ver;
            m_renderer->DrawText(titleWithVer, titleR, Colors::FgLight, m_formatCardTitle.Get());

            // Description
            float descY = textY + 18.0f;
            D2D1_RECT_F descR = D2D1::RectF(cx + 12.0f, descY, cx + cardW - 12.0f, descY + 34.0f);
            m_renderer->DrawText(cards[i].desc, descR, Colors::MutedLight, m_formatCardDesc.Get());

            // Tags
            float tagsY = descY + 36.0f;
            D2D1_RECT_F tagsR = D2D1::RectF(cx + 12.0f, tagsY, cx + cardW - 12.0f, tagsY + 14.0f);
            m_renderer->DrawText(cards[i].tags, tagsR, Colors::FaintLight, m_formatCardTag.Get());

            // Buttons: [OPEN ON DESKTOP] [DISABLE / ENABLE]
            float btnY = cy + cardH - 30.0f;
            float btn1W = 120.0f;
            float btnH = 22.0f;

            bool isRunning = WindowManager::Instance().IsWidgetRunning(cards[i].id);
            D2D1_ROUNDED_RECT openR{ D2D1::RectF(cx + 12.0f, btnY, cx + 12.0f + btn1W, btnY + btnH), 2.0f, 2.0f };
            m_renderer->FillRoundedRectangle(openR, Colors::FgLight);
            m_renderer->DrawText(isRunning ? L"↗ OPEN (ACTIVE)" : L"↗ OPEN ON DESKTOP", D2D1::RectF(cx + 12.0f, btnY, cx + 12.0f + btn1W, btnY + btnH), Colors::BgLight, m_formatButtonCenter.Get());

            m_clickableAreas.push_back({
                D2D1::RectF(cx + 12.0f, btnY, cx + 12.0f + btn1W, btnY + btnH),
                [id = cards[i].id]() {
                    WindowManager::Instance().ToggleWidgetById(id);
                }
            });

            // Action 2 button
            float btn2W = 52.0f;
            float btn2X = cx + 12.0f + btn1W + 6.0f;
            D2D1_ROUNDED_RECT disR{ D2D1::RectF(btn2X, btnY, btn2X + btn2W, btnY + btnH), 2.0f, 2.0f };
            m_renderer->DrawRoundedRectangle(disR, Colors::BorderLight, 1.0f);
            m_renderer->DrawText(cards[i].isAvailable ? L"ENABLE" : L"DISABLE", D2D1::RectF(btn2X, btnY, btn2X + btn2W, btnY + btnH), Colors::MutedLight, m_formatButtonCenter.Get());
        }
    }

    void Window::RenderActiveWidgetsTab(float leftX, float topY, float width, float /*height*/) {
        float contentX = leftX + 28.0f;
        float contentW = width - contentX - 28.0f;

        float curY = topY + 16.0f;

        // Subheader count
        int activeDesktop = 0;
        for (const auto& id : { L"clock", L"monitor", L"music", L"weather" }) {
            if (WindowManager::Instance().IsWidgetRunning(id)) activeDesktop++;
        }
        std::wstring countStr = L"ACTIVE CANVAS (5)  ·  ON DESKTOP (" + std::to_wstring(activeDesktop) + L")";
        D2D1_RECT_F countR = D2D1::RectF(contentX, curY, contentX + 300.0f, curY + 16.0f);
        m_renderer->DrawText(countStr, countR, Colors::FaintLight, m_formatSectionTitle.Get());
        curY += 24.0f;

        // List of Active Canvas Widgets
        struct ActiveWidgetRow {
            const wchar_t* id;
            const wchar_t* title;
            const wchar_t* cat;
            const wchar_t* ver;
            const wchar_t* icon;
        };

        ActiveWidgetRow rows[] = {
            { L"clock",   L"XE Precision Clock",       L"PRODUCTIVITY", L"v2.4.0", L"🕒" },
            { L"monitor", L"XE System Telemetry",      L"SYSTEM",       L"v3.1.2", L"⚡" },
            { L"scratch", L"XE Quick Scratchpad",      L"PRODUCTIVITY", L"v1.9.0", L"📝" },
            { L"music",   L"XE Ambient Sound & Media", L"MEDIA",        L"v2.0.1", L"🎵" },
            { L"weather", L"XE Atmosphere & Forecast", L"UTILITY",      L"v1.5.0", L"v1.5.0" }
        };

        float rowH = 38.0f;
        for (const auto& r : rows) {
            D2D1_RECT_F rowRect = D2D1::RectF(contentX, curY, contentX + contentW, curY + rowH);
            m_renderer->DrawLine(D2D1::Point2F(contentX, curY), D2D1::Point2F(contentX + contentW, curY), Colors::BorderLight, 1.0f);

            // Icon + Title
            std::wstring fullTitle = std::wstring(L"• ") + r.icon + L"  " + r.title;
            D2D1_RECT_F titleR = D2D1::RectF(contentX + 8.0f, curY, contentX + 280.0f, curY + rowH);
            m_renderer->DrawText(fullTitle, titleR, Colors::FgLight, m_formatBody.Get());

            // Status tag
            D2D1_RECT_F statusR = D2D1::RectF(contentX + contentW - 280.0f, curY, contentX + contentW - 190.0f, curY + rowH);
            m_renderer->DrawText(L"DASHBOARD ONLY", statusR, Colors::FaintLight, m_formatMono.Get());

            // Category tag
            D2D1_RECT_F catR = D2D1::RectF(contentX + contentW - 180.0f, curY, contentX + contentW - 110.0f, curY + rowH);
            m_renderer->DrawText(r.cat, catR, Colors::FaintLight, m_formatMono.Get());

            // Version
            D2D1_RECT_F verR = D2D1::RectF(contentX + contentW - 100.0f, curY, contentX + contentW - 60.0f, curY + rowH);
            m_renderer->DrawText(r.ver, verR, Colors::FaintLight, m_formatMono.Get());

            // Action icons: [↗] [📌] [×]
            float iconX = contentX + contentW - 54.0f;
            D2D1_RECT_F floatBtnR = D2D1::RectF(iconX, curY, iconX + 16.0f, curY + rowH);
            m_renderer->DrawText(L"↗", floatBtnR, Colors::FgLight, m_formatBody.Get());
            m_clickableAreas.push_back({ floatBtnR, [id = r.id]() { WindowManager::Instance().ToggleWidgetById(id); } });

            D2D1_RECT_F pinBtnR = D2D1::RectF(iconX + 18.0f, curY, iconX + 34.0f, curY + rowH);
            m_renderer->DrawText(L"📌", pinBtnR, Colors::MutedLight, m_formatBody.Get());

            D2D1_RECT_F closeBtnR = D2D1::RectF(iconX + 36.0f, curY, iconX + 50.0f, curY + rowH);
            m_renderer->DrawText(L"×", closeBtnR, Colors::FaintLight, m_formatBody.Get());
            m_clickableAreas.push_back({ closeBtnR, [id = r.id]() { WindowManager::Instance().CloseWidgetById(id); } });

            curY += rowH;
        }
        m_renderer->DrawLine(D2D1::Point2F(contentX, curY), D2D1::Point2F(contentX + contentW, curY), Colors::BorderLight, 1.0f);

        curY += 34.0f;

        // Section: LIVE PREVIEWS
        D2D1_RECT_F prevHeaderR = D2D1::RectF(contentX, curY, contentX + 300.0f, curY + 16.0f);
        m_renderer->DrawText(L"LIVE PREVIEWS", prevHeaderR, Colors::FaintLight, m_formatSectionTitle.Get());
        curY += 24.0f;

        // Mini preview representations
        float prevGap = 12.0f;
        float prevW = (contentW - (prevGap * 2.0f)) / 3.0f;
        float prevH = 150.0f;

        // 1. Clock Preview
        D2D1_ROUNDED_RECT p1{ D2D1::RectF(contentX, curY, contentX + prevW, curY + prevH), 4.0f, 4.0f };
        m_renderer->FillRoundedRectangle(p1, Colors::SurfaceLight);
        m_renderer->DrawRoundedRectangle(p1, Colors::BorderLight, 1.0f);
        D2D1_RECT_F p1Title = D2D1::RectF(contentX + 10.0f, curY + 8.0f, contentX + prevW - 10.0f, curY + 24.0f);
        m_renderer->DrawText(L"PRECISION CLOCK", p1Title, Colors::FaintLight, m_formatMono.Get());

        D2D1_RECT_F p1Time = D2D1::RectF(contentX + 10.0f, curY + 44.0f, contentX + prevW - 10.0f, curY + 80.0f);
        m_renderer->DrawText(L"22:31:09", p1Time, Colors::FgLight, m_formatStatNum.Get());
        D2D1_RECT_F p1Date = D2D1::RectF(contentX + 10.0f, curY + 84.0f, contentX + prevW - 10.0f, curY + 100.0f);
        m_renderer->DrawText(L"Mon, Nov 2", p1Date, Colors::MutedLight, m_formatBody.Get());

        // 2. System Telemetry Preview
        float p2X = contentX + prevW + prevGap;
        D2D1_ROUNDED_RECT p2{ D2D1::RectF(p2X, curY, p2X + prevW, curY + prevH), 4.0f, 4.0f };
        m_renderer->FillRoundedRectangle(p2, Colors::SurfaceLight);
        m_renderer->DrawRoundedRectangle(p2, Colors::BorderLight, 1.0f);
        D2D1_RECT_F p2Title = D2D1::RectF(p2X + 10.0f, curY + 8.0f, p2X + prevW - 10.0f, curY + 24.0f);
        m_renderer->DrawText(L"SYSTEM TELEMETRY", p2Title, Colors::FaintLight, m_formatMono.Get());

        D2D1_RECT_F p2Cpu = D2D1::RectF(p2X + 10.0f, curY + 40.0f, p2X + prevW - 10.0f, curY + 60.0f);
        m_renderer->DrawText(L"CPU 29%    RAM 36%", p2Cpu, Colors::FgLight, m_formatMonoMedium.Get());
        D2D1_RECT_F p2Net = D2D1::RectF(p2X + 10.0f, curY + 70.0f, p2X + prevW - 10.0f, curY + 90.0f);
        m_renderer->DrawText(L"↓ 44 Mb/s  ↑ 18.3 Mb/s", p2Net, Colors::MutedLight, m_formatMono.Get());

        // 3. Scratchpad Preview
        float p3X = p2X + prevW + prevGap;
        D2D1_ROUNDED_RECT p3{ D2D1::RectF(p3X, curY, p3X + prevW, curY + prevH), 4.0f, 4.0f };
        m_renderer->FillRoundedRectangle(p3, Colors::SurfaceLight);
        m_renderer->DrawRoundedRectangle(p3, Colors::BorderLight, 1.0f);
        D2D1_RECT_F p3Title = D2D1::RectF(p3X + 10.0f, curY + 8.0f, p3X + prevW - 10.0f, curY + 24.0f);
        m_renderer->DrawText(L"SCRATCHPAD", p3Title, Colors::FaintLight, m_formatMono.Get());

        D2D1_RECT_F p3Text1 = D2D1::RectF(p3X + 10.0f, curY + 36.0f, p3X + prevW - 10.0f, curY + 60.0f);
        m_renderer->DrawText(L"○ Project Milestone\n  Ship XE Widgets v2.0", p3Text1, Colors::FgLight, m_formatCardDesc.Get());
        D2D1_RECT_F p3Text2 = D2D1::RectF(p3X + 10.0f, curY + 74.0f, p3X + prevW - 10.0f, curY + 100.0f);
        m_renderer->DrawText(L"○ UI Polish Checklist\n  Thin borders & editorial aesthetic", p3Text2, Colors::MutedLight, m_formatCardDesc.Get());
    }

    void Window::RenderSettingsTab(float leftX, float topY, float width, float /*height*/) {
        float contentX = leftX + 28.0f;
        float contentW = width - contentX - 28.0f;

        float curY = topY + 16.0f;

        // 1. APPEARANCE
        D2D1_RECT_F appR = D2D1::RectF(contentX, curY, contentX + 200.0f, curY + 16.0f);
        m_renderer->DrawText(L"01 — APPEARANCE", appR, Colors::FaintLight, m_formatSectionTitle.Get());
        curY += 24.0f;

        // Theme cards: Light vs Dark
        float themeW = (contentW - 14.0f) / 2.0f;
        float themeH = 80.0f;

        // Light Theme Card
        D2D1_ROUNDED_RECT lightR{ D2D1::RectF(contentX, curY, contentX + themeW, curY + themeH), 3.0f, 3.0f };
        m_renderer->FillRoundedRectangle(lightR, Colors::SurfaceLight);
        m_renderer->DrawRoundedRectangle(lightR, !m_isDarkMode ? Colors::FgLight : Colors::BorderLight, !m_isDarkMode ? 1.5f : 1.0f);
        if (!m_isDarkMode) {
            D2D1_RECT_F chkR = D2D1::RectF(contentX + themeW - 28.0f, curY + 10.0f, contentX + themeW - 10.0f, curY + 28.0f);
            m_renderer->DrawText(L"✓", chkR, Colors::FgLight, m_formatBody.Get());
        }

        m_clickableAreas.push_back({
            D2D1::RectF(contentX, curY, contentX + themeW, curY + themeH),
            [this]() { if (m_isDarkMode) ToggleTheme(); }
        });

        D2D1_RECT_F lightTitleR = D2D1::RectF(contentX + 16.0f, curY + 16.0f, contentX + themeW - 30.0f, curY + 34.0f);
        m_renderer->DrawText(L"Light", lightTitleR, Colors::FgLight, m_formatCardTitle.Get());

        D2D1_RECT_F lightDescR = D2D1::RectF(contentX + 16.0f, curY + 36.0f, contentX + themeW - 16.0f, curY + 60.0f);
        m_renderer->DrawText(L"Warm off-white. Editorial and spacious.", lightDescR, Colors::MutedLight, m_formatBody.Get());

        // Dark Theme Card
        float darkX = contentX + themeW + 14.0f;
        D2D1_ROUNDED_RECT darkR{ D2D1::RectF(darkX, curY, darkX + themeW, curY + themeH), 3.0f, 3.0f };
        m_renderer->FillRoundedRectangle(darkR, Colors::SurfaceLight);
        m_renderer->DrawRoundedRectangle(darkR, m_isDarkMode ? Colors::FgLight : Colors::BorderLight, m_isDarkMode ? 1.5f : 1.0f);
        if (m_isDarkMode) {
            D2D1_RECT_F chkDarkR = D2D1::RectF(darkX + themeW - 28.0f, curY + 10.0f, darkX + themeW - 10.0f, curY + 28.0f);
            m_renderer->DrawText(L"✓", chkDarkR, Colors::FgLight, m_formatBody.Get());
        }

        m_clickableAreas.push_back({
            D2D1::RectF(darkX, curY, darkX + themeW, curY + themeH),
            [this]() { if (!m_isDarkMode) ToggleTheme(); }
        });

        D2D1_RECT_F darkTitleR = D2D1::RectF(darkX + 16.0f, curY + 16.0f, darkX + themeW - 16.0f, curY + 34.0f);
        m_renderer->DrawText(L"Dark", darkTitleR, Colors::FgLight, m_formatCardTitle.Get());

        D2D1_RECT_F darkDescR = D2D1::RectF(darkX + 16.0f, curY + 36.0f, darkX + themeW - 16.0f, curY + 60.0f);
        m_renderer->DrawText(L"Deep near-black. Focused and precise.", darkDescR, Colors::MutedLight, m_formatBody.Get());

        curY += themeH + 12.0f;

        // Colour system footer
        D2D1_RECT_F colSysR = D2D1::RectF(contentX, curY, contentX + 200.0f, curY + 16.0f);
        m_renderer->DrawText(L"COLOUR SYSTEM", colSysR, Colors::FaintLight, m_formatMono.Get());

        D2D1_RECT_F colValR = D2D1::RectF(contentX + contentW - 200.0f, curY, contentX + contentW, curY + 16.0f);
        m_renderer->DrawText(L"Monochromatic Editorial", colValR, Colors::MutedLight, m_formatMono.Get());

        curY += 34.0f;

        // 2. TYPOGRAPHY
        D2D1_RECT_F typoR = D2D1::RectF(contentX, curY, contentX + 200.0f, curY + 16.0f);
        m_renderer->DrawText(L"02 — TYPOGRAPHY", typoR, Colors::FaintLight, m_formatSectionTitle.Get());
        curY += 24.0f;

        struct TypoRow {
            const wchar_t* tag;
            const wchar_t* desc;
            const wchar_t* font;
        };

        TypoRow tRows[] = {
            { L"DISPLAY", L"Headings, display titles, hero typography", L"Geist Pixel" },
            { L"BODY",    L"Navigation, descriptions, general interface", L"Geist Sans" },
            { L"MONO",    L"Labels, version numbers, system telemetry",   L"Geist Mono" }
        };

        float tRowH = 44.0f;
        for (const auto& tr : tRows) {
            m_renderer->DrawLine(D2D1::Point2F(contentX, curY), D2D1::Point2F(contentX + contentW, curY), Colors::BorderLight, 1.0f);

            D2D1_RECT_F tagR = D2D1::RectF(contentX, curY + 6.0f, contentX + 140.0f, curY + 20.0f);
            m_renderer->DrawText(tr.tag, tagR, Colors::FaintLight, m_formatMono.Get());

            D2D1_RECT_F descR = D2D1::RectF(contentX, curY + 20.0f, contentX + 400.0f, curY + 38.0f);
            m_renderer->DrawText(tr.desc, descR, Colors::MutedLight, m_formatBody.Get());

            D2D1_RECT_F fontR = D2D1::RectF(contentX + contentW - 200.0f, curY, contentX + contentW, curY + tRowH);
            m_renderer->DrawText(tr.font, fontR, Colors::FgLight, m_formatPageTitle.Get());

            curY += tRowH;
        }
        m_renderer->DrawLine(D2D1::Point2F(contentX, curY), D2D1::Point2F(contentX + contentW, curY), Colors::BorderLight, 1.0f);

        curY += 8.0f;
        D2D1_RECT_F assetR = D2D1::RectF(contentX, curY, contentX + 200.0f, curY + 16.0f);
        m_renderer->DrawText(L"ASSET STATUS", assetR, Colors::FaintLight, m_formatMono.Get());

        D2D1_RECT_F assetValR = D2D1::RectF(contentX + contentW - 240.0f, curY, contentX + contentW, curY + 16.0f);
        m_renderer->DrawText(L"Bundled Locally (WOFF2 / DirectWrite System)", assetValR, Colors::MutedLight, m_formatMono.Get());

        curY += 34.0f;

        // 3. WIDGET BEHAVIOUR
        D2D1_RECT_F behR = D2D1::RectF(contentX, curY, contentX + 200.0f, curY + 16.0f);
        m_renderer->DrawText(L"03 — WIDGET BEHAVIOUR", behR, Colors::FaintLight, m_formatSectionTitle.Get());
        curY += 24.0f;

        // Always on Top Card with Toggle
        D2D1_ROUNDED_RECT aotBox{ D2D1::RectF(contentX, curY, contentX + contentW, curY + 54.0f), 3.0f, 3.0f };
        m_renderer->FillRoundedRectangle(aotBox, Colors::SurfaceLight);
        m_renderer->DrawRoundedRectangle(aotBox, Colors::BorderLight, 1.0f);

        D2D1_RECT_F aotTitleR = D2D1::RectF(contentX + 16.0f, curY + 8.0f, contentX + 400.0f, curY + 26.0f);
        m_renderer->DrawText(L"Always on Top", aotTitleR, Colors::FgLight, m_formatCardTitle.Get());

        D2D1_RECT_F aotDescR = D2D1::RectF(contentX + 16.0f, curY + 26.0f, contentX + 500.0f, curY + 46.0f);
        m_renderer->DrawText(L"Float desktop widgets above other foreground application windows.", aotDescR, Colors::MutedLight, m_formatBody.Get());

        // Toggle Switch on right
        float togW = 34.0f;
        float togH = 18.0f;
        float togX = contentX + contentW - togW - 16.0f;
        float togY = curY + 18.0f;
        D2D1_ROUNDED_RECT togR{ D2D1::RectF(togX, togY, togX + togW, togY + togH), 9.0f, 9.0f };
        m_renderer->FillRoundedRectangle(togR, m_alwaysOnTopSetting ? Colors::FgLight : Colors::BorderLight);

        float knobX = m_alwaysOnTopSetting ? (togX + togW - 10.0f) : (togX + 10.0f);
        m_renderer->FillEllipse(D2D1::Ellipse(D2D1::Point2F(knobX, togY + 9.0f), 7.0f, 7.0f), Colors::BgLight);

        m_clickableAreas.push_back({
            D2D1::RectF(contentX, curY, contentX + contentW, curY + 54.0f),
            [this]() {
                m_alwaysOnTopSetting = !m_alwaysOnTopSetting;
                for (const auto& w : WindowManager::Instance().GetWidgets()) {
                    if (w && w->IsOpen()) {
                        w->SetAlwaysOnTop(m_alwaysOnTopSetting);
                    }
                }
            }
        });
    }

    void Window::OnRender() {
        if (!m_renderer) return;
        InitTypography();
        m_clickableAreas.clear();

        m_renderer->BeginDraw();

        // 1. Background: Monochromatic Editorial (#EBE9E4 / #111110)
        m_renderer->Clear(m_isDarkMode ? Colors::BgDark : Colors::BgLight);

        RECT rc{};
        GetClientRect(m_hwnd, &rc);
        float width = static_cast<float>(rc.right - rc.left);
        float height = static_cast<float>(rc.bottom - rc.top);

        float sidebarW = 190.0f;

        // 2. Left Sidebar
        RenderSidebar(width, height);

        // 3. Top Header Bar
        RenderTopHeader(sidebarW, width);

        // 4. Tab Content Area
        float contentTop = 52.0f;
        switch (m_activeTab) {
            case ManagerTab::Overview:
                RenderOverviewTab(sidebarW, contentTop, width, height);
                break;
            case ManagerTab::Library:
                RenderLibraryTab(sidebarW, contentTop, width, height);
                break;
            case ManagerTab::ActiveWidgets:
                RenderActiveWidgetsTab(sidebarW, contentTop, width, height);
                break;
            case ManagerTab::Settings:
                RenderSettingsTab(sidebarW, contentTop, width, height);
                break;
        }

        m_renderer->EndDraw();
    }

}
