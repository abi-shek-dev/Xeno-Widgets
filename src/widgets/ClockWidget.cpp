#include "ClockWidget.hpp"
#include <algorithm>

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

        // 9pt ≈ 12.0f DIPs
        m_formatHeader = m_renderer->CreateTextFormat(
            L"Geist Mono",
            12.0f,
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_TEXT_ALIGNMENT_LEADING,
            DWRITE_PARAGRAPH_ALIGNMENT_CENTER
        );

        // 38pt ≈ 50.0f DIPs
        m_formatTime = m_renderer->CreateTextFormat(
            L"Geist Mono",
            48.0f,
            DWRITE_FONT_WEIGHT_SEMI_BOLD,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_TEXT_ALIGNMENT_CENTER,
            DWRITE_PARAGRAPH_ALIGNMENT_CENTER
        );

        // 11pt ≈ 15.0f DIPs
        m_formatDate = m_renderer->CreateTextFormat(
            L"Geist Mono",
            14.5f,
            DWRITE_FONT_WEIGHT_MEDIUM,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_TEXT_ALIGNMENT_CENTER,
            DWRITE_PARAGRAPH_ALIGNMENT_CENTER
        );

        // 9pt ≈ 12.0f DIPs
        m_formatFooter = m_renderer->CreateTextFormat(
            L"Geist Mono",
            12.0f,
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_TEXT_ALIGNMENT_LEADING,
            DWRITE_PARAGRAPH_ALIGNMENT_CENTER
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

        // 1. Clear transparent background for DWM premultiplied alpha
        m_renderer->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));

        // 2. Draw rounded rect card (6px margin, 6px radius)
        float margin = 6.0f;
        D2D1_ROUNDED_RECT cardRect{
            D2D1::RectF(margin, margin, width - margin, height - margin),
            6.0f,
            6.0f
        };
        m_renderer->FillRoundedRectangle(cardRect, Colors::SurfaceDark);
        m_renderer->DrawRoundedRectangle(cardRect, Colors::BorderDark, 1.0f);

        // 3. Header row (32px height, from y=6 to y=38)
        D2D1_RECT_F headerLabelRect = D2D1::RectF(margin + 14.0f, margin, width - margin - 60.0f, margin + 32.0f);
        m_renderer->DrawText(L"PRECISION CLOCK", headerLabelRect, Colors::MutedDark, m_formatHeader.Get());

        // 4. Header divider line
        m_renderer->DrawLine(
            D2D1::Point2F(margin, margin + 32.0f),
            D2D1::Point2F(width - margin, margin + 32.0f),
            Colors::BorderDark,
            1.0f
        );

        // 5. Time display (HH:MM:SS) centered horizontally at y=42
        D2D1_RECT_F timeRect = D2D1::RectF(margin, margin + 34.0f, width - margin, margin + 84.0f);
        m_renderer->DrawText(m_timeData.timeStr, timeRect, Colors::FgDark, m_formatTime.Get());

        // 6. Date display centered horizontally at y=88
        D2D1_RECT_F dateRect = D2D1::RectF(margin, margin + 84.0f, width - margin, margin + 104.0f);
        m_renderer->DrawText(m_timeData.dateStr, dateRect, Colors::MutedDark, m_formatDate.Get());

        // 7. Seconds progress bar (3px height at y=118, 24px side margins)
        float barMargin = 24.0f;
        float barY = margin + 112.0f;
        float barWidth = width - (barMargin * 2.0f);
        float barHeight = 3.0f;

        // Background bar
        D2D1_RECT_F bgBarRect = D2D1::RectF(barMargin, barY, barMargin + barWidth, barY + barHeight);
        m_renderer->FillRectangle(bgBarRect, Colors::BorderDark);

        // Foreground fill bar proportional to seconds / 59.0f
        float progress = std::clamp(static_cast<float>(m_timeData.second) / 59.0f, 0.0f, 1.0f);
        float fillWidth = progress * barWidth;
        if (fillWidth > 0.0f) {
            D2D1_RECT_F fgBarRect = D2D1::RectF(barMargin, barY, barMargin + fillWidth, barY + barHeight);
            m_renderer->FillRectangle(fgBarRect, Colors::FgDark);
        }

        // 8. Footer row at bottom of card
        D2D1_RECT_F footerRect = D2D1::RectF(barMargin, margin + 124.0f, width - barMargin, height - margin - 4.0f);
        m_renderer->DrawText(L"02 // CLOCK", footerRect, Colors::FaintDark, m_formatFooter.Get());

        // Small live status indicator dot next to footer
        float dotX = width - barMargin - 6.0f;
        float dotY = margin + 138.0f;
        D2D1_ROUNDED_RECT dotRect{
            D2D1::RectF(dotX - 2.5f, dotY - 2.5f, dotX + 2.5f, dotY + 2.5f),
            2.5f,
            2.5f
        };
        m_renderer->FillRoundedRectangle(dotRect, Colors::FgDark);

        // 9. Floating hover toolbar
        DrawHoverToolbar(width);
    }

}
