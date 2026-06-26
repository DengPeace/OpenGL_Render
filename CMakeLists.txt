cmake_minimum_required(VERSION 3.10)
project(TryGL)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)

# GLFW
add_subdirectory(3rd/glfw)

# GLM — header only
# (already available via include)

# ImGui
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/imgui)

# Source files
file(GLOB_RECURSE SOURCES
    "*.cpp"
    "wrapper/*.cpp"
    "glframework/*.cpp"
    "glframework/framebuffer/*.cpp"
    "glframework/light/*.cpp"
    "glframework/material/*.cpp"
    "glframework/mesh/*.cpp"
    "glframework/renderer/*.cpp"
    "application/*.cpp"
    "application/camera/*.cpp"
)

# Sources in submodules that we compile directly
set(EXTRA_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/imgui_impl_opengl3.cpp
    lib/glad.c
)

add_executable(${PROJECT_NAME} ${SOURCES} ${EXTRA_SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/3rd/glfw/include
    ${CMAKE_CURRENT_SOURCE_DIR}/glm
    ${CMAKE_CURRENT_SOURCE_DIR}/wrapper
    ${CMAKE_CURRENT_SOURCE_DIR}/glframework
    ${CMAKE_CURRENT_SOURCE_DIR}/glframework/framebuffer
    ${CMAKE_CURRENT_SOURCE_DIR}/glframework/light
    ${CMAKE_CURRENT_SOURCE_DIR}/glframework/material
    ${CMAKE_CURRENT_SOURCE_DIR}/glframework/mesh
    ${CMAKE_CURRENT_SOURCE_DIR}/glframework/renderer
    ${CMAKE_CURRENT_SOURCE_DIR}/application
    ${CMAKE_CURRENT_SOURCE_DIR}/application/camera
    ${IMGUI_DIR}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    glfw
    ${CMAKE_DL_LIBS}
)

# macOS specific
if(APPLE)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(COREVIDEO_LIBRARY CoreVideo)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        ${COCOA_LIBRARY}
        ${IOKIT_LIBRARY}
        ${COREVIDEO_LIBRARY}
    )
endif()

# assimp (if needed)
# find_package(ASSIMP REQUIRED)
# target_link_libraries(${PROJECT_NAME} PRIVATE assimp::assimp)
