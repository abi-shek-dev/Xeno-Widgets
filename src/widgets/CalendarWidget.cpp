#include "CalendarWidget.hpp"
#include <ctime>

namespace xe {

    CalendarWidget::CalendarWidget() {
        SYSTEMTIME st{};
        GetLocalTime(&st);
        m_selectedDay = st.wDay;

        m_events = {
            { L"1", L"Sprint Review & Release", L"10:30", L"Meeting" },
            { L"2", L"Deep Work — UI Polish Pass", L"14:00", L"Focus" },
            { L"3", L"Desktop Widgets Sync", L"16:45", L"Personal" }
        };
    }

    CalendarWidget::~CalendarWidget() {
        if (m_hwnd) {
            KillTimer(m_hwnd, m_timerId);
        }
    }

    void CalendarWidget::InitTypography() {
        if (m_typographyInitialized || !m_renderer) return;

        m_formatHeader = m_renderer->CreateTextFormat(
            L"Geist Mono", 11.0f, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER
        );

        m_formatDayHeader = m_renderer->CreateTextFormat(
            L"Geist Mono", 8.0f, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER
        );

        m_formatDayNum = m_renderer->CreateTextFormat(
            L"Geist Mono", 9.0f, DWRITE_FONT_WEIGHT_MEDIUM, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER
        );

        m_formatSection = m_renderer->CreateTextFormat(
            L"Geist Mono", 8.5f, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER
        );

        m_formatEventTitle = m_renderer->CreateTextFormat(
            L"Geist Sans", 10.0f, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER
        );

        m_formatEventTime = m_renderer->CreateTextFormat(
            L"Geist Mono", 8.5f, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_TEXT_ALIGNMENT_TRAILING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER
        );

        m_formatMono = m_renderer->CreateTextFormat(
            L"Geist Mono", 9.0f, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER
        );

        m_typographyInitialized = true;
    }

    void CalendarWidget::OnTick(UINT_PTR timerId) {
        if (timerId == m_timerId && m_hwnd) {
            InvalidateRect(m_hwnd, nullptr, FALSE);
        }
    }

    LRESULT CalendarWidget::HandleWidgetMessage(UINT msg, WPARAM wParam, LPARAM lParam) {
        if (msg == WM_CREATE || (msg == WM_SHOWWINDOW && wParam == TRUE)) {
            if (m_hwnd) {
                SetTimer(m_hwnd, m_timerId, 60000, nullptr);
            }
        }
        return WidgetWindow::HandleWidgetMessage(msg, wParam, lParam);
    }

    bool CalendarWidget::IsInInteractiveArea(int clientX, int clientY) const {
        POINT pt{ clientX, clientY };
        for (const auto& item : m_dayRects) {
            if (PtInRect(&item.first, pt)) return true;
        }
        return false;
    }

    void CalendarWidget::OnLButtonDown(int x, int y) {
        POINT pt{ x, y };
        for (const auto& item : m_dayRects) {
            if (PtInRect(&item.first, pt)) {
                m_selectedDay = item.second;
                if (m_hwnd) InvalidateRect(m_hwnd, nullptr, FALSE);
                return;
            }
        }
    }

    void CalendarWidget::OnRender() {
        if (!m_renderer) return;
        InitTypography();
        m_dayRects.clear();

        RECT rc{};
        GetClientRect(m_hwnd, &rc);
        float width = static_cast<float>(rc.right - rc.left);
        float height = static_cast<float>(rc.bottom - rc.top);

        // 1. Transparent background
        m_renderer->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));

        // 2. Main card
        float margin = 4.0f;
        D2D1_ROUNDED_RECT cardRect{
            D2D1::RectF(margin, margin, width - margin, height - margin),
            4.0f, 4.0f
        };
        m_renderer->FillRoundedRectangle(cardRect, Colors::SurfaceLight);
        m_renderer->DrawRoundedRectangle(cardRect, Colors::BorderLight, 1.0f);

        // 3. Header row
        float headerH = 28.0f;
        float headerY = margin + 2.0f;

        // Calendar icon
        float iconX = margin + 14.0f;
        float iconY = headerY + 8.0f;
        m_renderer->DrawRectangle(D2D1::RectF(iconX, iconY, iconX + 10.0f, iconY + 11.0f), Colors::MutedLight, 1.2f);
        m_renderer->DrawLine(D2D1::Point2F(iconX, iconY + 3.0f), D2D1::Point2F(iconX + 10.0f, iconY + 3.0f), Colors::MutedLight, 1.0f);

        D2D1_RECT_F headerLabelR = D2D1::RectF(margin + 28.0f, headerY, width - 100.0f, headerY + headerH);
        m_renderer->DrawText(L"CALENDAR", headerLabelR, Colors::FaintLight, m_formatHeader.Get());

        // Month / Year right label
        SYSTEMTIME st{};
        GetLocalTime(&st);
        const wchar_t* months[] = { L"", L"Jan", L"Feb", L"Mar", L"Apr", L"May", L"Jun", L"Jul", L"Aug", L"Sep", L"Oct", L"Nov", L"Dec" };
        std::wstring monthYearStr = std::wstring(months[st.wMonth]) + L" " + std::to_wstring(st.wYear);

        ComPtr<IDWriteTextFormat> fmtRightMonth;
        fmtRightMonth = m_renderer->CreateTextFormat(
            L"Geist Mono", 9.5f, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_TEXT_ALIGNMENT_TRAILING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER
        );
        D2D1_RECT_F monthR = D2D1::RectF(width - margin - 100.0f, headerY, width - margin - 12.0f, headerY + headerH);
        if (fmtRightMonth) {
            m_renderer->DrawText(monthYearStr, monthR, Colors::MutedLight, fmtRightMonth.Get());
        }

        // Header divider
        m_renderer->DrawLine(
            D2D1::Point2F(margin, margin + headerH),
            D2D1::Point2F(width - margin, margin + headerH),
            Colors::BorderLight, 1.0f
        );

        // 4. Days row (Su, Mo, Tu, We, Th, Fr, Sa)
        float curY = margin + headerH + 5.0f;
        float contentW = width - (margin * 2.0f) - 24.0f;
        float leftX = margin + 12.0f;
        float dayColW = contentW / 7.0f;

        const wchar_t* daysW[] = { L"Su", L"Mo", L"Tu", L"We", L"Th", L"Fr", L"Sa" };
        for (int i = 0; i < 7; ++i) {
            float dx = leftX + i * dayColW;
            D2D1_RECT_F dR = D2D1::RectF(dx, curY, dx + dayColW, curY + 12.0f);
            m_renderer->DrawText(daysW[i], dR, Colors::FaintLight, m_formatDayHeader.Get());
        }

        // 5. Day numbers grid (days 1 to 14)
        curY += 14.0f;
        int today = st.wDay;
        float dayCellH = 18.0f;

        for (int row = 0; row < 2; ++row) {
            for (int col = 0; col < 7; ++col) {
                int d = row * 7 + col + 1;
                float dx = leftX + col * dayColW;
                float dy = curY + row * (dayCellH + 2.0f);

                D2D1_ROUNDED_RECT cellR{ D2D1::RectF(dx + 2.0f, dy, dx + dayColW - 2.0f, dy + dayCellH), 2.0f, 2.0f };

                RECT clickRc = { static_cast<LONG>(dx + 2.0f), static_cast<LONG>(dy), static_cast<LONG>(dx + dayColW - 2.0f), static_cast<LONG>(dy + dayCellH) };
                m_dayRects.push_back({ clickRc, d });

                bool isSelected = (d == m_selectedDay);
                bool isToday = (d == today);

                if (isSelected) {
                    m_renderer->FillRoundedRectangle(cellR, Colors::FgLight);
                    m_renderer->DrawText(std::to_wstring(d), D2D1::RectF(dx, dy, dx + dayColW, dy + dayCellH), Colors::BgLight, m_formatDayNum.Get());
                } else {
                    if (isToday) {
                        m_renderer->DrawRoundedRectangle(cellR, Colors::BorderLight, 1.0f);
                    }
                    m_renderer->DrawText(std::to_wstring(d), D2D1::RectF(dx, dy, dx + dayColW, dy + dayCellH), isToday ? Colors::FgLight : Colors::MutedLight, m_formatDayNum.Get());
                }
            }
        }

        curY += 2 * (dayCellH + 2.0f) + 6.0f;

        // 6. Agenda section
        std::wstring agendaHeader = L"AGENDA · NOV " + std::to_wstring(m_selectedDay);
        D2D1_RECT_F agR = D2D1::RectF(leftX, curY, leftX + 200.0f, curY + 14.0f);
        m_renderer->DrawText(agendaHeader, agR, Colors::FaintLight, m_formatSection.Get());
        curY += 16.0f;

        // Event row 1
        D2D1_RECT_F ev1Box = D2D1::RectF(leftX, curY, leftX + contentW, curY + 20.0f);
        m_renderer->FillRoundedRectangle(D2D1::RoundedRect(ev1Box, 2.0f, 2.0f), Colors::BgLight);
        m_renderer->DrawRoundedRectangle(D2D1::RoundedRect(ev1Box, 2.0f, 2.0f), Colors::BorderLight, 1.0f);

        D2D1_RECT_F ev1T = D2D1::RectF(leftX + 6.0f, curY, leftX + contentW - 50.0f, curY + 20.0f);
        m_renderer->DrawText(L"• Sprint Review & Release", ev1T, Colors::FgLight, m_formatEventTitle.Get());

        D2D1_RECT_F ev1Time = D2D1::RectF(leftX + contentW - 50.0f, curY, leftX + contentW - 6.0f, curY + 20.0f);
        m_renderer->DrawText(L"10:30", ev1Time, Colors::FaintLight, m_formatEventTime.Get());

        // 7. Footer
        float footerY = height - margin - 20.0f;
        m_renderer->DrawLine(
            D2D1::Point2F(margin, footerY),
            D2D1::Point2F(width - margin, footerY),
            Colors::BorderLight, 1.0f
        );

        D2D1_RECT_F footL = D2D1::RectF(margin + 12.0f, footerY, width - 100.0f, height - margin);
        m_renderer->DrawText(L"3 EVENTS", footL, Colors::FaintLight, m_formatMono.Get());

        D2D1_RECT_F footR = D2D1::RectF(width - margin - 80.0f, footerY, width - margin - 12.0f, height - margin);
        if (fmtRightMonth) {
            m_renderer->DrawText(L"TODAY", footR, Colors::FaintLight, fmtRightMonth.Get());
        }

        DrawHoverToolbar(width);
    }

}
