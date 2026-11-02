cmake_minimum_required(VERSION 3.20)

project(XEWidgets VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Source files
set(SOURCES
    src/main.cpp
    src/core/Engine.cpp
    src/graphics/D2DRenderer.cpp
    src/windows/Window.cpp
)

set(HEADERS
    include/Common.hpp
    src/core/Logger.hpp
    src/core/Engine.hpp
    src/graphics/D2DRenderer.hpp
    src/windows/Window.hpp
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
)

# Output directory
set_target_properties(XEWidgets PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    OUTPUT_NAME "XEWidgets"
)
