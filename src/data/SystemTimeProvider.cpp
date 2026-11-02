#include "SystemTimeProvider.hpp"
#include <cstdio>

namespace xe {

    static const wchar_t* const s_daysFull[] = {
        L"Sunday", L"Monday", L"Tuesday", L"Wednesday", L"Thursday", L"Friday", L"Saturday"
    };

    static const wchar_t* const s_daysAbbr[] = {
        L"SUN", L"MON", L"TUE", L"WED", L"THU", L"FRI", L"SAT"
    };

    static const wchar_t* const s_monthsFull[] = {
        L"January", L"February", L"March", L"April", L"May", L"June",
        L"July", L"August", L"September", L"October", L"November", L"December"
    };

    static const wchar_t* const s_monthsAbbr[] = {
        L"JAN", L"FEB", L"MAR", L"APR", L"MAY", L"JUN",
        L"JUL", L"AUG", L"SEP", L"OCT", L"NOV", L"DEC"
    };

    SystemTimeProvider::TimeData SystemTimeProvider::GetCurrentTime() {
        SYSTEMTIME st{};
        GetLocalTime(&st);

        TimeData data{};
        data.hour = st.wHour;
        data.minute = st.wMinute;
        data.second = st.wSecond;
        data.day = st.wDay;
        data.month = st.wMonth;
        data.year = st.wYear;

        if (st.wDayOfWeek < 7) {
            data.dayOfWeek = s_daysFull[st.wDayOfWeek];
        }

        if (st.wMonth >= 1 && st.wMonth <= 12) {
            data.monthName = s_monthsFull[st.wMonth - 1];
        }

        wchar_t timeBuf[32]{};
        swprintf_s(timeBuf, L"%02d:%02d:%02d", st.wHour, st.wMinute, st.wSecond);
        data.timeStr = timeBuf;

        wchar_t dateBuf[64]{};
        const wchar_t* dayAbbr = (st.wDayOfWeek < 7) ? s_daysAbbr[st.wDayOfWeek] : L"";
        const wchar_t* monAbbr = (st.wMonth >= 1 && st.wMonth <= 12) ? s_monthsAbbr[st.wMonth - 1] : L"";
        swprintf_s(dateBuf, L"%s, %02d %s %04d", dayAbbr, st.wDay, monAbbr, st.wYear);
        data.dateStr = dateBuf;

        return data;
    }

}
