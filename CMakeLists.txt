# -----------------------------------------------------------------------------
# CMakeLists.txt for the Contact Management System.
#
# Cross-platform build (Linux, macOS, Windows). Produces a single
# executable from src/*.cpp, pulls headers from include/, and links
# against Dear ImGui, GLFW, and nlohmann::json — all vendored under
# external/ (GLFW and ImGui as git submodules, nlohmann::json as a
# single-header drop). Run `git submodule update --init --recursive`
# after cloning so the submodules are populated.
# -----------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.15)

project(ContactManagementSystem
        VERSION 1.0.0
        DESCRIPTION "Contact Management System (C++20 + Dear ImGui)"
        LANGUAGES CXX)

# -----------------------------------------------------------------------------
# Language / compile options
# -----------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Emit compile_commands.json so clangd / editor tooling resolves headers.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release)
endif()

if(MSVC)
    add_compile_options(/W4 /permissive- /utf-8)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS NOMINMAX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Put the built binary in bin/<Config> so it's easy to find on every platform.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# -----------------------------------------------------------------------------
# Third-party dependencies (vendored under external/)
# -----------------------------------------------------------------------------
set(EXTERNAL_DIR "${CMAKE_SOURCE_DIR}/external")

set(NLOHMANN_JSON_DIR "${EXTERNAL_DIR}/json")
set(IMGUI_DIR         "${EXTERNAL_DIR}/imgui")
set(GLFW_DIR          "${EXTERNAL_DIR}/glfw")

# Friendly error if anyone forgot `git submodule update --init --recursive`.
foreach(_dep_path "${NLOHMANN_JSON_DIR}/json.hpp"
                  "${IMGUI_DIR}/imgui.h"
                  "${GLFW_DIR}/CMakeLists.txt")
    if(NOT EXISTS "${_dep_path}")
        message(FATAL_ERROR
            "Missing dependency file: ${_dep_path}\n"
            "Run: git submodule update --init --recursive")
    endif()
endforeach()

# --- nlohmann/json (single header) -------------------------------------------
add_library(nlohmann_json INTERFACE)
target_include_directories(nlohmann_json INTERFACE "${NLOHMANN_JSON_DIR}")

# --- GLFW (built from the submodule) -----------------------------------------
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)
add_subdirectory("${GLFW_DIR}" EXCLUDE_FROM_ALL)

find_package(OpenGL REQUIRED)

# --- Dear ImGui (compiled from submodule sources) ----------------------------
set(IMGUI_SOURCE_FILES
    "${IMGUI_DIR}/imgui.cpp"
    "${IMGUI_DIR}/imgui_draw.cpp"
    "${IMGUI_DIR}/imgui_tables.cpp"
    "${IMGUI_DIR}/imgui_widgets.cpp"
    "${IMGUI_DIR}/imgui_demo.cpp"
    "${IMGUI_DIR}/backends/imgui_impl_glfw.cpp"
    "${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp")

add_library(imgui STATIC ${IMGUI_SOURCE_FILES})
target_include_directories(imgui PUBLIC
    "${IMGUI_DIR}"
    "${IMGUI_DIR}/backends")
target_link_libraries(imgui PUBLIC glfw OpenGL::GL)

# -----------------------------------------------------------------------------
# Application target
# -----------------------------------------------------------------------------
add_executable(contact_management_system
    src/main.cpp
    src/application.cpp
    src/glfw.cpp
    src/presentation.cpp
    src/logic.cpp
    src/data.cpp)

target_include_directories(contact_management_system PRIVATE
    "${CMAKE_SOURCE_DIR}/include"
    "${EXTERNAL_DIR}"
    "${NLOHMANN_JSON_DIR}"
    "${GLFW_DIR}/include")

target_link_libraries(contact_management_system PRIVATE
    imgui
    nlohmann_json
    glfw
    OpenGL::GL)

# Copy the sample data file next to the executable so a fresh build has
# something to open out of the box.
add_custom_command(TARGET contact_management_system POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
            "$<TARGET_FILE_DIR:contact_management_system>/data"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_SOURCE_DIR}/data/contacts.json"
            "$<TARGET_FILE_DIR:contact_management_system>/data/contacts.json"
    VERBATIM)
