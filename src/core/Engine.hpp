#pragma once

#include "Common.hpp"
#include "WidgetConfig.hpp"
#include "../graphics/D2DRenderer.hpp"
#include "../windows/Window.hpp"

namespace xe {

    class WindowManager;

    class Engine {
    public:
        Engine();
        ~Engine();

        bool Initialize(HINSTANCE hInstance);
        int Run();
        void Shutdown();
        void SaveConfig();

    private:
        std::wstring GetConfigFilePath() const;

        HINSTANCE m_hInstance = nullptr;
        bool m_comInitialized = false;

        std::unique_ptr<D2DRenderer> m_renderer;
        std::unique_ptr<Window> m_mainWindow;

        WidgetConfig m_widgetConfig;
        std::wstring m_configPath;
    };

}
