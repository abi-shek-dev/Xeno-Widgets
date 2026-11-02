#include "MusicWidget.hpp"
#include <algorithm>
#include <cstdio>

namespace xe {

    static std::wstring FormatTrackTime(float totalSeconds) {
        int total = std::max(0, static_cast<int>(totalSeconds));
        int m = total / 60;
        int s = total % 60;
        wchar_t buf[16]{};
        swprintf_s(buf, L"%d:%02d", m, s);
        return buf;
    }

    MusicWidget::MusicWidget() = default;

    MusicWidget::~MusicWidget() {
        if (m_hwnd) {
            KillTimer(m_hwnd, m_timerId);
        }
    }

    void MusicWidget::InitTypography() {
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

        // Badge: 8.5pt ≈ 11.0f DIPs
        m_formatBadge = m_renderer->CreateTextFormat(
            L"Geist Mono",
            11.0f,
            DWRITE_FONT_WEIGHT_MEDIUM,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_TEXT_ALIGNMENT_CENTER,
            DWRITE_PARAGRAPH_ALIGNMENT_CENTER
        );

        // Title: Geist Sans 14pt ≈ 18.0f DIPs Bold
        m_formatTitle = m_renderer->CreateTextFormat(
            L"Geist Sans",
            17.0f,
            DWRITE_FONT_WEIGHT_SEMI_BOLD,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_TEXT_ALIGNMENT_LEADING,
            DWRITE_PARAGRAPH_ALIGNMENT_NEAR
        );

        // Artist: Geist Sans 11pt ≈ 14.5f DIPs
        m_formatArtist = m_renderer->CreateTextFormat(
            L"Geist Sans",
            14.0f,
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_TEXT_ALIGNMENT_LEADING,
            DWRITE_PARAGRAPH_ALIGNMENT_NEAR
        );

        // Time labels: Geist Mono 9pt ≈ 12.0f DIPs
        m_formatTime = m_renderer->CreateTextFormat(
            L"Geist Mono",
            11.5f,
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_TEXT_ALIGNMENT_LEADING,
            DWRITE_PARAGRAPH_ALIGNMENT_CENTER
        );

        // Buttons: Geist Mono 10pt ≈ 13.0f DIPs Center
        m_formatBtn = m_renderer->CreateTextFormat(
            L"Geist Mono",
            13.0f,
            DWRITE_FONT_WEIGHT_MEDIUM,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_TEXT_ALIGNMENT_CENTER,
            DWRITE_PARAGRAPH_ALIGNMENT_CENTER
        );

        // Footer: Geist Mono 9pt ≈ 12.0f DIPs
        m_formatFooter = m_renderer->CreateTextFormat(
            L"Geist Mono",
            11.5f,
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_TEXT_ALIGNMENT_LEADING,
            DWRITE_PARAGRAPH_ALIGNMENT_CENTER
        );

        m_typographyInitialized = true;
    }

    void MusicWidget::OnTick(UINT_PTR timerId) {
        if (timerId == m_timerId) {
            if (m_track.isPlaying) {
                m_track.progressSec += 1.0f;
                if (m_track.progressSec > m_track.durationSec) {
                    m_track.progressSec = 0.0f;
                }
                if (m_hwnd) {
                    InvalidateRect(m_hwnd, nullptr, FALSE);
                }
            }
        }
    }

    bool MusicWidget::IsInInteractiveArea(int clientX, int clientY) const {
        POINT pt{ clientX, clientY };
        return PtInRect(&m_btnPrev, pt) || PtInRect(&m_btnPlay, pt) || PtInRect(&m_btnNext, pt);
    }

    void MusicWidget::OnLButtonDown(int x, int y) {
        POINT pt{ x, y };
        if (PtInRect(&m_btnPlay, pt)) {
            m_track.isPlaying = !m_track.isPlaying;
            if (m_hwnd) {
                InvalidateRect(m_hwnd, nullptr, FALSE);
            }
            return;
        }

        if (PtInRect(&m_btnPrev, pt)) {
            m_track.progressSec = 0.0f;
            if (m_hwnd) {
                InvalidateRect(m_hwnd, nullptr, FALSE);
            }
            return;
        }

        if (PtInRect(&m_btnNext, pt)) {
            m_track.progressSec = 0.0f;
            if (m_hwnd) {
                InvalidateRect(m_hwnd, nullptr, FALSE);
            }
            return;
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

    void MusicWidget::OnRender() {
        if (!m_renderer) return;
        InitTypography();

        RECT rc{};
        GetClientRect(m_hwnd, &rc);
        float width = static_cast<float>(rc.right - rc.left);
        float height = static_cast<float>(rc.bottom - rc.top);

        // 1. Clear transparent background for DWM compositing
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

        // 3. Header row (32px, y=6 to y=38)
        D2D1_RECT_F headerLabelRect = D2D1::RectF(margin + 14.0f, margin, margin + 200.0f, margin + 32.0f);
        m_renderer->DrawText(L"AMBIENT SOUND & MEDIA", headerLabelRect, Colors::MutedDark, m_formatHeader.Get());

        // Status Badge: "PLAYING" or "PAUSED"
        float badgeW = 62.0f;
        float badgeH = 18.0f;
        float badgeX = width - margin - badgeW - 60.0f; // leave room for hover toolbar
        float badgeY = margin + 7.0f;
        D2D1_ROUNDED_RECT badgeRect{
            D2D1::RectF(badgeX, badgeY, badgeX + badgeW, badgeY + badgeH),
            3.0f,
            3.0f
        };
        m_renderer->FillRoundedRectangle(badgeRect, Colors::BgDark);
        m_renderer->DrawRoundedRectangle(badgeRect, Colors::BorderDark, 1.0f);

        D2D1_RECT_F badgeTextRect = D2D1::RectF(badgeX, badgeY, badgeX + badgeW, badgeY + badgeH);
        m_renderer->DrawText(
            m_track.isPlaying ? L"PLAYING" : L"PAUSED",
            badgeTextRect,
            m_track.isPlaying ? Colors::FgDark : Colors::MutedDark,
            m_formatBadge.Get()
        );

        // 4. Header divider line at y=38
        m_renderer->DrawLine(
            D2D1::Point2F(margin, margin + 32.0f),
            D2D1::Point2F(width - margin, margin + 32.0f),
            Colors::BorderDark,
            1.0f
        );

        // 5. Track Title at y=46
        float contentMargin = 20.0f;
        D2D1_RECT_F titleRect = D2D1::RectF(contentMargin, margin + 38.0f, width - contentMargin, margin + 60.0f);
        m_renderer->DrawText(m_track.title, titleRect, Colors::FgDark, m_formatTitle.Get());

        // 6. Artist at y=68
        D2D1_RECT_F artistRect = D2D1::RectF(contentMargin, margin + 60.0f, width - contentMargin, margin + 78.0f);
        m_renderer->DrawText(m_track.artist, artistRect, Colors::MutedDark, m_formatArtist.Get());

        // 7. Progress bar at y=90
        float barY = margin + 84.0f;
        float barW = width - (contentMargin * 2.0f);
        float barH = 3.0f;

        D2D1_RECT_F bgProgressRect = D2D1::RectF(contentMargin, barY, contentMargin + barW, barY + barH);
        m_renderer->FillRectangle(bgProgressRect, Colors::BorderDark);

        float progress = std::clamp(m_track.progressSec / m_track.durationSec, 0.0f, 1.0f);
        float fillW = progress * barW;
        if (fillW > 0.0f) {
            D2D1_RECT_F fgProgressRect = D2D1::RectF(contentMargin, barY, contentMargin + fillW, barY + barH);
            m_renderer->FillRectangle(fgProgressRect, Colors::FgDark);
        }

        // Time labels (elapsed & total)
        std::wstring elapsedStr = FormatTrackTime(m_track.progressSec);
        std::wstring totalStr = FormatTrackTime(m_track.durationSec);

        D2D1_RECT_F elapsedRect = D2D1::RectF(contentMargin, barY + 5.0f, contentMargin + 80.0f, barY + 20.0f);
        m_renderer->DrawText(elapsedStr, elapsedRect, Colors::MutedDark, m_formatTime.Get());

        // Format right-aligned total time
        ComPtr<IDWriteTextFormat> formatRightTime = m_renderer->CreateTextFormat(
            L"Geist Mono",
            11.5f,
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_TEXT_ALIGNMENT_TRAILING,
            DWRITE_PARAGRAPH_ALIGNMENT_CENTER
        );
        D2D1_RECT_F totalRect = D2D1::RectF(width - contentMargin - 80.0f, barY + 5.0f, width - contentMargin, barY + 20.0f);
        m_renderer->DrawText(totalStr, totalRect, Colors::MutedDark, formatRightTime.Get());

        // 8. Control buttons row at y=118
        float btnY = margin + 112.0f;
        float btnH = 28.0f;
        float centerX = width / 2.0f;

        // Play Button (48x28)
        float playW = 48.0f;
        float playX = centerX - (playW / 2.0f);
        m_btnPlay = {
            static_cast<LONG>(playX),
            static_cast<LONG>(btnY),
            static_cast<LONG>(playX + playW),
            static_cast<LONG>(btnY + btnH)
        };

        // Prev Button (36x28)
        float prevW = 36.0f;
        float prevX = playX - 12.0f - prevW;
        m_btnPrev = {
            static_cast<LONG>(prevX),
            static_cast<LONG>(btnY),
            static_cast<LONG>(prevX + prevW),
            static_cast<LONG>(btnY + btnH)
        };

        // Next Button (36x28)
        float nextW = 36.0f;
        float nextX = playX + playW + 12.0f;
        m_btnNext = {
            static_cast<LONG>(nextX),
            static_cast<LONG>(btnY),
            static_cast<LONG>(nextX + nextW),
            static_cast<LONG>(btnY + btnH)
        };

        // Render Prev Button
        D2D1_ROUNDED_RECT prevRect{
            D2D1::RectF(prevX, btnY, prevX + prevW, btnY + btnH),
            3.0f, 3.0f
        };
        m_renderer->FillRoundedRectangle(prevRect, Colors::BgDark);
        m_renderer->DrawRoundedRectangle(prevRect, Colors::BorderDark, 1.0f);
        m_renderer->DrawText(
            L"\u25C0\u25C0",
            D2D1::RectF(prevX, btnY, prevX + prevW, btnY + btnH),
            Colors::FgDark,
            m_formatBtn.Get()
        );

        // Render Play/Pause Button
        D2D1_ROUNDED_RECT playRect{
            D2D1::RectF(playX, btnY, playX + playW, btnY + btnH),
            3.0f, 3.0f
        };
        if (m_track.isPlaying) {
            m_renderer->FillRoundedRectangle(playRect, Colors::FgDark);
            m_renderer->DrawRoundedRectangle(playRect, Colors::FgDark, 1.0f);
            m_renderer->DrawText(
                L"\u25AE\u25AE", // ❚❚ pause glyph
                D2D1::RectF(playX, btnY, playX + playW, btnY + btnH),
                Colors::BgDark,
                m_formatBtn.Get()
            );
        } else {
            m_renderer->FillRoundedRectangle(playRect, Colors::BgDark);
            m_renderer->DrawRoundedRectangle(playRect, Colors::BorderDark, 1.0f);
            m_renderer->DrawText(
                L"\u25B6", // ▶ play glyph
                D2D1::RectF(playX, btnY, playX + playW, btnY + btnH),
                Colors::FgDark,
                m_formatBtn.Get()
            );
        }

        // Render Next Button
        D2D1_ROUNDED_RECT nextRect{
            D2D1::RectF(nextX, btnY, nextX + nextW, btnY + btnH),
            3.0f, 3.0f
        };
        m_renderer->FillRoundedRectangle(nextRect, Colors::BgDark);
        m_renderer->DrawRoundedRectangle(nextRect, Colors::BorderDark, 1.0f);
        m_renderer->DrawText(
            L"\u25B6\u25B6",
            D2D1::RectF(nextX, btnY, nextX + nextW, btnY + btnH),
            Colors::FgDark,
            m_formatBtn.Get()
        );

        // 9. Volume row at y=156
        float volY = margin + 148.0f;
        D2D1_RECT_F volLabelRect = D2D1::RectF(contentMargin, volY, contentMargin + 32.0f, volY + 16.0f);
        m_renderer->DrawText(L"VOL", volLabelRect, Colors::MutedDark, m_formatFooter.Get());

        float volBarX = contentMargin + 36.0f;
        float volBarW = 60.0f;
        float volBarY = volY + 6.0f;
        D2D1_RECT_F volBgRect = D2D1::RectF(volBarX, volBarY, volBarX + volBarW, volBarY + 3.0f);
        m_renderer->FillRectangle(volBgRect, Colors::BorderDark);

        float volFillW = volBarW * 0.70f;
        D2D1_RECT_F volFillRect = D2D1::RectF(volBarX, volBarY, volBarX + volFillW, volBarY + 3.0f);
        m_renderer->FillRectangle(volFillRect, Colors::FgDark);

        D2D1_RECT_F volPctRect = D2D1::RectF(volBarX + volBarW + 8.0f, volY, volBarX + volBarW + 45.0f, volY + 16.0f);
        m_renderer->DrawText(L"70%", volPctRect, Colors::MutedDark, m_formatFooter.Get());

        // Footer Section Label on right
        ComPtr<IDWriteTextFormat> formatRightFooter = m_renderer->CreateTextFormat(
            L"Geist Mono",
            11.5f,
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_TEXT_ALIGNMENT_TRAILING,
            DWRITE_PARAGRAPH_ALIGNMENT_CENTER
        );
        D2D1_RECT_F mediaSectionRect = D2D1::RectF(width - contentMargin - 120.0f, volY, width - contentMargin, volY + 16.0f);
        m_renderer->DrawText(L"03 // MEDIA", mediaSectionRect, Colors::FaintDark, formatRightFooter.Get());

        // 10. Floating hover toolbar
        DrawHoverToolbar(width);
    }

}
