#pragma once

#include "Common.hpp"
#include "../windows/WidgetWindow.hpp"
#include "../data/TelemetryProvider.hpp"

namespace xe {

    class SystemMonitorWidget : public WidgetWindow {
    public:
        SystemMonitorWidget();
        ~SystemMonitorWidget() override;

        void OnTick(UINT_PTR timerId) override;

    protected:
        void OnRender() override;
        LRESULT HandleWidgetMessage(UINT msg, WPARAM wParam, LPARAM lParam) override;

    private:
        void InitTypography();

        TelemetryData m_data;
        UINT_PTR m_timerId = 3;

        ComPtr<IDWriteTextFormat> m_formatHeader;
        ComPtr<IDWriteTextFormat> m_formatBadge;
        ComPtr<IDWriteTextFormat> m_formatMetricLabel;
        ComPtr<IDWriteTextFormat> m_formatCpuValue;
        ComPtr<IDWriteTextFormat> m_formatRamValue;
        ComPtr<IDWriteTextFormat> m_formatNetValue;
        ComPtr<IDWriteTextFormat> m_formatFooter;
        bool m_typographyInitialized = false;
    };

}
