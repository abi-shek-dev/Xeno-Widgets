#pragma once

#include "Common.hpp"
#include "WidgetWindow.hpp"
#include <memory>
#include <vector>
#include <string>

namespace xe {

    class WindowManager {
    public:
        static WindowManager& Instance();

        bool Initialize(HINSTANCE hInstance, D2DRenderer* renderer);
        void Shutdown();

        void ShowAllWidgets();
        void HideAllWidgets();
        void DisableAllClickThrough();

        WidgetWindow* CreateWidget(std::unique_ptr<WidgetWindow> widget, const WidgetWindow::Config& config);
        void RemoveWidget(HWND hwnd);

        void UpdateInputState();
        void CleanupDeadWidgets();

        bool IsWidgetRunning(const std::wstring& id) const;
        void CloseWidgetById(const std::wstring& id);
        void OpenWidgetById(const std::wstring& id);
        void ToggleWidgetById(const std::wstring& id);
        WidgetWindow* FindWidgetById(const std::wstring& id);

        const std::vector<std::unique_ptr<WidgetWindow>>& GetWidgets() const { return m_widgets; }
        std::vector<std::unique_ptr<WidgetWindow>>& GetWidgetsMutable() { return m_widgets; }

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
