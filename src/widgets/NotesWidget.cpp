#include "NotesWidget.hpp"
#include <shlobj.h>
#include <fstream>
#include <algorithm>

namespace xe {

    NotesWidget::NotesWidget() {
        LoadNotes();
        if (m_notes.empty()) {
            m_notes = {
                { L"1", L"Project Milestone", L"Ship XE Widgets v2.0 native C++ desktop suite.", false, true },
                { L"2", L"UI Polish Checklist", L"Thin borders, generous whitespace, precise typography.", false, false },
                { L"3", L"Direct2D Verification", L"Hardware accelerated 60fps rendering verified.", true, false }
            };
        }
    }

    NotesWidget::~NotesWidget() {
        if (m_hwnd) {
            KillTimer(m_hwnd, m_timerId);
        }
        SaveNotes();
    }

    void NotesWidget::LoadNotes() {
        // Safe AppData path
        PWSTR appDataPath = nullptr;
        if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &appDataPath)) && appDataPath) {
            std::wstring path = std::wstring(appDataPath) + L"\\XEWidgets\\notes.txt";
            CoTaskMemFree(appDataPath);
            std::wifstream f(path);
            if (f.is_open()) {
                std::wstring line;
                while (std::getline(f, line)) {
                    if (line.empty()) continue;
                    size_t sep = line.find(L'|');
                    if (sep != std::wstring::npos) {
                        NoteItem item;
                        item.completed = (line[0] == L'1');
                        item.title = line.substr(2, sep - 2);
                        item.content = line.substr(sep + 1);
                        m_notes.push_back(item);
                    }
                }
            }
        }
    }

    void NotesWidget::SaveNotes() {
        PWSTR appDataPath = nullptr;
        if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &appDataPath)) && appDataPath) {
            std::wstring dir = std::wstring(appDataPath) + L"\\XEWidgets";
            CreateDirectoryW(dir.c_str(), nullptr);
            std::wstring path = dir + L"\\notes.txt";
            CoTaskMemFree(appDataPath);
            std::wofstream f(path);
            if (f.is_open()) {
                for (const auto& n : m_notes) {
                    f << (n.completed ? L"1|" : L"0|") << n.title << L"|" << n.content << L"\n";
                }
            }
        }
    }

    void NotesWidget::InitTypography() {
        if (m_typographyInitialized || !m_renderer) return;

        m_formatHeader = m_renderer->CreateTextFormat(
            L"Geist Mono", 11.0f, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER
        );

        m_formatTitle = m_renderer->CreateTextFormat(
            L"Geist Sans", 11.5f, DWRITE_FONT_WEIGHT_MEDIUM, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER
        );

        m_formatContent = m_renderer->CreateTextFormat(
            L"Geist Sans", 10.0f, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_NEAR
        );

        m_formatMono = m_renderer->CreateTextFormat(
            L"Geist Mono", 9.0f, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER
        );

        m_formatButton = m_renderer->CreateTextFormat(
            L"Geist Mono", 9.0f, DWRITE_FONT_WEIGHT_MEDIUM, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER
        );

        m_typographyInitialized = true;
    }

    void NotesWidget::OnTick(UINT_PTR timerId) {
        if (timerId == m_timerId && m_hwnd) {
            InvalidateRect(m_hwnd, nullptr, FALSE);
        }
    }

    LRESULT NotesWidget::HandleWidgetMessage(UINT msg, WPARAM wParam, LPARAM lParam) {
        if (msg == WM_CREATE || (msg == WM_SHOWWINDOW && wParam == TRUE)) {
            if (m_hwnd) {
                SetTimer(m_hwnd, m_timerId, 5000, nullptr);
            }
        }
        return WidgetWindow::HandleWidgetMessage(msg, wParam, lParam);
    }

    bool NotesWidget::IsInInteractiveArea(int clientX, int clientY) const {
        POINT pt{ clientX, clientY };
        if (PtInRect(&m_btnNewRect, pt)) return true;
        for (const auto& r : m_checkRects) {
            if (PtInRect(&r, pt)) return true;
        }
        for (const auto& r : m_deleteRects) {
            if (PtInRect(&r, pt)) return true;
        }
        return false;
    }

    void NotesWidget::OnLButtonDown(int x, int y) {
        POINT pt{ x, y };
        if (PtInRect(&m_btnNewRect, pt)) {
            NoteItem newItem;
            newItem.id = std::to_wstring(m_notes.size() + 1);
            newItem.title = L"Quick Note #" + std::to_wstring(m_notes.size() + 1);
            newItem.content = L"Tap to mark complete or review task item.";
            newItem.completed = false;
            newItem.isPinned = false;
            m_notes.insert(m_notes.begin(), newItem);
            SaveNotes();
            if (m_hwnd) InvalidateRect(m_hwnd, nullptr, FALSE);
            return;
        }

        for (size_t i = 0; i < m_checkRects.size(); ++i) {
            if (PtInRect(&m_checkRects[i], pt) && i < m_notes.size()) {
                m_notes[i].completed = !m_notes[i].completed;
                SaveNotes();
                if (m_hwnd) InvalidateRect(m_hwnd, nullptr, FALSE);
                return;
            }
        }

        for (size_t i = 0; i < m_deleteRects.size(); ++i) {
            if (PtInRect(&m_deleteRects[i], pt) && i < m_notes.size()) {
                m_notes.erase(m_notes.begin() + i);
                SaveNotes();
                if (m_hwnd) InvalidateRect(m_hwnd, nullptr, FALSE);
                return;
            }
        }
    }

    void NotesWidget::OnRender() {
        if (!m_renderer) return;
        InitTypography();
        m_checkRects.clear();
        m_deleteRects.clear();

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

        // StickyNote icon
        float iconX = margin + 14.0f;
        float iconY = headerY + 8.0f;
        m_renderer->DrawRectangle(D2D1::RectF(iconX, iconY, iconX + 10.0f, iconY + 11.0f), Colors::MutedLight, 1.2f);
        m_renderer->DrawLine(D2D1::Point2F(iconX + 6.0f, iconY + 7.0f), D2D1::Point2F(iconX + 10.0f, iconY + 11.0f), Colors::MutedLight, 1.0f);

        D2D1_RECT_F headerLabelR = D2D1::RectF(margin + 28.0f, headerY, width - 80.0f, headerY + headerH);
        m_renderer->DrawText(L"SCRATCHPAD", headerLabelR, Colors::FaintLight, m_formatHeader.Get());

        // + NEW button
        float btnW = 38.0f;
        float btnH = 17.0f;
        float btnX = width - margin - btnW - 8.0f;
        float btnY = headerY + 5.0f;
        m_btnNewRect = {
            static_cast<LONG>(btnX),
            static_cast<LONG>(btnY),
            static_cast<LONG>(btnX + btnW),
            static_cast<LONG>(btnY + btnH)
        };
        D2D1_ROUNDED_RECT newBtnR{ D2D1::RectF(btnX, btnY, btnX + btnW, btnY + btnH), 2.0f, 2.0f };
        m_renderer->DrawRoundedRectangle(newBtnR, Colors::BorderLight, 1.0f);
        m_renderer->DrawText(L"+ NEW", D2D1::RectF(btnX, btnY, btnX + btnW, btnY + btnH), Colors::MutedLight, m_formatButton.Get());

        // Header divider
        m_renderer->DrawLine(
            D2D1::Point2F(margin, margin + headerH),
            D2D1::Point2F(width - margin, margin + headerH),
            Colors::BorderLight, 1.0f
        );

        // 4. Notes list
        float curY = margin + headerH + 6.0f;
        float contentW = width - (margin * 2.0f) - 16.0f;
        float leftX = margin + 8.0f;

        size_t maxNotes = std::min<size_t>(m_notes.size(), 3);
        for (size_t i = 0; i < maxNotes; ++i) {
            float boxH = 34.0f;
            D2D1_ROUNDED_RECT noteBox{ D2D1::RectF(leftX, curY, leftX + contentW, curY + boxH), 3.0f, 3.0f };
            m_renderer->FillRoundedRectangle(noteBox, Colors::BgLight);
            m_renderer->DrawRoundedRectangle(noteBox, Colors::BorderLight, 1.0f);

            // Checkbox
            float chkX = leftX + 8.0f;
            float chkY = curY + 11.0f;
            RECT chkRc = { static_cast<LONG>(chkX - 2.0f), static_cast<LONG>(chkY - 2.0f), static_cast<LONG>(chkX + 14.0f), static_cast<LONG>(chkY + 14.0f) };
            m_checkRects.push_back(chkRc);

            if (m_notes[i].completed) {
                m_renderer->FillEllipse(D2D1::Ellipse(D2D1::Point2F(chkX + 5.0f, chkY + 5.0f), 5.0f, 5.0f), Colors::FgLight);
            } else {
                m_renderer->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(chkX + 5.0f, chkY + 5.0f), 5.0f, 5.0f), Colors::BorderLight, 1.2f);
            }

            // Title
            D2D1_RECT_F tR = D2D1::RectF(leftX + 24.0f, curY + 3.0f, leftX + contentW - 24.0f, curY + 18.0f);
            m_renderer->DrawText(m_notes[i].title, tR, m_notes[i].completed ? Colors::FaintLight : Colors::FgLight, m_formatTitle.Get());

            // Content
            D2D1_RECT_F cR = D2D1::RectF(leftX + 24.0f, curY + 18.0f, leftX + contentW - 24.0f, curY + 32.0f);
            m_renderer->DrawText(m_notes[i].content, cR, Colors::MutedLight, m_formatContent.Get());

            // Delete trash icon on right
            float delX = leftX + contentW - 18.0f;
            float delY = curY + 10.0f;
            RECT delRc = { static_cast<LONG>(delX - 2.0f), static_cast<LONG>(delY - 2.0f), static_cast<LONG>(delX + 16.0f), static_cast<LONG>(delY + 16.0f) };
            m_deleteRects.push_back(delRc);
            m_renderer->DrawLine(D2D1::Point2F(delX, delY), D2D1::Point2F(delX + 10.0f, delY + 10.0f), Colors::FaintLight, 1.0f);
            m_renderer->DrawLine(D2D1::Point2F(delX + 10.0f, delY), D2D1::Point2F(delX, delY + 10.0f), Colors::FaintLight, 1.0f);

            curY += boxH + 5.0f;
        }

        // 5. Footer
        float footerY = height - margin - 20.0f;
        m_renderer->DrawLine(
            D2D1::Point2F(margin, footerY),
            D2D1::Point2F(width - margin, footerY),
            Colors::BorderLight, 1.0f
        );

        std::wstring noteCountStr = std::to_wstring(m_notes.size()) + L" NOTES";
        D2D1_RECT_F footL = D2D1::RectF(margin + 12.0f, footerY, width - 100.0f, height - margin);
        m_renderer->DrawText(noteCountStr, footL, Colors::FaintLight, m_formatMono.Get());

        ComPtr<IDWriteTextFormat> fmtRight;
        fmtRight = m_renderer->CreateTextFormat(
            L"Geist Mono", 9.0f, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_TEXT_ALIGNMENT_TRAILING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER
        );
        D2D1_RECT_F footR = D2D1::RectF(width - margin - 90.0f, footerY, width - margin - 12.0f, height - margin);
        if (fmtRight) {
            m_renderer->DrawText(L"AUTO-SAVED", footR, Colors::FaintLight, fmtRight.Get());
        }

        DrawHoverToolbar(width);
    }

}
