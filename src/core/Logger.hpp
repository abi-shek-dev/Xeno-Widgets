#pragma once

#include "Common.hpp"
#include <mutex>
#include <iostream>
#include <fstream>
#include <format>
#include <chrono>

namespace xe {

    inline std::string GetLogFilePath() {
        char* appData = nullptr;
        size_t len = 0;
        if (_dupenv_s(&appData, &len, "APPDATA") == 0 && appData != nullptr) {
            std::string dir = std::string(appData) + "\\XEWidgets";
            CreateDirectoryA(dir.c_str(), nullptr);
            std::string path = dir + "\\XEWidgets.log";
            free(appData);
            return path;
        }
        return "XEWidgets.log";
    }

    enum class LogLevel {
        Debug,
        Info,
        Warning,
        Error
    };

    class Logger {
    public:
        static Logger& Instance() {
            static Logger instance;
            return instance;
        }

        void Log(LogLevel level, std::string_view message) {
            std::lock_guard<std::mutex> lock(m_mutex);

            auto now = std::chrono::system_clock::now();
            auto time = std::chrono::system_clock::to_time_t(now);
            std::tm tm{};
            localtime_s(&tm, &time);

            const char* levelStr = "INFO";
            switch (level) {
                case LogLevel::Debug:   levelStr = "DEBUG"; break;
                case LogLevel::Info:    levelStr = "INFO";  break;
                case LogLevel::Warning: levelStr = "WARN";  break;
                case LogLevel::Error:   levelStr = "ERROR"; break;
            }

            char timeBuffer[32];
            std::strftime(timeBuffer, sizeof(timeBuffer), "%H:%M:%S", &tm);

            std::string formatted = std::format("[{}] [{}] {}\n", timeBuffer, levelStr, message);

            std::cout << formatted;
            OutputDebugStringA(formatted.c_str());

            static std::ofstream logFile(GetLogFilePath(), std::ios::app);
            if (logFile.is_open()) {
                logFile << formatted;
                logFile.flush();
            }
        }

    private:
        Logger() = default;
        ~Logger() = default;
        Logger(const Logger&) = delete;
        Logger& operator=(const Logger&) = delete;

        std::mutex m_mutex;
    };

    #define XE_LOG_DEBUG(msg) xe::Logger::Instance().Log(xe::LogLevel::Debug, (msg))
    #define XE_LOG_INFO(msg)  xe::Logger::Instance().Log(xe::LogLevel::Info, (msg))
    #define XE_LOG_WARN(msg)  xe::Logger::Instance().Log(xe::LogLevel::Warning, (msg))
    #define XE_LOG_ERROR(msg) xe::Logger::Instance().Log(xe::LogLevel::Error, (msg))
}
