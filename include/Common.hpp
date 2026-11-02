#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>
#include <windowsx.h>
#include <wrl/client.h>
#include <d2d1.h>
#include <d2d1_1.h>
#include <dwrite.h>
#include <dwmapi.h>
#include <shellapi.h>

#include <string>
#include <string_view>
#include <memory>
#include <vector>
#include <functional>
#include <optional>
#include <chrono>

namespace xe {
    // Convenient alias for COM Smart Pointers
    template<typename T>
    using ComPtr = Microsoft::WRL::ComPtr<T>;

    // Color tokens matching the XE editorial design system (Dark Theme default)
    namespace Colors {
        // Dark theme tokens
        inline constexpr D2D1_COLOR_F BgDark       = { 0.067f, 0.067f, 0.063f, 1.0f }; // #111110
        inline constexpr D2D1_COLOR_F SurfaceDark  = { 0.110f, 0.106f, 0.098f, 1.0f }; // #1C1B19
        inline constexpr D2D1_COLOR_F BorderDark   = { 0.165f, 0.157f, 0.145f, 1.0f }; // #2A2825
        inline constexpr D2D1_COLOR_F FgDark       = { 0.941f, 0.933f, 0.914f, 1.0f }; // #F0EEE9
        inline constexpr D2D1_COLOR_F MutedDark    = { 0.478f, 0.467f, 0.455f, 1.0f }; // #7A7774
        inline constexpr D2D1_COLOR_F FaintDark    = { 0.302f, 0.294f, 0.282f, 1.0f }; // #4D4B48

        // Light theme tokens (Portfolio Reference)
        inline constexpr D2D1_COLOR_F BgLight      = { 0.922f, 0.914f, 0.894f, 1.0f }; // #EBE9E4
        inline constexpr D2D1_COLOR_F SurfaceLight = { 0.867f, 0.859f, 0.835f, 1.0f }; // #DDDBD5
        inline constexpr D2D1_COLOR_F BorderLight  = { 0.816f, 0.808f, 0.788f, 1.0f }; // #D0CEC9
        inline constexpr D2D1_COLOR_F FgLight      = { 0.071f, 0.071f, 0.071f, 1.0f }; // #121212
        inline constexpr D2D1_COLOR_F MutedLight   = { 0.412f, 0.408f, 0.396f, 1.0f }; // #696865
        inline constexpr D2D1_COLOR_F FaintLight   = { 0.659f, 0.651f, 0.639f, 1.0f }; // #A8A6A3
    }
}
