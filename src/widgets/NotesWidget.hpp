#pragma once

#include "Common.hpp"
#include "../windows/WidgetWindow.hpp"
#include <vector>

namespace xe {

    struct NoteItem {
        std::wstring id;
        std::wstring title;
        std::wstring content;
        bool completed = false;
        bool isPinned = false;
    };

    class NotesWidget : public WidgetWindow {
    public:
        NotesWidget();
        ~NotesWidget() override;

        void OnTick(UINT_PTR timerId) override;

    protected:
        void OnRender() override;
        void OnLButtonDown(int x, int y) override;
        bool IsInInteractiveArea(int clientX, int clientY) const override;
        LRESULT HandleWidgetMessage(UINT msg, WPARAM wParam, LPARAM lParam) override;

    private:
        void InitTypography();
        void LoadNotes();
        void SaveNotes();

        std::vector<NoteItem> m_notes;
        bool m_isAdding = false;
        UINT_PTR m_timerId = 5;

        RECT m_btnNewRect{};
        std::vector<RECT> m_checkRects;
        std::vector<RECT> m_deleteRects;

        ComPtr<IDWriteTextFormat> m_formatHeader;
        ComPtr<IDWriteTextFormat> m_formatTitle;
        ComPtr<IDWriteTextFormat> m_formatContent;
        ComPtr<IDWriteTextFormat> m_formatMono;
        ComPtr<IDWriteTextFormat> m_formatButton;
        bool m_typographyInitialized = false;
    };

}
