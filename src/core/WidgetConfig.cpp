#include "WidgetConfig.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>

namespace xe {

    bool WidgetConfig::LoadFromFile(const std::string& path) {
        std::ifstream ifs(path);
        if (!ifs.is_open()) return false;
        std::stringstream buffer;
        buffer << ifs.rdbuf();
        return ParseJson(buffer.str());
    }

    bool WidgetConfig::LoadFromFile(const std::wstring& path) {
        std::ifstream ifs(path);
        if (!ifs.is_open()) return false;
        std::stringstream buffer;
        buffer << ifs.rdbuf();
        return ParseJson(buffer.str());
    }

    bool WidgetConfig::SaveToFile(const std::string& path) const {
        std::ofstream ofs(path, std::ios::trunc);
        if (!ofs.is_open()) return false;
        ofs << SerializeToJson();
        return ofs.good();
    }

    bool WidgetConfig::SaveToFile(const std::wstring& path) const {
        std::ofstream ofs(path, std::ios::trunc);
        if (!ofs.is_open()) return false;
        ofs << SerializeToJson();
        return ofs.good();
    }

    void WidgetConfig::UpdateWidget(const WidgetEntry& entry) {
        for (auto& w : m_widgets) {
            if (w.id == entry.id) {
                w = entry;
                return;
            }
        }
        m_widgets.push_back(entry);
    }

    std::optional<WidgetEntry> WidgetConfig::FindWidget(const std::string& id) const {
        for (const auto& w : m_widgets) {
            if (w.id == id) {
                return w;
            }
        }
        return std::nullopt;
    }

    std::string WidgetConfig::SerializeToJson() const {
        std::ostringstream ss;
        ss << "{\n";
        ss << "  \"widgets\": [\n";
        for (size_t i = 0; i < m_widgets.size(); ++i) {
            const auto& w = m_widgets[i];
            ss << "    {\n";
            ss << "      \"id\": \"" << w.id << "\",\n";
            ss << "      \"x\": " << w.x << ",\n";
            ss << "      \"y\": " << w.y << ",\n";
            ss << "      \"width\": " << w.width << ",\n";
            ss << "      \"height\": " << w.height << ",\n";
            ss << "      \"opacity\": " << w.opacity << ",\n";
            ss << "      \"alwaysOnTop\": " << (w.alwaysOnTop ? "true" : "false") << ",\n";
            ss << "      \"clickThrough\": " << (w.clickThrough ? "true" : "false") << ",\n";
            ss << "      \"locked\": " << (w.locked ? "true" : "false") << ",\n";
            ss << "      \"visible\": " << (w.visible ? "true" : "false") << "\n";
            ss << "    }" << (i + 1 < m_widgets.size() ? ",\n" : "\n");
        }
        ss << "  ]\n";
        ss << "}\n";
        return ss.str();
    }

    bool WidgetConfig::ParseJson(const std::string& json) {
        size_t widgetsPos = json.find("\"widgets\"");
        if (widgetsPos == std::string::npos) {
            return false;
        }
        size_t arrayStart = json.find('[', widgetsPos);
        if (arrayStart == std::string::npos) {
            return false;
        }

        std::vector<WidgetEntry> parsedWidgets;
        size_t pos = arrayStart + 1;

        auto Trim = [](const std::string& s) -> std::string {
            size_t first = s.find_first_not_of(" \t\r\n");
            if (first == std::string::npos) return "";
            size_t last = s.find_last_not_of(" \t\r\n");
            return s.substr(first, (last - first + 1));
        };

        auto ExtractKeyVal = [&](const std::string& obj, const std::string& key) -> std::optional<std::string> {
            std::string pattern = "\"" + key + "\"";
            size_t kPos = obj.find(pattern);
            if (kPos == std::string::npos) return std::nullopt;
            size_t colon = obj.find(':', kPos + pattern.size());
            if (colon == std::string::npos) return std::nullopt;

            size_t valStart = obj.find_first_not_of(" \t\r\n", colon + 1);
            if (valStart == std::string::npos) return std::nullopt;

            if (obj[valStart] == '\"') {
                size_t valEnd = obj.find('\"', valStart + 1);
                if (valEnd == std::string::npos) return std::nullopt;
                return obj.substr(valStart + 1, valEnd - valStart - 1);
            } else {
                size_t valEnd = obj.find_first_of(",}\r\n", valStart);
                if (valEnd == std::string::npos) valEnd = obj.size();
                return Trim(obj.substr(valStart, valEnd - valStart));
            }
        };

        while (pos < json.size()) {
            size_t objStart = json.find('{', pos);
            size_t arrayEnd = json.find(']', pos);
            if (objStart == std::string::npos || (arrayEnd != std::string::npos && objStart > arrayEnd)) {
                break;
            }

            int depth = 1;
            size_t objEnd = objStart + 1;
            while (objEnd < json.size() && depth > 0) {
                if (json[objEnd] == '{') depth++;
                else if (json[objEnd] == '}') depth--;
                objEnd++;
            }

            if (depth == 0) {
                std::string objStr = json.substr(objStart, objEnd - objStart);
                WidgetEntry entry;
                if (auto v = ExtractKeyVal(objStr, "id")) entry.id = *v;
                if (auto v = ExtractKeyVal(objStr, "x")) {
                    try { entry.x = std::stoi(*v); } catch (...) {}
                }
                if (auto v = ExtractKeyVal(objStr, "y")) {
                    try { entry.y = std::stoi(*v); } catch (...) {}
                }
                if (auto v = ExtractKeyVal(objStr, "width")) {
                    try { entry.width = std::stoi(*v); } catch (...) {}
                }
                if (auto v = ExtractKeyVal(objStr, "height")) {
                    try { entry.height = std::stoi(*v); } catch (...) {}
                }
                if (auto v = ExtractKeyVal(objStr, "opacity")) {
                    try { entry.opacity = std::stof(*v); } catch (...) {}
                }
                if (auto v = ExtractKeyVal(objStr, "alwaysOnTop")) entry.alwaysOnTop = (*v == "true");
                if (auto v = ExtractKeyVal(objStr, "clickThrough")) entry.clickThrough = (*v == "true");
                if (auto v = ExtractKeyVal(objStr, "locked")) entry.locked = (*v == "true");
                if (auto v = ExtractKeyVal(objStr, "visible")) entry.visible = (*v == "true");

                if (!entry.id.empty()) {
                    parsedWidgets.push_back(entry);
                }
                pos = objEnd;
            } else {
                break;
            }
        }

        if (!parsedWidgets.empty()) {
            m_widgets = std::move(parsedWidgets);
            return true;
        }
        return false;
    }

}
