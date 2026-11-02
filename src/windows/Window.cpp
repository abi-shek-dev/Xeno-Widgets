#include "Window.hpp"
#include "../core/Logger.hpp"

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
        wc.hCursor = LoadCursorW(nullptr, MAKEINTRESOURCEW(32512));
        wc.hbrBackground = nullptr; // Null background brush prevents GDI flickering during Direct2D render
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

        int posX = CW_USEDEFAULT;
        int posY = CW_USEDEFAULT;

        if (config.centerOnScreen) {
            int screenW = GetSystemMetrics(SM_CXSCREEN);
            int screenH = GetSystemMetrics(SM_CYSCREEN);
            posX = (screenW - config.width) / 2;
            posY = (screenH - config.height) / 2;
        }

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
        BOOL useDarkMode = TRUE;
        DwmSetWindowAttribute(m_hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &useDarkMode, sizeof(useDarkMode));

        // Create Direct2D device resources for this HWND
        if (m_renderer) {
            if (!m_renderer->CreateDeviceResources(m_hwnd)) {
                XE_LOG_ERROR("Failed to bind Direct2D to window");
                return false;
            }

            // Initialize typography formats (Geist Pixel/Sans/Mono fallback to Segoe UI/Consolas)
            m_formatTitle = m_renderer->CreateTextFormat(
                L"Geist Pixel", 28.0f, DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_NORMAL,
                DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_NEAR
            );

            m_formatSubtitle = m_renderer->CreateTextFormat(
                L"Geist Sans", 13.0f, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
                DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_NEAR
            );

            m_formatMono = m_renderer->CreateTextFormat(
                L"Geist Mono", 11.0f, DWRITE_FONT_WEIGHT_MEDIUM, DWRITE_FONT_STYLE_NORMAL,
                DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER
            );

            m_formatBody = m_renderer->CreateTextFormat(
                L"Geist Sans", 12.0f, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
                DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_NEAR
            );
        }

        XE_LOG_INFO("Native Win32 window created successfully");
        return true;
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
                // Direct2D clears the full surface; avoid GDI background erase flicker
                return 1;

            case WM_DESTROY:
                PostQuitMessage(0);
                return 0;

            default:
                break;
        }

        return DefWindowProcW(m_hwnd, uMsg, wParam, lParam);
    }

    void Window::OnRender() {
        if (!m_renderer) return;

        m_renderer->BeginDraw();

        // Background (#111110)
        m_renderer->Clear(Colors::BgDark);

        RECT rc{};
        GetClientRect(m_hwnd, &rc);
        float width = static_cast<float>(rc.right - rc.left);
        float height = static_cast<float>(rc.bottom - rc.top);

        // Outer margin: 32px
        float margin = 32.0f;
        float cardW = width - (margin * 2.0f);
        float cardH = height - (margin * 2.0f);

        if (cardW > 100.0f && cardH > 100.0f) {
            // Main Content Card (#1C1B19 with #2A2825 border)
            D2D1_ROUNDED_RECT cardRect{
                D2D1::RectF(margin, margin, margin + cardW, margin + cardH),
                6.0f,
                6.0f
            };
            m_renderer->FillRoundedRectangle(cardRect, Colors::SurfaceDark);
            m_renderer->DrawRoundedRectangle(cardRect, Colors::BorderDark, 1.0f);

            // Card Header divider line
            float headerH = 70.0f;
            m_renderer->DrawLine(
                D2D1::Point2F(margin, margin + headerH),
                D2D1::Point2F(margin + cardW, margin + headerH),
                Colors::BorderDark,
                1.0f
            );

            // Header Section Label
            D2D1_RECT_F labelRect = D2D1::RectF(margin + 24.0f, margin + 14.0f, margin + cardW - 24.0f, margin + 30.0f);
            m_renderer->DrawText(L"01 // SYSTEM FOUNDATION", labelRect, Colors::MutedDark, m_formatMono.Get());

            // Header Title
            D2D1_RECT_F titleRect = D2D1::RectF(margin + 24.0f, margin + 30.0f, margin + cardW - 24.0f, margin + 64.0f);
            m_renderer->DrawText(L"XE WIDGETS", titleRect, Colors::FgDark, m_formatTitle.Get());

            // Status Badge in Top Right
            float badgeW = 200.0f;
            float badgeH = 26.0f;
            float badgeX = margin + cardW - badgeW - 20.0f;
            float badgeY = margin + 22.0f;
            D2D1_ROUNDED_RECT badgeRect{
                D2D1::RectF(badgeX, badgeY, badgeX + badgeW, badgeY + badgeH),
                3.0f,
                3.0f
            };
            m_renderer->FillRoundedRectangle(badgeRect, Colors::BgDark);
            m_renderer->DrawRoundedRectangle(badgeRect, Colors::BorderDark, 1.0f);

            D2D1_RECT_F badgeTextRect = D2D1::RectF(badgeX + 8.0f, badgeY, badgeX + badgeW - 8.0f, badgeY + badgeH);
            m_renderer->DrawText(L"D2D1 ACCELERATED // PHASE 1", badgeTextRect, Colors::FgDark, m_formatMono.Get());

            // Subtitle Description
            D2D1_RECT_F descRect = D2D1::RectF(margin + 24.0f, margin + 86.0f, margin + cardW - 24.0f, margin + 130.0f);
            m_renderer->DrawText(
                L"Native Windows desktop widget platform engineered with C++20, Win32, Direct2D, and DirectWrite. "
                L"Rainmeter-inspired architecture with independent HWND windows, frameless alpha transparency, and native telemetry.",
                descRect,
                Colors::MutedDark,
                m_formatSubtitle.Get()
            );

            // Technical Specs 3-Column Grid
            float gridTop = margin + 146.0f;
            float colGap = 16.0f;
            float colW = (cardW - 48.0f - (colGap * 2.0f)) / 3.0f;
            float colH = 110.0f;

            struct SpecBox {
                const wchar_t* tag;
                const wchar_t* title;
                const wchar_t* detail;
            };

            SpecBox specs[3] = {
                { L"SPEC // 01", L"WIN32 API & C++20", L"Pure native Win32 message pump with per-monitor v2 DPI scaling." },
                { L"SPEC // 02", L"DIRECT2D + DWRITE", L"Hardware-accelerated geometry, typographic layout, and brush caching." },
                { L"SPEC // 03", L"DESKTOP Z-ORDER", L"Foundation for wallpaper anchoring, position locking, and click-through." }
            };

            for (int i = 0; i < 3; ++i) {
                float colX = margin + 24.0f + static_cast<float>(i) * (colW + colGap);
                D2D1_ROUNDED_RECT specRect{
                    D2D1::RectF(colX, gridTop, colX + colW, gridTop + colH),
                    4.0f,
                    4.0f
                };
                m_renderer->FillRoundedRectangle(specRect, Colors::BgDark);
                m_renderer->DrawRoundedRectangle(specRect, Colors::BorderDark, 1.0f);

                D2D1_RECT_F tagR = D2D1::RectF(colX + 12.0f, gridTop + 10.0f, colX + colW - 12.0f, gridTop + 26.0f);
                m_renderer->DrawText(specs[i].tag, tagR, Colors::FaintDark, m_formatMono.Get());

                D2D1_RECT_F titleR = D2D1::RectF(colX + 12.0f, gridTop + 28.0f, colX + colW - 12.0f, gridTop + 48.0f);
                m_renderer->DrawText(specs[i].title, titleR, Colors::FgDark, m_formatMono.Get());

                D2D1_RECT_F detailR = D2D1::RectF(colX + 12.0f, gridTop + 50.0f, colX + colW - 12.0f, gridTop + colH - 10.0f);
                m_renderer->DrawText(specs[i].detail, detailR, Colors::MutedDark, m_formatBody.Get());
            }

            // Footer Status Bar
            float footerY = margin + cardH - 36.0f;
            m_renderer->DrawLine(
                D2D1::Point2F(margin, footerY),
                D2D1::Point2F(margin + cardW, footerY),
                Colors::BorderDark,
                1.0f
            );

            // Active Green Status Dot
            D2D1_POINT_2F dotCenter = D2D1::Point2F(margin + 28.0f, footerY + 18.0f);
            m_renderer->FillRoundedRectangle(
                D2D1::RoundedRect(D2D1::RectF(dotCenter.x - 3.0f, dotCenter.y - 3.0f, dotCenter.x + 3.0f, dotCenter.y + 3.0f), 3.0f, 3.0f),
                Colors::FgDark
            );

            D2D1_RECT_F footerTextRect = D2D1::RectF(margin + 38.0f, footerY + 8.0f, margin + cardW - 24.0f, footerY + 28.0f);
            m_renderer->DrawText(
                L"ENGINE STATUS: OPERATIONAL · ZERO WEB RUNTIME · STANDALONE MSVC X64 BINARY",
                footerTextRect,
                Colors::MutedDark,
                m_formatMono.Get()
            );
        }

        m_renderer->EndDraw();
    }

}
