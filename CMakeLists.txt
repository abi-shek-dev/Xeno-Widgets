cmake_minimum_required(VERSION 3.20)

project(XEWidgets VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Source files
set(SOURCES
    src/main.cpp
    src/core/Engine.cpp
    src/core/WidgetConfig.cpp
    src/graphics/D2DRenderer.cpp
    src/windows/Window.cpp
    src/windows/WidgetWindow.cpp
    src/windows/WindowManager.cpp
    src/data/SystemTimeProvider.cpp
    src/data/SMTCProvider.cpp
    src/data/TelemetryProvider.cpp
    src/manager/TrayManager.cpp
    src/widgets/ClockWidget.cpp
    src/widgets/MusicWidget.cpp
    src/widgets/SystemMonitorWidget.cpp
    src/widgets/WeatherWidget.cpp
    src/widgets/NotesWidget.cpp
    src/widgets/CalendarWidget.cpp
)

set(HEADERS
    include/Common.hpp
    src/core/Logger.hpp
    src/core/Engine.hpp
    src/core/WidgetConfig.hpp
    src/graphics/D2DRenderer.hpp
    src/windows/Window.hpp
    src/windows/WidgetWindow.hpp
    src/windows/WindowManager.hpp
    src/data/SystemTimeProvider.hpp
    src/data/SMTCProvider.hpp
    src/data/TelemetryProvider.hpp
    src/manager/TrayManager.hpp
    src/widgets/ClockWidget.hpp
    src/widgets/MusicWidget.hpp
    src/widgets/SystemMonitorWidget.hpp
    src/widgets/WeatherWidget.hpp
    src/widgets/NotesWidget.hpp
    src/widgets/CalendarWidget.hpp
)

# Native Windows Executable
add_executable(XEWidgets WIN32
    ${SOURCES}
    ${HEADERS}
)

# Target definitions
target_compile_definitions(XEWidgets PRIVATE
    UNICODE
    _UNICODE
    WIN32_LEAN_AND_MEAN
    NOMINMAX
)

# Include search paths
target_include_directories(XEWidgets PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# MSVC Compiler Options
if(MSVC)
    target_compile_options(XEWidgets PRIVATE
        /std:c++20
        /W4
        /permissive-
        /utf-8
        /EHsc
    )
    # Support both WinMain and main entry points
    target_link_options(XEWidgets PRIVATE
        /SUBSYSTEM:WINDOWS
        /ENTRY:wWinMainCRTStartup
    )
endif()

# Win32, Direct2D, and DirectWrite Native System Libraries
target_link_libraries(XEWidgets PRIVATE
    d2d1
    dwrite
    windowscodecs
    dwmapi
    shcore
    user32
    gdi32
    ole32
    shell32
    iphlpapi
)

# Windows Runtime / C++/WinRT Support for SMTC Integration
set(WINRT_FOUND FALSE)
file(GLOB WINSDK_CPPWINRT_DIRS "C:/Program Files (x86)/Windows Kits/10/Include/10.*/cppwinrt")
if(WINSDK_CPPWINRT_DIRS)
    list(GET WINSDK_CPPWINRT_DIRS -1 DETECTED_CPPWINRT_DIR)
    if(EXISTS "${DETECTED_CPPWINRT_DIR}/winrt/base.h")
        set(WINRT_FOUND TRUE)
        set(CPPWINRT_INCLUDE_PATH "${DETECTED_CPPWINRT_DIR}")
    endif()
endif()

if(NOT WINRT_FOUND AND DEFINED ENV{WindowsSdkDir})
    set(ENV_SDK_DIR "$ENV{WindowsSdkDir}")
    set(ENV_SDK_VER "$ENV{WindowsSDKVersion}")
    string(REGEX REPLACE "[\\/]+$" "" ENV_SDK_DIR "${ENV_SDK_DIR}")
    string(REGEX REPLACE "[\\/]+$" "" ENV_SDK_VER "${ENV_SDK_VER}")
    if(EXISTS "${ENV_SDK_DIR}/Include/${ENV_SDK_VER}/cppwinrt/winrt/base.h")
        set(WINRT_FOUND TRUE)
        set(CPPWINRT_INCLUDE_PATH "${ENV_SDK_DIR}/Include/${ENV_SDK_VER}/cppwinrt")
    endif()
endif()

if(WINRT_FOUND)
    message(STATUS "C++/WinRT headers located at: ${CPPWINRT_INCLUDE_PATH}")
    target_compile_definitions(XEWidgets PRIVATE XE_SMTC_ENABLED)
    target_include_directories(XEWidgets SYSTEM PRIVATE "${CPPWINRT_INCLUDE_PATH}")
    target_link_libraries(XEWidgets PRIVATE windowsapp)
else()
    message(STATUS "C++/WinRT headers not found; SMTCProvider using fallback demo mode")
endif()

# Output directory
set_target_properties(XEWidgets PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    OUTPUT_NAME "XEWidgets"
)
