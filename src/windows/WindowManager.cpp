#include "WindowManager.hpp"
#include "../core/Logger.hpp"
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
        // Close all widgets; clear container
        for (auto& widget : m_widgets) {
            if (widget) {
                widget->Close();
            }
        }
        m_widgets.clear();
        m_renderer = nullptr;
        m_hInstance = nullptr;
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
            return w->GetHwnd() == hwnd || w->GetHwnd() == nullptr;
        });
        if (it != m_widgets.end()) {
            m_widgets.erase(it, m_widgets.end());
            XE_LOG_INFO("Widget removed from WindowManager");
        }
    }

}
