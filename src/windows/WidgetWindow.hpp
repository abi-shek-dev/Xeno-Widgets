#pragma once

#include "Common.hpp"
#include "../graphics/D2DRenderer.hpp"

namespace xe {

    class WidgetWindow {
    public:
        struct Config {
            std::wstring id = L"unknown";
            std::wstring title = L"Widget";
            int x = CW_USEDEFAULT;
            int y = CW_USEDEFAULT;
            int width = 300;
            int height = 200;
            float opacity = 1.0f;
            bool alwaysOnTop = false;
            bool clickThrough = false;
            bool locked = false;
            bool visible = true;
        };

        WidgetWindow();
        virtual ~WidgetWindow();

        bool Create(const Config& config, D2DRenderer* renderer);
        void Show();
        void Hide();
        void Close();
        void Destroy();
        HWND GetHwnd() const { return m_hwnd; }
        bool IsOpen() const { return m_hwnd != nullptr && IsWindow(m_hwnd); }
        const Config& GetConfig() const { return m_config; }
        Config& GetConfig() { return m_config; }
        const std::wstring& GetId() const { return m_config.id; }

        void SetAlwaysOnTop(bool value);
        void SetOpacity(float opacity);   // 0.0 to 1.0 using SetLayeredWindowAttributes
        void SetClickThrough(bool value); // toggle WS_EX_TRANSPARENT
        void SetLocked(bool locked);      // toggle m_locked; affects WM_NCHITTEST

        // Called by WindowManager or WM_TIMER at timer tick
        virtual void OnTick(UINT_PTR timerId) = 0;

    protected:
        virtual void OnRender() = 0;
        virtual void OnLButtonDown(int /*x*/, int /*y*/) {}
        virtual void OnLButtonUp(int /*x*/, int /*y*/) {}
        virtual void OnMouseMove(int /*x*/, int /*y*/) {}
        virtual void OnMouseLeave() {}
        virtual bool IsInInteractiveArea(int /*clientX*/, int /*clientY*/) const {
            return false;
        }
        virtual LRESULT HandleWidgetMessage(UINT /*msg*/, WPARAM /*wParam*/, LPARAM /*lParam*/) {
            return 0;
        }

        void DrawHoverToolbar(float widgetWidth);

        D2DRenderer* m_renderer = nullptr;
        HWND m_hwnd = nullptr;
        bool m_locked = false;
        bool m_hoverToolbarVisible = false;

    private:
        static LRESULT CALLBACK StaticWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
        LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
        bool RegisterClass();
        void ShowContextMenu(int screenX, int screenY);

        Config m_config;
        bool m_mouseTracking = false;

        RECT m_toolbarLockRect{};
        RECT m_toolbarCloseRect{};
        ComPtr<IDWriteTextFormat> m_formatToolbar;

        static inline bool s_classRegistered = false;
        static inline const wchar_t* s_className = L"XEWidgets_Widget_Class";
    };

}
