#include "Common.hpp"
#include "core/Engine.hpp"
#include "core/Logger.hpp"

int WINAPI wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE /*hPrevInstance*/,
    _In_ LPWSTR /*lpCmdLine*/,
    _In_ int /*nCmdShow*/
) {
    if (AttachConsole(ATTACH_PARENT_PROCESS)) {
        FILE* dummy;
        freopen_s(&dummy, "CONOUT$", "w", stdout);
        freopen_s(&dummy, "CONOUT$", "w", stderr);
    }

    // Single-instance enforcement: Prevents duplicate widgets on restart
    SetLastError(0);
    HANDLE hMutex = CreateMutexW(nullptr, TRUE, L"XEWidgets_SingleInstance_Mutex_9921");
    if (hMutex && GetLastError() == ERROR_ALREADY_EXISTS) {
        XE_LOG_WARN("Another instance of XEWidgets is already running. Focusing existing instance...");
        HWND existing = FindWindowW(L"XEWidgets_Native_Class", nullptr);
        if (existing) {
            ShowWindow(existing, SW_SHOW);
            SetForegroundWindow(existing);
            // Tell the running instance to show all its widgets in case they were hidden
            PostMessageW(existing, WM_APP + 2, 0, 0);
        }
        CloseHandle(hMutex);
        return 0;
    }

    XE_LOG_INFO("=================================================");
    XE_LOG_INFO("   XE WIDGETS — NATIVE C++20 DESKTOP PLATFORM    ");
    XE_LOG_INFO("   Direct2D · DirectWrite · Win32 Hardware Render");
    XE_LOG_INFO("=================================================");

    xe::Engine engine;
    if (!engine.Initialize(hInstance)) {
        XE_LOG_ERROR("XE Engine initialization failed. Exiting...");
        if (hMutex) CloseHandle(hMutex);
        return -1;
    }

    int exitCode = engine.Run();

    if (hMutex) {
        CloseHandle(hMutex);
    }
    return exitCode;
}

// Console entry point fallback for developer terminal builds
int main() {
    return wWinMain(GetModuleHandleW(nullptr), nullptr, GetCommandLineW(), SW_SHOWNORMAL);
}
