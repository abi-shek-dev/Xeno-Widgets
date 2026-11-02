#include "Common.hpp"
#include "core/Engine.hpp"
#include "core/Logger.hpp"

int WINAPI wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE /*hPrevInstance*/,
    _In_ LPWSTR /*lpCmdLine*/,
    _In_ int /*nCmdShow*/
) {
    XE_LOG_INFO("=================================================");
    XE_LOG_INFO("   XE WIDGETS — NATIVE C++20 DESKTOP PLATFORM    ");
    XE_LOG_INFO("   Direct2D · DirectWrite · Win32 Hardware Render");
    XE_LOG_INFO("=================================================");

    xe::Engine engine;
    if (!engine.Initialize(hInstance)) {
        XE_LOG_ERROR("XE Engine initialization failed. Exiting...");
        return -1;
    }

    int exitCode = engine.Run();
    return exitCode;
}

// Console entry point fallback for developer terminal builds
int main() {
    return wWinMain(GetModuleHandleW(nullptr), nullptr, GetCommandLineW(), SW_SHOWNORMAL);
}
