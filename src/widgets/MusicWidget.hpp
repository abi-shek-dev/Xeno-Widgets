#pragma once

#include "Common.hpp"
#include "../windows/WidgetWindow.hpp"

namespace xe {

    class MusicWidget : public WidgetWindow {
    public:
        struct TrackInfo {
            std::wstring title = L"Nocturne in E-flat Major";
            std::wstring artist = L"Fr\u00E9d\u00E9ric Chopin";
            float progressSec = 47.0f;
            float durationSec = 271.0f; // 4:31
            bool isPlaying = false;
        };

        MusicWidget();
        ~MusicWidget() override;

        void OnTick(UINT_PTR timerId) override;

    protected:
        void OnRender() override;
        void OnLButtonDown(int x, int y) override;
        bool IsInInteractiveArea(int clientX, int clientY) const override;
        LRESULT HandleWidgetMessage(UINT msg, WPARAM wParam, LPARAM lParam) override;

    private:
        void InitTypography();

        TrackInfo m_track;
        UINT_PTR m_timerId = 2;

        RECT m_btnPrev{};
        RECT m_btnPlay{};
        RECT m_btnNext{};

        ComPtr<IDWriteTextFormat> m_formatHeader;
        ComPtr<IDWriteTextFormat> m_formatBadge;
        ComPtr<IDWriteTextFormat> m_formatTitle;
        ComPtr<IDWriteTextFormat> m_formatArtist;
        ComPtr<IDWriteTextFormat> m_formatTime;
        ComPtr<IDWriteTextFormat> m_formatBtn;
        ComPtr<IDWriteTextFormat> m_formatFooter;
        bool m_typographyInitialized = false;
    };

}
