#pragma once

#include "Common.hpp"
#include "../windows/WidgetWindow.hpp"
#include <vector>

namespace xe {

    struct CalEvent {
        std::wstring id;
        std::wstring title;
        std::wstring time;
        std::wstring category;
    };

    class CalendarWidget : public WidgetWindow {
    public:
        CalendarWidget();
        ~CalendarWidget() override;

        void OnTick(UINT_PTR timerId) override;

    protected:
        void OnRender() override;
        void OnLButtonDown(int x, int y) override;
        bool IsInInteractiveArea(int clientX, int clientY) const override;
        LRESULT HandleWidgetMessage(UINT msg, WPARAM wParam, LPARAM lParam) override;

    private:
        void InitTypography();

        int m_selectedDay = 2;
        UINT_PTR m_timerId = 6;
        std::vector<CalEvent> m_events;
        std::vector<std::pair<RECT, int>> m_dayRects;

        ComPtr<IDWriteTextFormat> m_formatHeader;
        ComPtr<IDWriteTextFormat> m_formatDayHeader;
        ComPtr<IDWriteTextFormat> m_formatDayNum;
        ComPtr<IDWriteTextFormat> m_formatSection;
        ComPtr<IDWriteTextFormat> m_formatEventTitle;
        ComPtr<IDWriteTextFormat> m_formatEventTime;
        ComPtr<IDWriteTextFormat> m_formatMono;
        bool m_typographyInitialized = false;
    };

}
