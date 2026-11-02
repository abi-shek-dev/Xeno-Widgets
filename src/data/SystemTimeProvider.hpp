#pragma once

#include "Common.hpp"
#include <string>

namespace xe {

    class SystemTimeProvider {
    public:
        struct TimeData {
            int hour = 0;
            int minute = 0;
            int second = 0;
            int day = 1;
            int month = 1;
            int year = 2026;
            std::wstring dayOfWeek; // L"Monday" etc.
            std::wstring monthName; // L"November" etc.
            std::wstring timeStr;   // L"14:30:45"
            std::wstring dateStr;   // L"MON, 02 NOV 2026"
        };

        static TimeData GetCurrentTime();
    };

}
