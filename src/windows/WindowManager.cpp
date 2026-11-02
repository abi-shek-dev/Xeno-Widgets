#include "WindowManager.hpp"
#include "../core/Logger.hpp"
#include "../widgets/ClockWidget.hpp"
#include "../widgets/MusicWidget.hpp"
#include "../widgets/SystemMonitorWidget.hpp"
#include "../widgets/WeatherWidget.hpp"
#include "../widgets/NotesWidget.hpp"
#include "../widgets/CalendarWidget.hpp"
#include <algorithm>

namespace xe {

    WindowManager& WindowManager::Instance() {
        static WindowManager s_instance;
        return s_instance;
    }

    bool WindowManager::Initialize(HINSTANCE hInstance, D2DRenderer* renderer) {
        m_hInstance = hInstance;
        m_renderer = renderer;

        if (!m_renderer) {
            XE_LOG_ERROR("WindowManager initialized with null D2DRenderer");
            return false;
        }

        XE_LOG_INFO("WindowManager initialized successfully");
        return true;
    }

    void WindowManager::Shutdown() {
        XE_LOG_INFO("Shutting down WindowManager and closing widgets...");
        for (auto& widget : m_widgets) {
            if (widget) {
                widget->Close();
            }
        }
        m_widgets.clear();
        m_renderer = nullptr;
        m_hInstance = nullptr;
    }

    void WindowManager::ShowAllWidgets() {
        for (auto& widget : m_widgets) {
            if (widget && widget->IsOpen()) {
                widget->Show();
            }
        }
    }

    void WindowManager::HideAllWidgets() {
        for (auto& widget : m_widgets) {
            if (widget && widget->IsOpen()) {
                widget->Hide();
            }
        }
    }

    void WindowManager::DisableAllClickThrough() {
        for (auto& widget : m_widgets) {
            if (widget && widget->IsOpen()) {
                widget->SetClickThrough(false);
            }
        }
        XE_LOG_INFO("Disabled click-through for all active widgets");
    }

    WidgetWindow* WindowManager::CreateWidget(std::unique_ptr<WidgetWindow> widget, const WidgetWindow::Config& config) {
        if (!widget || !m_renderer) {
            XE_LOG_ERROR("Cannot create widget: invalid widget pointer or renderer");
            return nullptr;
        }

        if (!widget->Create(config, m_renderer)) {
            XE_LOG_ERROR("Failed to create WidgetWindow instance");
            return nullptr;
        }

        widget->Show();
        WidgetWindow* ptr = widget.get();
        m_widgets.push_back(std::move(widget));
        XE_LOG_INFO("Widget added to WindowManager");
        return ptr;
    }

    void WindowManager::RemoveWidget(HWND hwnd) {
        auto it = std::remove_if(m_widgets.begin(), m_widgets.end(), [hwnd](const std::unique_ptr<WidgetWindow>& w) {
            return w && (w->GetHwnd() == hwnd || !w->IsOpen());
        });
        if (it != m_widgets.end()) {
            m_widgets.erase(it, m_widgets.end());
            XE_LOG_INFO("Widget removed from WindowManager");
        }
    }

    void WindowManager::CleanupDeadWidgets() {
        auto it = std::remove_if(m_widgets.begin(), m_widgets.end(), [](const std::unique_ptr<WidgetWindow>& w) {
            return !w || !w->IsOpen();
        });
        if (it != m_widgets.end()) {
            m_widgets.erase(it, m_widgets.end());
        }
    }

    void WindowManager::UpdateInputState() {
        CleanupDeadWidgets();

        // Rainmeter-style Ctrl key modifier:
        // When user holds Ctrl, temporarily bypass WS_EX_TRANSPARENT on click-through widgets
        // so they can be clicked, hovered, closed, or dragged with the mouse!
        bool ctrlPressed = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
        for (auto& widget : m_widgets) {
            if (!widget || !widget->IsOpen()) continue;
            if (widget->GetConfig().clickThrough) {
                HWND hwnd = widget->GetHwnd();
                if (hwnd && IsWindow(hwnd)) {
                    LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
                    bool hasTransparent = (exStyle & WS_EX_TRANSPARENT) != 0;
                    if (ctrlPressed && hasTransparent) {
                        SetWindowLongPtrW(hwnd, GWL_EXSTYLE, exStyle & ~WS_EX_TRANSPARENT);
                    } else if (!ctrlPressed && !hasTransparent) {
                        SetWindowLongPtrW(hwnd, GWL_EXSTYLE, exStyle | WS_EX_TRANSPARENT);
                    }
                }
            }
        }
    }

    bool WindowManager::IsWidgetRunning(const std::wstring& id) const {
        for (const auto& w : m_widgets) {
            if (w && w->GetId() == id && w->IsOpen()) {
                return true;
            }
        }
        return false;
    }

    WidgetWindow* WindowManager::FindWidgetById(const std::wstring& id) {
        for (auto& w : m_widgets) {
            if (w && w->GetId() == id && w->IsOpen()) {
                return w.get();
            }
        }
        return nullptr;
    }

    void WindowManager::CloseWidgetById(const std::wstring& id) {
        for (auto& w : m_widgets) {
            if (w && w->GetId() == id && w->IsOpen()) {
                w->Close();
                break;
            }
        }
    }

    void WindowManager::OpenWidgetById(const std::wstring& id) {
        if (IsWidgetRunning(id)) return;

        WidgetWindow::Config cfg;
        cfg.id = id;
        cfg.opacity = 1.0f;
        cfg.alwaysOnTop = false;
        cfg.clickThrough = false;
        cfg.locked = false;
        cfg.visible = true;

        if (id == L"clock") {
            cfg.title = L"XE Precision Clock";
            cfg.x = 40; cfg.y = 60;
            cfg.width = 340; cfg.height = 180;
            CreateWidget(std::make_unique<ClockWidget>(), cfg);
        } else if (id == L"music") {
            cfg.title = L"XE Media Player";
            cfg.x = 40; cfg.y = 260;
            cfg.width = 340; cfg.height = 180;
            CreateWidget(std::make_unique<MusicWidget>(), cfg);
        } else if (id == L"monitor") {
            cfg.title = L"XE System Telemetry";
            cfg.x = 400; cfg.y = 60;
            cfg.width = 340; cfg.height = 180;
            CreateWidget(std::make_unique<SystemMonitorWidget>(), cfg);
        } else if (id == L"weather") {
            cfg.title = L"XE Atmosphere";
            cfg.x = 400; cfg.y = 260;
            cfg.width = 340; cfg.height = 180;
            CreateWidget(std::make_unique<WeatherWidget>(), cfg);
        } else if (id == L"notes" || id == L"scratch") {
            cfg.title = L"XE Quick Scratchpad";
            cfg.x = 760; cfg.y = 60;
            cfg.width = 340; cfg.height = 180;
            CreateWidget(std::make_unique<NotesWidget>(), cfg);
        } else if (id == L"calendar") {
            cfg.title = L"XE Event Horizon Calendar";
            cfg.x = 760; cfg.y = 260;
            cfg.width = 340; cfg.height = 180;
            CreateWidget(std::make_unique<CalendarWidget>(), cfg);
        }
    }

    void WindowManager::ToggleWidgetById(const std::wstring& id) {
        if (IsWidgetRunning(id)) {
            CloseWidgetById(id);
        } else {
            OpenWidgetById(id);
        }
    }

}
