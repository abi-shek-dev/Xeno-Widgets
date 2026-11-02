#pragma once

#include "Common.hpp"
#include "../windows/WidgetWindow.hpp"
#include "../data/SystemTimeProvider.hpp"

namespace xe {

    class ClockWidget : public WidgetWindow {
    public:
        ClockWidget();
        ~ClockWidget() override;

        void OnTick(UINT_PTR timerId) override;

    protected:
        void OnRender() override;
        LRESULT HandleWidgetMessage(UINT msg, WPARAM wParam, LPARAM lParam) override;

    private:
        void InitTypography();

        SystemTimeProvider::TimeData m_timeData;
        UINT_PTR m_timerId = 1;

        ComPtr<IDWriteTextFormat> m_formatHeader;
        ComPtr<IDWriteTextFormat> m_formatTime;
        ComPtr<IDWriteTextFormat> m_formatDate;
        ComPtr<IDWriteTextFormat> m_formatFooter;
        bool m_typographyInitialized = false;
    };

}
