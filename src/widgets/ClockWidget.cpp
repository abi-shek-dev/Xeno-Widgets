#include "ClockWidget.hpp"
#include <algorithm>
#include <ctime>

namespace xe {

    ClockWidget::ClockWidget() {
        m_timeData = SystemTimeProvider::GetCurrentTime();
    }

    ClockWidget::~ClockWidget() {
        if (m_hwnd) {
            KillTimer(m_hwnd, m_timerId);
        }
    }

    void ClockWidget::InitTypography() {
        if (m_typographyInitialized || !m_renderer) return;

        m_formatHeader = m_renderer->CreateTextFormat(
            L"Geist Mono", 11.0f, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER
        );

        m_formatTime = m_renderer->CreateTextFormat(
            L"Geist Mono", 32.0f, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER
        );

        m_formatDate = m_renderer->CreateTextFormat(
            L"Geist Sans", 11.5f, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER
        );

        m_formatFooter = m_renderer->CreateTextFormat(
            L"Geist Mono", 9.0f, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER
        );

        m_typographyInitialized = true;
    }

    void ClockWidget::OnTick(UINT_PTR timerId) {
        if (timerId == m_timerId) {
            m_timeData = SystemTimeProvider::GetCurrentTime();
            if (m_hwnd) {
                InvalidateRect(m_hwnd, nullptr, FALSE);
            }
        }
    }

    LRESULT ClockWidget::HandleWidgetMessage(UINT msg, WPARAM wParam, LPARAM lParam) {
        if (msg == WM_CREATE || (msg == WM_SHOWWINDOW && wParam == TRUE)) {
            if (m_hwnd) {
                SetTimer(m_hwnd, m_timerId, 1000, nullptr);
            }
        }
        return WidgetWindow::HandleWidgetMessage(msg, wParam, lParam);
    }

    void ClockWidget::OnRender() {
        if (!m_renderer) return;
        InitTypography();

        RECT rc{};
        GetClientRect(m_hwnd, &rc);
        float width = static_cast<float>(rc.right - rc.left);
        float height = static_cast<float>(rc.bottom - rc.top);

        // 1. Clear transparent background for DWM
        m_renderer->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));

        // 2. Main card (#DDDBD5 surface, #D0CEC9 border, 4px radius)
        float margin = 4.0f;
        D2D1_ROUNDED_RECT cardRect{
            D2D1::RectF(margin, margin, width - margin, height - margin),
            4.0f, 4.0f
        };
        m_renderer->FillRoundedRectangle(cardRect, Colors::SurfaceLight);
        m_renderer->DrawRoundedRectangle(cardRect, Colors::BorderLight, 1.0f);

        // 3. Header row (28px height)
        float headerH = 28.0f;
        float headerY = margin + 2.0f;

        // Clock icon (circle + hands)
        D2D1_POINT_2F clockC = D2D1::Point2F(margin + 16.0f, headerY + 12.0f);
        m_renderer->DrawEllipse(D2D1::Ellipse(clockC, 5.0f, 5.0f), Colors::MutedLight, 1.0f);
        m_renderer->DrawLine(clockC, D2D1::Point2F(clockC.x, clockC.y - 3.0f), Colors::MutedLight, 1.0f);
        m_renderer->DrawLine(clockC, D2D1::Point2F(clockC.x + 2.5f, clockC.y), Colors::MutedLight, 1.0f);

        D2D1_RECT_F headerLabelRect = D2D1::RectF(margin + 28.0f, headerY, width - 120.0f, headerY + headerH);
        m_renderer->DrawText(L"PRECISION CLOCK", headerLabelRect, Colors::FaintLight, m_formatHeader.Get());

        // Right side tabs: CLK (active black pill), SW, FCS
        ComPtr<IDWriteTextFormat> fmtPill = m_renderer->CreateTextFormat(
            L"Geist Mono", 9.0f, DWRITE_FONT_WEIGHT_MEDIUM, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER
        );

        float tabH = 17.0f;
        float tabY = headerY + 5.0f;
        float tabFcsW = 24.0f;
        float tabSwW = 20.0f;
        float tabClkW = 28.0f;

        float tabRight = width - margin - 8.0f;
        float fcsX = tabRight - tabFcsW;
        float swX = fcsX - tabSwW - 3.0f;
        float clkX = swX - tabClkW - 3.0f;

        // CLK (active black)
        D2D1_ROUNDED_RECT clkR{ D2D1::RectF(clkX, tabY, clkX + tabClkW, tabY + tabH), 2.0f, 2.0f };
        m_renderer->FillRoundedRectangle(clkR, Colors::FgLight);
        if (fmtPill) m_renderer->DrawText(L"CLK", D2D1::RectF(clkX, tabY, clkX + tabClkW, tabY + tabH), Colors::BgLight, fmtPill.Get());

        // SW
        D2D1_ROUNDED_RECT swR{ D2D1::RectF(swX, tabY, swX + tabSwW, tabY + tabH), 2.0f, 2.0f };
        m_renderer->DrawRoundedRectangle(swR, Colors::BorderLight, 1.0f);
        if (fmtPill) m_renderer->DrawText(L"SW", D2D1::RectF(swX, tabY, swX + tabSwW, tabY + tabH), Colors::MutedLight, fmtPill.Get());

        // FCS
        D2D1_ROUNDED_RECT fcsR{ D2D1::RectF(fcsX, tabY, fcsX + tabFcsW, tabY + tabH), 2.0f, 2.0f };
        m_renderer->DrawRoundedRectangle(fcsR, Colors::BorderLight, 1.0f);
        if (fmtPill) m_renderer->DrawText(L"FCS", D2D1::RectF(fcsX, tabY, fcsX + tabFcsW, tabY + tabH), Colors::MutedLight, fmtPill.Get());

        // Header divider line
        m_renderer->DrawLine(
            D2D1::Point2F(margin, margin + headerH),
            D2D1::Point2F(width - margin, margin + headerH),
            Colors::BorderLight, 1.0f
        );

        // 4. Time display (HH:MM:SS) centered horizontally at y=46
        D2D1_RECT_F timeRect = D2D1::RectF(margin, margin + 36.0f, width - margin, margin + 82.0f);
        m_renderer->DrawText(m_timeData.timeStr, timeRect, Colors::FgLight, m_formatTime.Get());

        // 5. Date display (e.g. "Mon, Nov 2") centered horizontally at y=84
        SYSTEMTIME st{};
        GetLocalTime(&st);
        const wchar_t* months[] = { L"", L"Jan", L"Feb", L"Mar", L"Apr", L"May", L"Jun", L"Jul", L"Aug", L"Sep", L"Oct", L"Nov", L"Dec" };
        const wchar_t* wdays[] = { L"Sun", L"Mon", L"Tue", L"Wed", L"Thu", L"Fri", L"Sat" };
        std::wstring dateNice = std::wstring(wdays[st.wDayOfWeek]) + L", " + months[st.wMonth] + L" " + std::to_wstring(st.wDay);

        D2D1_RECT_F dateRect = D2D1::RectF(margin, margin + 80.0f, width - margin, margin + 98.0f);
        m_renderer->DrawText(dateNice, dateRect, Colors::MutedLight, m_formatDate.Get());

        // 6. Timezone pills row (centered horizontally at y=104)
        float tzY = margin + 104.0f;
        float tzH = 17.0f;

        // Centering the timezone group: icon (10) + LOCAL (36) + NY (22) + LONDON (44) + TOKYO (38) + gaps
        float groupW = 12.0f + 4.0f + 38.0f + 4.0f + 20.0f + 4.0f + 44.0f + 4.0f + 38.0f;
        float tzStartX = (width - groupW) / 2.0f;

        // Globe icon
        D2D1_POINT_2F globeC = D2D1::Point2F(tzStartX + 5.0f, tzY + 8.5f);
        m_renderer->DrawEllipse(D2D1::Ellipse(globeC, 4.5f, 4.5f), Colors::FaintLight, 1.0f);
        m_renderer->DrawLine(D2D1::Point2F(globeC.x - 4.5f, globeC.y), D2D1::Point2F(globeC.x + 4.5f, globeC.y), Colors::FaintLight, 1.0f);

        float curX = tzStartX + 16.0f;

        // LOCAL (black active pill)
        D2D1_ROUNDED_RECT localR{ D2D1::RectF(curX, tzY, curX + 38.0f, tzY + tzH), 2.0f, 2.0f };
        m_renderer->FillRoundedRectangle(localR, Colors::FgLight);
        if (fmtPill) m_renderer->DrawText(L"LOCAL", D2D1::RectF(curX, tzY, curX + 38.0f, tzY + tzH), Colors::BgLight, fmtPill.Get());
        curX += 38.0f + 4.0f;

        // NY
        D2D1_ROUNDED_RECT nyR{ D2D1::RectF(curX, tzY, curX + 20.0f, tzY + tzH), 2.0f, 2.0f };
        m_renderer->DrawRoundedRectangle(nyR, Colors::BorderLight, 1.0f);
        if (fmtPill) m_renderer->DrawText(L"NY", D2D1::RectF(curX, tzY, curX + 20.0f, tzY + tzH), Colors::FaintLight, fmtPill.Get());
        curX += 20.0f + 4.0f;

        // LONDON
        D2D1_ROUNDED_RECT lonR{ D2D1::RectF(curX, tzY, curX + 44.0f, tzY + tzH), 2.0f, 2.0f };
        m_renderer->DrawRoundedRectangle(lonR, Colors::BorderLight, 1.0f);
        if (fmtPill) m_renderer->DrawText(L"LONDON", D2D1::RectF(curX, tzY, curX + 44.0f, tzY + tzH), Colors::FaintLight, fmtPill.Get());
        curX += 44.0f + 4.0f;

        // TOKYO
        D2D1_ROUNDED_RECT tokR{ D2D1::RectF(curX, tzY, curX + 38.0f, tzY + tzH), 2.0f, 2.0f };
        m_renderer->DrawRoundedRectangle(tokR, Colors::BorderLight, 1.0f);
        if (fmtPill) m_renderer->DrawText(L"TOKYO", D2D1::RectF(curX, tzY, curX + 38.0f, tzY + tzH), Colors::FaintLight, fmtPill.Get());

        // 7. Footer
        float footerY = height - margin - 20.0f;
        m_renderer->DrawLine(
            D2D1::Point2F(margin, footerY),
            D2D1::Point2F(width - margin, footerY),
            Colors::BorderLight, 1.0f
        );

        D2D1_RECT_F footL = D2D1::RectF(margin + 12.0f, footerY, width - 100.0f, height - margin);
        m_renderer->DrawText(L"NTP SYNC", footL, Colors::FaintLight, m_formatFooter.Get());

        ComPtr<IDWriteTextFormat> fmtRight;
        fmtRight = m_renderer->CreateTextFormat(
            L"Geist Mono", 9.0f, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_TEXT_ALIGNMENT_TRAILING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER
        );
        D2D1_RECT_F footR = D2D1::RectF(width - margin - 80.0f, footerY, width - margin - 12.0f, height - margin);
        if (fmtRight) {
            m_renderer->DrawText(L"±0.2ms", footR, Colors::FaintLight, fmtRight.Get());
        }

        DrawHoverToolbar(width);
    }

}
