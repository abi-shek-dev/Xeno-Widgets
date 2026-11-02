#include "MusicWidget.hpp"
#include <algorithm>
#include <cwchar>

namespace xe {

    MusicWidget::MusicWidget() {
        // Fallback demo track matching the screenshot
        m_track.title = L"Neon Horizon (Synthwave Mix)";
        m_track.artist = L"Kavinsky & The Midnight";
        m_track.progressSec = 48.0f;
        m_track.durationSec = 218.0f; // 3:38
        m_track.isPlaying = false;
    }

    MusicWidget::~MusicWidget() {
        if (m_hwnd) {
            KillTimer(m_hwnd, m_timerId);
        }
    }

    void MusicWidget::InitTypography() {
        if (m_typographyInitialized || !m_renderer) return;

        m_formatHeader = m_renderer->CreateTextFormat(
            L"Geist Mono", 11.0f, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER
        );

        m_formatTitle = m_renderer->CreateTextFormat(
            L"Geist Sans", 12.5f, DWRITE_FONT_WEIGHT_SEMI_BOLD, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER
        );

        m_formatArtist = m_renderer->CreateTextFormat(
            L"Geist Sans", 11.0f, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER
        );

        m_formatTime = m_renderer->CreateTextFormat(
            L"Geist Mono", 9.0f, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER
        );

        m_formatBtn = m_renderer->CreateTextFormat(
            L"Geist Mono", 10.0f, DWRITE_FONT_WEIGHT_MEDIUM, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER
        );

        m_formatFooter = m_renderer->CreateTextFormat(
            L"Geist Mono", 9.0f, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER
        );

        m_typographyInitialized = true;
    }

    void MusicWidget::OnTick(UINT_PTR timerId) {
        if (timerId == m_timerId) {
            // Check Windows SMTC live media
            MediaInfo liveInfo = SMTCProvider::Instance().GetCurrentMedia();
            if (liveInfo.hasMedia) {
                m_usingSMTC = true;
                if (!liveInfo.title.empty()) m_track.title = liveInfo.title;
                if (!liveInfo.artist.empty()) m_track.artist = liveInfo.artist;
                m_track.isPlaying = liveInfo.isPlaying;
                m_track.progressSec = static_cast<float>(liveInfo.progressSeconds);
                m_track.durationSec = static_cast<float>(liveInfo.durationSeconds);
            } else {
                m_usingSMTC = false;
                if (m_track.isPlaying) {
                    m_track.progressSec += 1.0f;
                    if (m_track.progressSec > m_track.durationSec) {
                        m_track.progressSec = 0.0f;
                    }
                }
            }

            if (m_hwnd) {
                InvalidateRect(m_hwnd, nullptr, FALSE);
            }
        }
    }

    LRESULT MusicWidget::HandleWidgetMessage(UINT msg, WPARAM wParam, LPARAM lParam) {
        if (msg == WM_CREATE || (msg == WM_SHOWWINDOW && wParam == TRUE)) {
            if (m_hwnd) {
                SetTimer(m_hwnd, m_timerId, 1000, nullptr);
            }
        }
        return WidgetWindow::HandleWidgetMessage(msg, wParam, lParam);
    }

    bool MusicWidget::IsInInteractiveArea(int clientX, int clientY) const {
        POINT pt{ clientX, clientY };
        return PtInRect(&m_btnPrev, pt) || PtInRect(&m_btnPlay, pt) || PtInRect(&m_btnNext, pt);
    }

    void MusicWidget::OnLButtonDown(int x, int y) {
        POINT pt{ x, y };

        if (PtInRect(&m_btnPlay, pt)) {
            m_track.isPlaying = !m_track.isPlaying;
            if (m_hwnd) InvalidateRect(m_hwnd, nullptr, FALSE);
        } else if (PtInRect(&m_btnPrev, pt)) {
            m_track.progressSec = 0.0f;
            if (m_hwnd) InvalidateRect(m_hwnd, nullptr, FALSE);
        } else if (PtInRect(&m_btnNext, pt)) {
            m_track.progressSec = 0.0f;
            if (m_hwnd) InvalidateRect(m_hwnd, nullptr, FALSE);
        }
    }

    void MusicWidget::OnRender() {
        if (!m_renderer) return;
        InitTypography();

        RECT rc{};
        GetClientRect(m_hwnd, &rc);
        float width = static_cast<float>(rc.right - rc.left);
        float height = static_cast<float>(rc.bottom - rc.top);

        // 1. Clear transparent background
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

        // Music note icon
        float iconX = margin + 14.0f;
        float iconY = headerY + 8.0f;
        m_renderer->DrawLine(D2D1::Point2F(iconX + 3.0f, iconY + 2.0f), D2D1::Point2F(iconX + 3.0f, iconY + 11.0f), Colors::MutedLight, 1.2f);
        m_renderer->FillEllipse(D2D1::Ellipse(D2D1::Point2F(iconX + 2.0f, iconY + 11.0f), 2.5f, 2.0f), Colors::MutedLight);
        m_renderer->DrawLine(D2D1::Point2F(iconX + 3.0f, iconY + 2.0f), D2D1::Point2F(iconX + 9.0f, iconY + 4.0f), Colors::MutedLight, 1.5f);
        m_renderer->DrawLine(D2D1::Point2F(iconX + 9.0f, iconY + 4.0f), D2D1::Point2F(iconX + 9.0f, iconY + 13.0f), Colors::MutedLight, 1.2f);
        m_renderer->FillEllipse(D2D1::Ellipse(D2D1::Point2F(iconX + 8.0f, iconY + 13.0f), 2.5f, 2.0f), Colors::MutedLight);

        D2D1_RECT_F headerLabelR = D2D1::RectF(margin + 28.0f, headerY, width - 80.0f, headerY + headerH);
        m_renderer->DrawText(L"MEDIA PLAYER", headerLabelR, Colors::FaintLight, m_formatHeader.Get());

        // 5-bar Equalizer icon on right
        float eqRight = width - margin - 12.0f;
        float eqY = headerY + 16.0f;
        float barHeights[] = { 4.0f, 8.0f, 6.0f, 10.0f, 7.0f };
        for (int i = 0; i < 5; ++i) {
            float bx = eqRight - (4 - i) * 3.5f;
            float bh = barHeights[i];
            m_renderer->DrawLine(D2D1::Point2F(bx, eqY), D2D1::Point2F(bx, eqY - bh), Colors::MutedLight, 1.5f);
        }

        // Header divider line
        m_renderer->DrawLine(
            D2D1::Point2F(margin, margin + headerH),
            D2D1::Point2F(width - margin, margin + headerH),
            Colors::BorderLight, 1.0f
        );

        // 4. Body Content
        float bodyTop = margin + headerH + 8.0f;

        // Track title
        D2D1_RECT_F titleR = D2D1::RectF(margin + 12.0f, bodyTop, width - margin - 12.0f, bodyTop + 18.0f);
        m_renderer->DrawText(m_track.title, titleR, Colors::FgLight, m_formatTitle.Get());

        // Artist
        D2D1_RECT_F artistR = D2D1::RectF(margin + 12.0f, bodyTop + 18.0f, width - margin - 12.0f, bodyTop + 33.0f);
        m_renderer->DrawText(m_track.artist, artistR, Colors::MutedLight, m_formatArtist.Get());

        // Album
        D2D1_RECT_F albumR = D2D1::RectF(margin + 12.0f, bodyTop + 33.0f, width - margin - 12.0f, bodyTop + 46.0f);
        m_renderer->DrawText(L"Endless Summer", albumR, Colors::FaintLight, m_formatFooter.Get());

        // Progress bar (height 3px, 12px margins)
        float progY = bodyTop + 54.0f;
        float progW = width - (margin * 2.0f) - 24.0f;
        float progX = margin + 12.0f;

        // Track background
        D2D1_RECT_F progBgR = D2D1::RectF(progX, progY, progX + progW, progY + 3.0f);
        m_renderer->FillRectangle(progBgR, Colors::BorderLight);

        // Fill bar
        float duration = m_track.durationSec > 0.0f ? m_track.durationSec : 218.0f;
        float progressRatio = std::clamp(m_track.progressSec / duration, 0.0f, 1.0f);
        float fillW = progW * progressRatio;
        if (fillW > 0.0f) {
            D2D1_RECT_F progFgR = D2D1::RectF(progX, progY, progX + fillW, progY + 3.0f);
            m_renderer->FillRectangle(progFgR, Colors::FgLight);
        }

        // Time labels: 0:48 (left), 3:38 (right)
        int curSec = static_cast<int>(m_track.progressSec);
        int totSec = static_cast<int>(duration);
        wchar_t timeElapsedBuf[32];
        swprintf_s(timeElapsedBuf, L"%d:%02d", curSec / 60, curSec % 60);

        wchar_t timeTotalBuf[32];
        swprintf_s(timeTotalBuf, L"%d:%02d", totSec / 60, totSec % 60);

        D2D1_RECT_F elapR = D2D1::RectF(progX, progY + 4.0f, progX + 50.0f, progY + 16.0f);
        m_renderer->DrawText(timeElapsedBuf, elapR, Colors::FaintLight, m_formatTime.Get());

        ComPtr<IDWriteTextFormat> fmtRight;
        fmtRight = m_renderer->CreateTextFormat(
            L"Geist Mono", 9.0f, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_TEXT_ALIGNMENT_TRAILING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER
        );
        D2D1_RECT_F totR = D2D1::RectF(progX + progW - 50.0f, progY + 4.0f, progX + progW, progY + 16.0f);
        if (fmtRight) {
            m_renderer->DrawText(timeTotalBuf, totR, Colors::FaintLight, fmtRight.Get());
        }

        // 5. Controls Row: Volume on left, Playback buttons on right
        float ctrlY = progY + 22.0f;

        // Speaker icon
        float spkX = margin + 12.0f;
        float spkY = ctrlY + 6.0f;
        m_renderer->DrawLine(D2D1::Point2F(spkX, spkY - 2.0f), D2D1::Point2F(spkX + 2.0f, spkY - 2.0f), Colors::MutedLight, 1.2f);
        m_renderer->DrawLine(D2D1::Point2F(spkX + 2.0f, spkY - 4.0f), D2D1::Point2F(spkX + 5.0f, spkY - 6.0f), Colors::MutedLight, 1.2f);
        m_renderer->DrawLine(D2D1::Point2F(spkX + 5.0f, spkY - 6.0f), D2D1::Point2F(spkX + 5.0f, spkY + 6.0f), Colors::MutedLight, 1.2f);
        m_renderer->DrawLine(D2D1::Point2F(spkX + 5.0f, spkY + 6.0f), D2D1::Point2F(spkX + 2.0f, spkY + 4.0f), Colors::MutedLight, 1.2f);
        m_renderer->DrawLine(D2D1::Point2F(spkX + 2.0f, spkY + 4.0f), D2D1::Point2F(spkX, spkY + 4.0f), Colors::MutedLight, 1.2f);
        m_renderer->DrawLine(D2D1::Point2F(spkX, spkY + 4.0f), D2D1::Point2F(spkX, spkY - 2.0f), Colors::MutedLight, 1.2f);

        // Volume track bar + knob
        float volX = spkX + 12.0f;
        float volW = 46.0f;
        float volBarY = spkY;
        m_renderer->DrawLine(D2D1::Point2F(volX, volBarY), D2D1::Point2F(volX + volW, volBarY), Colors::BorderLight, 2.0f);
        // Knob at ~70%
        float knobX = volX + volW * 0.7f;
        m_renderer->FillEllipse(D2D1::Ellipse(D2D1::Point2F(knobX, volBarY), 4.0f, 4.0f), Colors::FgLight);

        // Transport buttons on right (Prev, Play, Next)
        float btnAreaR = width - margin - 12.0f;
        float btnNextX = btnAreaR - 24.0f;
        float btnPlayX = btnNextX - 28.0f - 4.0f;
        float btnPrevX = btnPlayX - 24.0f - 4.0f;
        float btnY = ctrlY;
        float btnH = 22.0f;

        // Prev Button
        m_btnPrev = { static_cast<LONG>(btnPrevX), static_cast<LONG>(btnY), static_cast<LONG>(btnPrevX + 24.0f), static_cast<LONG>(btnY + btnH) };
        D2D1_ROUNDED_RECT prevR{ D2D1::RectF(btnPrevX, btnY, btnPrevX + 24.0f, btnY + btnH), 2.0f, 2.0f };
        m_renderer->DrawRoundedRectangle(prevR, Colors::BorderLight, 1.0f);
        m_renderer->DrawText(L"|\u25C0", D2D1::RectF(btnPrevX, btnY, btnPrevX + 24.0f, btnY + btnH), Colors::MutedLight, m_formatBtn.Get());

        // Play Button (Solid black button with white triangle)
        float playW = 28.0f;
        float playH = 24.0f;
        float playY = btnY - 1.0f;
        m_btnPlay = { static_cast<LONG>(btnPlayX), static_cast<LONG>(playY), static_cast<LONG>(btnPlayX + playW), static_cast<LONG>(playY + playH) };
        D2D1_ROUNDED_RECT playR{ D2D1::RectF(btnPlayX, playY, btnPlayX + playW, playY + playH), 3.0f, 3.0f };
        m_renderer->FillRoundedRectangle(playR, Colors::FgLight);
        // White play/pause glyph
        m_renderer->DrawText(m_track.isPlaying ? L"\u25AE\u25AE" : L"\u25B6", D2D1::RectF(btnPlayX, playY, btnPlayX + playW, playY + playH), Colors::BgLight, m_formatBtn.Get());

        // Next Button
        m_btnNext = { static_cast<LONG>(btnNextX), static_cast<LONG>(btnY), static_cast<LONG>(btnNextX + 24.0f), static_cast<LONG>(btnY + btnH) };
        D2D1_ROUNDED_RECT nextR{ D2D1::RectF(btnNextX, btnY, btnNextX + 24.0f, btnY + btnH), 2.0f, 2.0f };
        m_renderer->DrawRoundedRectangle(nextR, Colors::BorderLight, 1.0f);
        m_renderer->DrawText(L"\u25B6|", D2D1::RectF(btnNextX, btnY, btnNextX + 24.0f, btnY + btnH), Colors::MutedLight, m_formatBtn.Get());

        // 6. Footer
        float footerY = height - margin - 20.0f;
        m_renderer->DrawLine(
            D2D1::Point2F(margin, footerY),
            D2D1::Point2F(width - margin, footerY),
            Colors::BorderLight, 1.0f
        );

        D2D1_RECT_F footL = D2D1::RectF(margin + 12.0f, footerY, width - 100.0f, height - margin);
        m_renderer->DrawText(L"DIRECTSOUND", footL, Colors::FaintLight, m_formatFooter.Get());

        D2D1_RECT_F footR = D2D1::RectF(width - margin - 80.0f, footerY, width - margin - 12.0f, height - margin);
        if (fmtRight) {
            m_renderer->DrawText(L"24-BIT", footR, Colors::FaintLight, fmtRight.Get());
        }

        DrawHoverToolbar(width);
    }

}
