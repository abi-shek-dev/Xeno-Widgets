#pragma once

#include "Common.hpp"
#include <string>
#include <vector>
#include <optional>

namespace xe {

    struct WidgetEntry {
        std::string id;
        int x = 100, y = 100;
        int width = 320, height = 200;
        float opacity = 1.0f;
        bool alwaysOnTop = false;
        bool clickThrough = false;
        bool locked = false;
        bool visible = true;
    };

    class WidgetConfig {
    public:
        bool LoadFromFile(const std::string& path);
        bool LoadFromFile(const std::wstring& path);
        bool SaveToFile(const std::string& path) const;
        bool SaveToFile(const std::wstring& path) const;

        const std::vector<WidgetEntry>& GetWidgets() const { return m_widgets; }
        std::vector<WidgetEntry>& GetWidgetsMutable() { return m_widgets; }
        void SetWidgets(const std::vector<WidgetEntry>& w) { m_widgets = w; }
        void UpdateWidget(const WidgetEntry& entry); // upsert by id
        std::optional<WidgetEntry> FindWidget(const std::string& id) const;

    private:
        std::string SerializeToJson() const;
        bool ParseJson(const std::string& json);

        std::vector<WidgetEntry> m_widgets;
    };

}
