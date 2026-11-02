#include "Engine.hpp"
#include "Logger.hpp"

namespace xe {

    Engine::Engine() = default;

    Engine::~Engine() {
        Shutdown();
    }

    bool Engine::Initialize(HINSTANCE hInstance) {
        m_hInstance = hInstance;

        // Initialize COM for Direct2D, WIC, and DirectWrite
        HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
        if (FAILED(hr)) {
            XE_LOG_ERROR("Failed to initialize COM apartment");
            return false;
        }
        m_comInitialized = true;

        // Configure Per-Monitor DPI Awareness v2
        if (!SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2)) {
            XE_LOG_WARN("Per-Monitor V2 DPI Awareness unavailable, falling back to Per-Monitor V1");
            SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE);
        }

        XE_LOG_INFO("Initializing XE Core Graphics Engine...");
        m_renderer = std::make_unique<D2DRenderer>();
        if (!m_renderer->Initialize()) {
            XE_LOG_ERROR("Direct2D / DirectWrite initialization failed");
            return false;
        }

        XE_LOG_INFO("Creating Phase 1 Foundation Window...");
        m_mainWindow = std::make_unique<Window>();
        WindowConfig config{};
        config.title = L"XE Widgets — Native Desktop Engine [Phase 1]";
        config.width = 780;
        config.height = 490;
        config.resizable = true;
        config.frameless = false;
        config.alwaysOnTop = false;
        config.centerOnScreen = true;

        if (!m_mainWindow->Create(config, m_renderer.get())) {
            XE_LOG_ERROR("Failed to create main foundation window");
            return false;
        }

        m_mainWindow->Show(SW_SHOWNORMAL);
        XE_LOG_INFO("XE Native Engine initialized successfully");
        return true;
    }

    int Engine::Run() {
        XE_LOG_INFO("Entering native Win32 message loop...");
        MSG msg{};
        while (GetMessageW(&msg, nullptr, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }

        XE_LOG_INFO("Message loop exited cleanly");
        return static_cast<int>(msg.wParam);
    }

    void Engine::Shutdown() {
        if (m_mainWindow) {
            m_mainWindow->Close();
            m_mainWindow.reset();
        }

        if (m_renderer) {
            m_renderer->Shutdown();
            m_renderer.reset();
        }

        if (m_comInitialized) {
            CoUninitialize();
            m_comInitialized = false;
        }

        XE_LOG_INFO("XE Native Engine shutdown complete");
    }

}
