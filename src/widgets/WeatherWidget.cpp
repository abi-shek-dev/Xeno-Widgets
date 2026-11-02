#include "WeatherWidget.hpp"
#include <algorithm>
#include <cmath>

namespace xe {

    WeatherWidget::WeatherWidget() = default;

    WeatherWidget::~WeatherWidget() {
        if (m_hwnd) {
            KillTimer(m_hwnd, m_timerId);
        }
    }

    void WeatherWidget::InitTypography() {
        if (m_typographyInitialized || !m_renderer) return;

        m_formatHeader = m_renderer->CreateTextFormat(
            L"Geist Mono", 11.5f, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER
        );

        m_formatBadge = m_renderer->CreateTextFormat(
            L"Geist Mono", 10.0f, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER
        );

        m_formatBigTemp = m_renderer->CreateTextFormat(
            L"Geist Mono", 32.0f, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER
        );

        m_formatCondition = m_renderer->CreateTextFormat(
            L"Geist Sans", 11.0f, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER
        );

        m_formatLocation = m_renderer->CreateTextFormat(
            L"Geist Mono", 10.0f, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER
        );

        m_formatStatLabel = m_renderer->CreateTextFormat(
            L"Geist Mono", 8.5f, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER
        );

        m_formatStatVal = m_renderer->CreateTextFormat(
            L"Geist Mono", 11.0f, DWRITE_FONT_WEIGHT_MEDIUM, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER
        );

        m_formatForecastDay = m_renderer->CreateTextFormat(
            L"Geist Mono", 9.0f, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER
        );

        m_formatForecastSym = m_renderer->CreateTextFormat(
            L"Geist Mono", 12.0f, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER
        );

        m_formatForecastTemp = m_renderer->CreateTextFormat(
            L"Geist Mono", 10.0f, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER
        );

        m_formatFooter = m_renderer->CreateTextFormat(
            L"Geist Mono", 9.0f, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER
        );

        m_typographyInitialized = true;
    }

    void WeatherWidget::OnTick(UINT_PTR timerId) {
        if (timerId == m_timerId && m_hwnd) {
            InvalidateRect(m_hwnd, nullptr, FALSE);
        }
    }

    LRESULT WeatherWidget::HandleWidgetMessage(UINT msg, WPARAM wParam, LPARAM lParam) {
        if (msg == WM_CREATE || (msg == WM_SHOWWINDOW && wParam == TRUE)) {
            if (m_hwnd) {
                SetTimer(m_hwnd, m_timerId, 30000, nullptr);
            }
        }
        return WidgetWindow::HandleWidgetMessage(msg, wParam, lParam);
    }

    bool WeatherWidget::IsInInteractiveArea(int clientX, int clientY) const {
        POINT pt{ clientX, clientY };
        return PtInRect(&m_tempToggleRect, pt);
    }

    void WeatherWidget::OnLButtonDown(int x, int y) {
        POINT pt{ x, y };
        if (PtInRect(&m_tempToggleRect, pt)) {
            m_weather.isCelsius = !m_weather.isCelsius;
            if (m_hwnd) InvalidateRect(m_hwnd, nullptr, FALSE);
        }
    }

    void WeatherWidget::OnRender() {
        if (!m_renderer) return;
        InitTypography();

        RECT rc{};
        GetClientRect(m_hwnd, &rc);
        float width = static_cast<float>(rc.right - rc.left);
        float height = static_cast<float>(rc.bottom - rc.top);

        // 1. Transparent background
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

        // Weather icon (sun with rays)
        D2D1_POINT_2F sunC = D2D1::Point2F(margin + 16.0f, headerY + 12.0f);
        m_renderer->DrawEllipse(D2D1::Ellipse(sunC, 4.5f, 4.5f), Colors::MutedLight, 1.0f);
        // Rays
        m_renderer->DrawLine(D2D1::Point2F(sunC.x - 7.0f, sunC.y), D2D1::Point2F(sunC.x - 5.5f, sunC.y), Colors::MutedLight, 1.0f);
        m_renderer->DrawLine(D2D1::Point2F(sunC.x + 5.5f, sunC.y), D2D1::Point2F(sunC.x + 7.0f, sunC.y), Colors::MutedLight, 1.0f);
        m_renderer->DrawLine(D2D1::Point2F(sunC.x, sunC.y - 7.0f), D2D1::Point2F(sunC.x, sunC.y - 5.5f), Colors::MutedLight, 1.0f);
        m_renderer->DrawLine(D2D1::Point2F(sunC.x, sunC.y + 5.5f), D2D1::Point2F(sunC.x, sunC.y + 7.0f), Colors::MutedLight, 1.0f);

        D2D1_RECT_F titleR = D2D1::RectF(margin + 28.0f, headerY, width - 60.0f, headerY + headerH);
        m_renderer->DrawText(L"ATMOSPHERE", titleR, Colors::FaintLight, m_formatHeader.Get());

        // Toggle button: °C / °F
        float btnW = 22.0f;
        float btnH = 17.0f;
        float btnX = width - margin - btnW - 8.0f;
        float btnY = headerY + 5.0f;

        m_tempToggleRect = {
            static_cast<LONG>(btnX),
            static_cast<LONG>(btnY),
            static_cast<LONG>(btnX + btnW),
            static_cast<LONG>(btnY + btnH)
        };

        D2D1_ROUNDED_RECT toggleR{ D2D1::RectF(btnX, btnY, btnX + btnW, btnY + btnH), 2.0f, 2.0f };
        m_renderer->DrawRoundedRectangle(toggleR, Colors::BorderLight, 1.0f);
        D2D1_RECT_F toggleTextR = D2D1::RectF(btnX, btnY, btnX + btnW, btnY + btnH);
        m_renderer->DrawText(m_weather.isCelsius ? L"°C" : L"°F", toggleTextR, Colors::MutedLight, m_formatBadge.Get());

        // Header divider line
        m_renderer->DrawLine(
            D2D1::Point2F(margin, margin + headerH),
            D2D1::Point2F(width - margin, margin + headerH),
            Colors::BorderLight, 1.0f
        );

        // 4. Main Temp & Location area
        float bodyTop = margin + headerH + 6.0f;

        int dispTemp = m_weather.isCelsius ? m_weather.tempC : static_cast<int>(std::round(m_weather.tempC * 9.0f / 5.0f + 32.0f));
        std::wstring tempStr = std::to_wstring(dispTemp) + (m_weather.isCelsius ? L"°C" : L"°F");

        D2D1_RECT_F tempRect = D2D1::RectF(margin + 12.0f, bodyTop, margin + 120.0f, bodyTop + 30.0f);
        m_renderer->DrawText(tempStr, tempRect, Colors::FgLight, m_formatBigTemp.Get());

        D2D1_RECT_F condRect = D2D1::RectF(margin + 12.0f, bodyTop + 28.0f, margin + 180.0f, bodyTop + 42.0f);
        m_renderer->DrawText(m_weather.condition, condRect, Colors::MutedLight, m_formatCondition.Get());

        std::wstring locStr = m_weather.city + L", " + m_weather.country;
        D2D1_RECT_F locRect = D2D1::RectF(margin + 12.0f, bodyTop + 42.0f, margin + 200.0f, bodyTop + 56.0f);
        m_renderer->DrawText(locStr, locRect, Colors::FaintLight, m_formatLocation.Get());

        // Weather icon tile on the right (square #EBE9E4 box)
        float tileW = 38.0f;
        float tileH = 38.0f;
        float tileX = width - margin - tileW - 12.0f;
        float tileY = bodyTop + 4.0f;
        D2D1_ROUNDED_RECT iconTileR{ D2D1::RectF(tileX, tileY, tileX + tileW, tileY + tileH), 4.0f, 4.0f };
        m_renderer->FillRoundedRectangle(iconTileR, Colors::BgLight);
        m_renderer->DrawRoundedRectangle(iconTileR, Colors::BorderLight, 1.0f);

        // Sun & cloud icon inside tile
        D2D1_POINT_2F tileSunC = D2D1::Point2F(tileX + 16.0f, tileY + 16.0f);
        m_renderer->DrawEllipse(D2D1::Ellipse(tileSunC, 5.0f, 5.0f), Colors::MutedLight, 1.0f);
        m_renderer->DrawLine(D2D1::Point2F(tileX + 12.0f, tileY + 25.0f), D2D1::Point2F(tileX + 28.0f, tileY + 25.0f), Colors::MutedLight, 1.5f);
        m_renderer->DrawLine(D2D1::Point2F(tileX + 14.0f, tileY + 28.0f), D2D1::Point2F(tileX + 26.0f, tileY + 28.0f), Colors::MutedLight, 1.5f);

        // 5. 3-Column stats row (HUM, WIND, UV)
        float statBoxY = bodyTop + 62.0f;
        float statBoxH = 26.0f;
        float statBoxW = width - (margin * 2.0f) - 20.0f;
        float statBoxX = margin + 10.0f;

        D2D1_ROUNDED_RECT statsR{ D2D1::RectF(statBoxX, statBoxY, statBoxX + statBoxW, statBoxY + statBoxH), 3.0f, 3.0f };
        m_renderer->FillRoundedRectangle(statsR, Colors::BgLight);
        m_renderer->DrawRoundedRectangle(statsR, Colors::BorderLight, 1.0f);

        float colW = statBoxW / 3.0f;
        // Divider 1
        m_renderer->DrawLine(D2D1::Point2F(statBoxX + colW, statBoxY), D2D1::Point2F(statBoxX + colW, statBoxY + statBoxH), Colors::BorderLight, 1.0f);
        // Divider 2
        m_renderer->DrawLine(D2D1::Point2F(statBoxX + colW * 2.0f, statBoxY), D2D1::Point2F(statBoxX + colW * 2.0f, statBoxY + statBoxH), Colors::BorderLight, 1.0f);

        // Col 1: HUM
        D2D1_RECT_F humLbl = D2D1::RectF(statBoxX, statBoxY + 1.0f, statBoxX + colW, statBoxY + 12.0f);
        m_renderer->DrawText(L"HUM", humLbl, Colors::FaintLight, m_formatStatLabel.Get());
        D2D1_RECT_F humVal = D2D1::RectF(statBoxX, statBoxY + 12.0f, statBoxX + colW, statBoxY + statBoxH - 1.0f);
        m_renderer->DrawText(std::to_wstring(m_weather.humidity) + L"%", humVal, Colors::FgLight, m_formatStatVal.Get());

        // Col 2: WIND
        D2D1_RECT_F windLbl = D2D1::RectF(statBoxX + colW, statBoxY + 1.0f, statBoxX + colW * 2.0f, statBoxY + 12.0f);
        m_renderer->DrawText(L"WIND", windLbl, Colors::FaintLight, m_formatStatLabel.Get());
        D2D1_RECT_F windVal = D2D1::RectF(statBoxX + colW, statBoxY + 12.0f, statBoxX + colW * 2.0f, statBoxY + statBoxH - 1.0f);
        m_renderer->DrawText(std::to_wstring(m_weather.windKmH) + L"km/h", windVal, Colors::FgLight, m_formatStatVal.Get());

        // Col 3: UV
        D2D1_RECT_F uvLbl = D2D1::RectF(statBoxX + colW * 2.0f, statBoxY + 1.0f, statBoxX + statBoxW, statBoxY + 12.0f);
        m_renderer->DrawText(L"UV", uvLbl, Colors::FaintLight, m_formatStatLabel.Get());
        D2D1_RECT_F uvVal = D2D1::RectF(statBoxX + colW * 2.0f, statBoxY + 12.0f, statBoxX + statBoxW, statBoxY + statBoxH - 1.0f);
        m_renderer->DrawText(std::to_wstring(m_weather.uvIndex), uvVal, Colors::FgLight, m_formatStatVal.Get());

        // 6. 4-Day Forecast row
        float fcY = statBoxY + statBoxH + 4.0f;
        float fcColW = (width - margin * 2.0f - 24.0f) / 4.0f;
        float fcStartX = margin + 12.0f;

        for (size_t i = 0; i < m_weather.forecast.size(); ++i) {
            float x0 = fcStartX + i * fcColW;
            float x1 = x0 + fcColW;

            // Day
            D2D1_RECT_F dayR = D2D1::RectF(x0, fcY, x1, fcY + 11.0f);
            m_renderer->DrawText(m_weather.forecast[i].day, dayR, Colors::FaintLight, m_formatForecastDay.Get());

            // Symbol
            D2D1_RECT_F symR = D2D1::RectF(x0, fcY + 10.0f, x1, fcY + 23.0f);
            m_renderer->DrawText(m_weather.forecast[i].symbol, symR, Colors::MutedLight, m_formatForecastSym.Get());

            // Temp
            int fcT = m_weather.isCelsius ? m_weather.forecast[i].tempC : static_cast<int>(std::round(m_weather.forecast[i].tempC * 9.0f / 5.0f + 32.0f));
            D2D1_RECT_F tempR = D2D1::RectF(x0, fcY + 22.0f, x1, fcY + 34.0f);
            m_renderer->DrawText(std::to_wstring(fcT) + L"°", tempR, Colors::FgLight, m_formatForecastTemp.Get());
        }

        // 7. Footer
        float footerY = height - margin - 20.0f;
        m_renderer->DrawLine(
            D2D1::Point2F(margin, footerY),
            D2D1::Point2F(width - margin, footerY),
            Colors::BorderLight, 1.0f
        );

        D2D1_RECT_F footL = D2D1::RectF(margin + 12.0f, footerY, width - 100.0f, height - margin);
        m_renderer->DrawText(L"METEOROLOGICAL", footL, Colors::FaintLight, m_formatFooter.Get());

        int dispH = m_weather.isCelsius ? m_weather.highC : static_cast<int>(std::round(m_weather.highC * 9.0f / 5.0f + 32.0f));
        int dispL = m_weather.isCelsius ? m_weather.lowC : static_cast<int>(std::round(m_weather.lowC * 9.0f / 5.0f + 32.0f));
        std::wstring hlStr = L"H:" + std::to_wstring(dispH) + L"° L:" + std::to_wstring(dispL) + L"°";

        ComPtr<IDWriteTextFormat> fmtRight;
        fmtRight = m_renderer->CreateTextFormat(
            L"Geist Mono", 9.0f, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_TEXT_ALIGNMENT_TRAILING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER
        );
        D2D1_RECT_F footR = D2D1::RectF(width - margin - 120.0f, footerY, width - margin - 12.0f, height - margin);
        if (fmtRight) {
            m_renderer->DrawText(hlStr, footR, Colors::FaintLight, fmtRight.Get());
        }

        DrawHoverToolbar(width);
    }

}
