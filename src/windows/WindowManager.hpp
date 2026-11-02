#pragma once

#include "Common.hpp"
#include "WidgetWindow.hpp"
#include <memory>
#include <vector>

namespace xe {

    class WindowManager {
    public:
        static WindowManager& Instance();

        bool Initialize(HINSTANCE hInstance, D2DRenderer* renderer);
        void Shutdown();

        WidgetWindow* CreateWidget(std::unique_ptr<WidgetWindow> widget, const WidgetWindow::Config& config);
        void RemoveWidget(HWND hwnd);

        HINSTANCE GetHInstance() const { return m_hInstance; }
        D2DRenderer* GetRenderer() const { return m_renderer; }

    private:
        WindowManager() = default;
        ~WindowManager() = default;
        WindowManager(const WindowManager&) = delete;
        WindowManager& operator=(const WindowManager&) = delete;

        HINSTANCE m_hInstance = nullptr;
        D2DRenderer* m_renderer = nullptr;
        std::vector<std::unique_ptr<WidgetWindow>> m_widgets;
    };

}
