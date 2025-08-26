cmake_minimum_required(VERSION 3.16)
project(reif LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(MSYS2_PREFIX "F:/msys2/mingw64") # change this to your path where ur msys is !!!!
endif()

list(APPEND CMAKE_PREFIX_PATH "${MSYS2_PREFIX}")

find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(GLEW CONFIG REQUIRED)
find_package(Freetype REQUIRED)
find_package(HarfBuzz REQUIRED)

include_directories(
    vendor/imgui
    vendor/imgui/backends
)

set(IMGUI_SOURCES
    vendor/imgui/imgui.cpp
    vendor/imgui/imgui_draw.cpp
    vendor/imgui/imgui_tables.cpp
    vendor/imgui/imgui_widgets.cpp
    vendor/imgui/backends/imgui_impl_opengl3.cpp
    vendor/imgui/backends/imgui_impl_glfw.cpp
)

file(GLOB_RECURSE SRC_FILES
    src/*.cpp
    src/**/*.cpp
)

add_library(glm::glm INTERFACE IMPORTED)
target_include_directories(glm::glm INTERFACE "${MSYS2_PREFIX}/include")

add_executable(reif
    ${SRC_FILES}
    ${IMGUI_SOURCES}
)

target_include_directories(reif PRIVATE
    ${OPENGL_INCLUDE_DIR}
    ${FREETYPE_INCLUDE_DIRS}
    ${HarfBuzz_INCLUDE_DIRS}
    ${MSYS2_PREFIX}/include
)

target_link_libraries(reif PRIVATE
    OpenGL::GL
    glfw
    GLEW::GLEW
    glm::glm
    Freetype::Freetype
    ${HarfBuzz_LIBRARIES}

    "${MSYS2_PREFIX}/lib/libz.a"
    "${MSYS2_PREFIX}/lib/libpng16.a"
    "${MSYS2_PREFIX}/lib/libgraphite2.a"
    "${MSYS2_PREFIX}/lib/libbrotlidec.a"
    "${MSYS2_PREFIX}/lib/libbrotlienc.a"
    "${MSYS2_PREFIX}/lib/libbrotlicommon.a"
    "${MSYS2_PREFIX}/lib/libbz2.a"

    -ldwrite
    -luuid
    -lole32
)
