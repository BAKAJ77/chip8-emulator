cmake_minimum_required(VERSION 3.0.0)
project(Chip8Emulator VERSION 0.1.0 LANGUAGES C CXX)

# Set CMake macro variables
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CONFIGURATION_TYPES "Debug" "Release")
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# Define project include directories, and the project source code files
set(PROJECT_INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}/external/glad" "${PROJECT_SOURCE_DIR}/external/glfw/include"
    "${PROJECT_SOURCE_DIR}/src")

set(PROJECT_HEADER_FILES "")
set(PROJECT_SOURCE_FILES "external/glad/glad/glad.c" "src/main.cpp" "src/core/window.h" "src/core/window.cpp")

set(BUILD_SHARED_LIBS OFF) # Force GLFW to be built statically
option(BUILD_EMULATOR_TESTS "Defines whether or not the emulator tests should be built" ON)

# Define executable target and configure the target
add_executable(Chip8Emulator "${PROJECT_HEADER_FILES}" "${PROJECT_SOURCE_FILES}")
target_include_directories(Chip8Emulator PUBLIC "${PROJECT_INCLUDE_DIRECTORIES}")
set_target_properties(Chip8Emulator PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY "$<IF:$<CONFIG:Debug>,${CMAKE_BINARY_DIR}/bin/debug,${CMAKE_BINARY_DIR}/bin/release>"
    LIBRARY_OUTPUT_DIRECTORY "$<IF:$<CONFIG:Debug>,${CMAKE_BINARY_DIR}/bin/debug,${CMAKE_BINARY_DIR}/bin/release>"
    ARCHIVE_OUTPUT_DIRECTORY "$<IF:$<CONFIG:Debug>,${CMAKE_BINARY_DIR}/bin/debug,${CMAKE_BINARY_DIR}/bin/release>")

add_subdirectory("external/glfw")
target_link_libraries(Chip8Emulator PRIVATE glfw)

if (MSVC)
    target_compile_options(Chip8Emulator PRIVATE "/std:c++17") # Force MSVC to use C++17 standard
    target_link_options(Chip8Emulator PRIVATE "$<IF:$<CONFIG:Debug>,/SUBSYSTEM:CONSOLE,/SUBSYSTEM:WINDOWS>")
endif()

include(CTest)
enable_testing()

add_subdirectory("tests")