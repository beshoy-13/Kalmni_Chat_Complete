cmake_minimum_required(VERSION 3.10)
project(Kalmni_Client)

set(CMAKE_CXX_STANDARD 17)

# Use pkg-config to get SDL2 and OpenGL
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)

include_directories(
    ${SDL2_INCLUDE_DIRS}
    imgui
    imgui/backends
)

link_directories(${SDL2_LIBRARY_DIRS})

set(IMGUI_SOURCES
    imgui/imgui.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_widgets.cpp
    imgui/imgui_tables.cpp
    imgui/backends/imgui_impl_sdl2.cpp
    imgui/backends/imgui_impl_sdlrenderer2.cpp
)

add_executable(Kalmni_Client
    main.cpp
    ${IMGUI_SOURCES}
)

target_link_libraries(Kalmni_Client
    ${SDL2_LIBRARIES}
    GL
    pthread
)

