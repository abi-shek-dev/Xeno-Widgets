#pragma once

#include "Common.hpp"
#include "../graphics/D2DRenderer.hpp"

namespace xe {

    struct WindowConfig {
        std::wstring title = L"XE Widgets Foundation";
        int width = 720;
        int height = 480;
        bool resizable = true;
        bool frameless = false;
        bool alwaysOnTop = false;
        bool centerOnScreen = true;
    };

    class Window {
    public:
        Window();
        virtual ~Window();

        bool Create(const WindowConfig& config, D2DRenderer* renderer);
        void Show(int nCmdShow = SW_SHOWNORMAL);
        void Close();

        HWND GetHwnd() const { return m_hwnd; }
        bool IsOpen() const { return m_hwnd != nullptr; }

    protected:
        virtual void OnRender();
        virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

    private:
        static LRESULT CALLBACK StaticWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
        static bool RegisterWindowClass();

        HWND m_hwnd = nullptr;
        D2DRenderer* m_renderer = nullptr;
        WindowConfig m_config;

        // Typography formats for Phase 1 UI demo
        ComPtr<IDWriteTextFormat> m_formatTitle;
        ComPtr<IDWriteTextFormat> m_formatSubtitle;
        ComPtr<IDWriteTextFormat> m_formatMono;
        ComPtr<IDWriteTextFormat> m_formatBody;

        static inline bool s_classRegistered = false;
        static inline const wchar_t* s_className = L"XEWidgets_Native_Class";
    };

}
