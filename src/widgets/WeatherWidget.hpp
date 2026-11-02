#pragma once

#include "Common.hpp"
#include "../windows/WidgetWindow.hpp"

namespace xe {

    struct WeatherInfo {
        std::wstring city = L"San Francisco";
        std::wstring country = L"CA, USA";
        int tempC = 19;
        std::wstring condition = L"Partly Cloudy";
        int humidity = 64;
        int windKmH = 12;
        int uvIndex = 3;
        int highC = 22;
        int lowC = 13;
        bool isCelsius = true;

        struct ForecastDay {
            std::wstring day;
            int tempC;
            std::wstring symbol;
        };
        std::vector<ForecastDay> forecast = {
            { L"Tue", 20, L"◑" },
            { L"Wed", 18, L"○" },
            { L"Thu", 16, L"≋" },
            { L"Fri", 21, L"○" }
        };
    };

    class WeatherWidget : public WidgetWindow {
    public:
        WeatherWidget();
        ~WeatherWidget() override;

        void OnTick(UINT_PTR timerId) override;

    protected:
        void OnRender() override;
        void OnLButtonDown(int x, int y) override;
        bool IsInInteractiveArea(int clientX, int clientY) const override;
        LRESULT HandleWidgetMessage(UINT msg, WPARAM wParam, LPARAM lParam) override;

    private:
        void InitTypography();

        WeatherInfo m_weather;
        UINT_PTR m_timerId = 4;
        RECT m_tempToggleRect{};

        ComPtr<IDWriteTextFormat> m_formatHeader;
        ComPtr<IDWriteTextFormat> m_formatBadge;
        ComPtr<IDWriteTextFormat> m_formatBigTemp;
        ComPtr<IDWriteTextFormat> m_formatCondition;
        ComPtr<IDWriteTextFormat> m_formatLocation;
        ComPtr<IDWriteTextFormat> m_formatStatLabel;
        ComPtr<IDWriteTextFormat> m_formatStatVal;
        ComPtr<IDWriteTextFormat> m_formatForecastDay;
        ComPtr<IDWriteTextFormat> m_formatForecastSym;
        ComPtr<IDWriteTextFormat> m_formatForecastTemp;
        ComPtr<IDWriteTextFormat> m_formatFooter;
        bool m_typographyInitialized = false;
    };

}
