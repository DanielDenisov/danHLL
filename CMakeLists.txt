cmake_minimum_required(VERSION 4.1)
project(danHLL)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
set(IMGUI_DIR "${CMAKE_SOURCE_DIR}/imgui")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${IMGUI_DIR})
include_directories(${IMGUI_DIR}/backends)


set(PROJECT_SOURCES
        main.cpp
        config.h
        gamestate/GameState.h
        gamestate/GameState.cpp
        memory/memory.h
        memory/memory.cpp
        draw/Overlay.h
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
        ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
        utils/gameUtil.h
        utils/localUtil.h
        draw/ESP.h
)

add_executable(danHLL ${PROJECT_SOURCES})

target_link_libraries(danHLL PRIVATE glfw OpenGL::GL)
