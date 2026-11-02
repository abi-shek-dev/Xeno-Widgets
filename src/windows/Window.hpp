#pragma once

#include "Common.hpp"
#include "../graphics/D2DRenderer.hpp"
#include <functional>
#include <vector>

namespace xe {

    struct WindowConfig {
        std::wstring title = L"XE Widgets — Workspace Manager";
        int width = 1080;
        int height = 700;
        bool resizable = true;
        bool frameless = false;
        bool alwaysOnTop = false;
        bool centerOnScreen = true;
    };

    enum class ManagerTab {
        Overview = 0,
        Library = 1,
        ActiveWidgets = 2,
        Settings = 3
    };

    struct ClickableArea {
        D2D1_RECT_F rect{};
        std::function<void()> onClick;
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

        void InitTypography();
        void RenderSidebar(float width, float height);
        void RenderTopHeader(float leftX, float width);
        void RenderOverviewTab(float leftX, float topY, float width, float height);
        void RenderLibraryTab(float leftX, float topY, float width, float height);
        void RenderActiveWidgetsTab(float leftX, float topY, float width, float height);
        void RenderSettingsTab(float leftX, float topY, float width, float height);

        void OnLButtonDown(int x, int y);
        void OnMouseMove(int x, int y);

        HWND m_hwnd = nullptr;
        D2DRenderer* m_renderer = nullptr;
        WindowConfig m_config;

        ManagerTab m_activeTab = ManagerTab::Overview;
        std::wstring m_libraryFilter = L"All";
        std::vector<ClickableArea> m_clickableAreas;
        POINT m_mousePos{ -1, -1 };
        bool m_alwaysOnTopSetting = false;
        bool m_isDarkMode = false;

        void ToggleTheme();

        // DirectWrite typography formats
        ComPtr<IDWriteTextFormat> m_formatHeroTitle;
        ComPtr<IDWriteTextFormat> m_formatSectionTitle;
        ComPtr<IDWriteTextFormat> m_formatPageTitle;
        ComPtr<IDWriteTextFormat> m_formatSubtitle;
        ComPtr<IDWriteTextFormat> m_formatBody;
        ComPtr<IDWriteTextFormat> m_formatMono;
        ComPtr<IDWriteTextFormat> m_formatMonoMedium;
        ComPtr<IDWriteTextFormat> m_formatStatNum;
        ComPtr<IDWriteTextFormat> m_formatStatLabel;
        ComPtr<IDWriteTextFormat> m_formatCardTitle;
        ComPtr<IDWriteTextFormat> m_formatCardDesc;
        ComPtr<IDWriteTextFormat> m_formatCardTag;
        ComPtr<IDWriteTextFormat> m_formatButton;
        ComPtr<IDWriteTextFormat> m_formatButtonCenter;
        ComPtr<IDWriteTextFormat> m_formatLogo;
        bool m_typographyInitialized = false;

        static inline bool s_classRegistered = false;
        static inline const wchar_t* s_className = L"XEWidgets_Native_Class";
    };

}
