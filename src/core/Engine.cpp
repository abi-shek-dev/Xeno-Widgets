#include "Engine.hpp"
#include "Logger.hpp"
#include "../data/SMTCProvider.hpp"
#include "../data/TelemetryProvider.hpp"
#include "../manager/TrayManager.hpp"
#include "../windows/WindowManager.hpp"
#include "../widgets/ClockWidget.hpp"
#include "../widgets/MusicWidget.hpp"
#include "../widgets/SystemMonitorWidget.hpp"
#include "../widgets/WeatherWidget.hpp"
#include "../widgets/NotesWidget.hpp"
#include "../widgets/CalendarWidget.hpp"
#include <shlobj.h>

namespace xe {

    static std::string WideToUtf8(const std::wstring& wstr) {
        if (wstr.empty()) return {};
        int size = WideCharToMultiByte(CP_UTF8, 0, wstr.data(), static_cast<int>(wstr.size()), nullptr, 0, nullptr, nullptr);
        std::string result(size, 0);
        WideCharToMultiByte(CP_UTF8, 0, wstr.data(), static_cast<int>(wstr.size()), result.data(), size, nullptr, nullptr);
        return result;
    }

    static std::wstring Utf8ToWide(const std::string& str) {
        if (str.empty()) return {};
        int size = MultiByteToWideChar(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), nullptr, 0);
        std::wstring result(size, 0);
        MultiByteToWideChar(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), result.data(), size);
        return result;
    }

    Engine::Engine() = default;

    Engine::~Engine() {
        Shutdown();
    }

    std::wstring Engine::GetConfigFilePath() const {
        PWSTR appDataPath = nullptr;
        HRESULT hr = SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &appDataPath);
        if (SUCCEEDED(hr) && appDataPath) {
            std::wstring dir = std::wstring(appDataPath) + L"\\XEWidgets";
            CoTaskMemFree(appDataPath);
            CreateDirectoryW(dir.c_str(), nullptr); // OK if already exists
            return dir + L"\\widgets.json";
        }
        return L"widgets.json";
    }

    void Engine::SaveConfig() {
        for (auto& w : m_widgetConfig.GetWidgetsMutable()) {
            w.visible = false;
        }
        for (const auto& widget : WindowManager::Instance().GetWidgets()) {
            if (!widget || !widget->IsOpen()) continue;
            HWND hwnd = widget->GetHwnd();
            if (hwnd && IsWindow(hwnd)) {
                RECT r{};
                GetWindowRect(hwnd, &r);
                widget->GetConfig().x = r.left;
                widget->GetConfig().y = r.top;
            }
            const auto& cfg = widget->GetConfig();
            WidgetEntry entry;
            entry.id = WideToUtf8(cfg.id);
            entry.x = cfg.x;
            entry.y = cfg.y;
            entry.width = cfg.width;
            entry.height = cfg.height;
            entry.opacity = cfg.opacity;
            entry.alwaysOnTop = cfg.alwaysOnTop;
            entry.clickThrough = cfg.clickThrough;
            entry.locked = cfg.locked;
            entry.visible = true;
            m_widgetConfig.UpdateWidget(entry);
        }
        m_widgetConfig.SaveToFile(m_configPath);
        XE_LOG_INFO("Saved widget configuration to disk");
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

        XE_LOG_INFO("Creating XE Workspace Manager Window...");
        m_mainWindow = std::make_unique<Window>();
        WindowConfig config{};
        config.title = L"XE Widgets — Workspace Manager";
        config.width = 1080;
        config.height = 700;
        config.resizable = true;
        config.frameless = false;
        config.alwaysOnTop = false;
        config.centerOnScreen = true;

        if (!m_mainWindow->Create(config, m_renderer.get())) {
            XE_LOG_ERROR("Failed to create main manager window");
            return false;
        }

        m_mainWindow->Show(SW_SHOWNORMAL);
        XE_LOG_INFO("XE Native Engine initialized successfully");

        TrayManager::Instance().Initialize(m_mainWindow->GetHwnd(), m_hInstance);

        // Initialize WindowManager and launch desktop widgets
        if (!WindowManager::Instance().Initialize(m_hInstance, m_renderer.get())) {
            XE_LOG_ERROR("Failed to initialize WindowManager");
            return false;
        }

        // Initialize Windows System Media Transport Controls (SMTC) integration
        SMTCProvider::Instance().Initialize();

        // Initialize Windows System Telemetry Provider
        TelemetryProvider::Instance().Initialize();

        // Load widget configurations
        m_configPath = GetConfigFilePath();
        bool configLoaded = m_widgetConfig.LoadFromFile(m_configPath);
        if (!configLoaded || m_widgetConfig.GetWidgets().empty()) {
            XE_LOG_INFO("Widget config not found or invalid; generating default config...");
            std::vector<WidgetEntry> defaults = {
                { "clock",   40,  60, 340, 180, 1.0f, false, false, false, true },
                { "monitor", 400, 60, 340, 180, 1.0f, false, false, false, true },
                { "music",   40,  260, 340, 180, 1.0f, false, false, false, true },
                { "weather", 400, 260, 340, 180, 1.0f, false, false, false, true },
                { "scratch", 760, 60, 340, 180, 1.0f, false, false, false, false },
                { "calendar", 760, 260, 340, 180, 1.0f, false, false, false, false }
            };
            m_widgetConfig.SetWidgets(defaults);
            m_widgetConfig.SaveToFile(m_configPath);
        }

        auto GetConfigFor = [&](const std::string& id, const std::wstring& title, int defX, int defY, int defW, int defH) -> WidgetWindow::Config {
            WidgetWindow::Config cfg;
            cfg.id = Utf8ToWide(id);
            cfg.title = title;
            auto opt = m_widgetConfig.FindWidget(id);
            if (opt) {
                cfg.x = opt->x;
                cfg.y = opt->y;
                cfg.width = opt->width;
                cfg.height = opt->height;
                cfg.opacity = opt->opacity;
                cfg.alwaysOnTop = opt->alwaysOnTop;
                cfg.clickThrough = opt->clickThrough;
                cfg.locked = opt->locked;
                cfg.visible = opt->visible;
            } else {
                cfg.x = defX;
                cfg.y = defY;
                cfg.width = defW;
                cfg.height = defH;
                cfg.visible = true;
            }
            return cfg;
        };

        // 1. Precision Clock widget
        WidgetWindow::Config clockConfig = GetConfigFor("clock", L"XE Precision Clock", 40, 60, 340, 180);
        if (clockConfig.visible) {
            WindowManager::Instance().CreateWidget(std::make_unique<ClockWidget>(), clockConfig);
        }

        // 2. System Telemetry Monitor widget
        WidgetWindow::Config monitorConfig = GetConfigFor("monitor", L"XE System Telemetry", 400, 60, 340, 180);
        if (monitorConfig.visible) {
            WindowManager::Instance().CreateWidget(std::make_unique<SystemMonitorWidget>(), monitorConfig);
        }

        // 3. Media Player widget
        WidgetWindow::Config musicConfig = GetConfigFor("music", L"XE Media Player", 40, 260, 340, 180);
        if (musicConfig.visible) {
            WindowManager::Instance().CreateWidget(std::make_unique<MusicWidget>(), musicConfig);
        }

        // 4. Atmosphere (Weather) widget
        WidgetWindow::Config weatherConfig = GetConfigFor("weather", L"XE Atmosphere", 400, 260, 340, 180);
        if (weatherConfig.visible) {
            WindowManager::Instance().CreateWidget(std::make_unique<WeatherWidget>(), weatherConfig);
        }

        // 5. Quick Scratchpad widget
        WidgetWindow::Config scratchConfig = GetConfigFor("scratch", L"XE Quick Scratchpad", 760, 60, 340, 180);
        if (scratchConfig.visible) {
            WindowManager::Instance().CreateWidget(std::make_unique<NotesWidget>(), scratchConfig);
        }

        // 6. Calendar widget
        WidgetWindow::Config calConfig = GetConfigFor("calendar", L"XE Event Horizon Calendar", 760, 260, 340, 180);
        if (calConfig.visible) {
            WindowManager::Instance().CreateWidget(std::make_unique<CalendarWidget>(), calConfig);
        }

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
        SaveConfig();

        TelemetryProvider::Instance().Shutdown();

        SMTCProvider::Instance().Shutdown();

        TrayManager::Instance().Shutdown();

        WindowManager::Instance().Shutdown();

        if (m_mainWindow) {
            m_mainWindow->Close();
            m_mainWindow.reset();
        }

        if (m_renderer) {
            m_renderer->DiscardDeviceResources();
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
