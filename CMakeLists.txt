cmake_minimum_required(VERSION 3.0.0)
project(Chip8Emulator VERSION 0.1.0 LANGUAGES C CXX)

# Set CMake macro variables
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CONFIGURATION_TYPES "Debug" "Release")
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# Define project include directories, and the project source code files
set(PROJECT_INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}/external/SDL/include" "${PROJECT_SOURCE_DIR}/external/SDL_mixer/include"
    "${PROJECT_SOURCE_DIR}/external/json/include" "${PROJECT_SOURCE_DIR}/src")

set(PROJECT_HEADER_FILES "src/vector.h" "src/core/window.h" "src/core/renderer.h" "src/core/interpreter.h" "src/debugging.h")
set(PROJECT_SOURCE_FILES "src/main.cpp" "src/core/window.cpp" "src/core/renderer.cpp" "src/core/interpreter.cpp")

set(BUILD_SHARED_LIBS OFF) # Force SDL to be built statically
option(BUILD_EMULATOR_TESTS "Defines whether or not the emulator tests should be built" ON)

# Define executable target and configure the target
add_executable(Chip8Emulator "${PROJECT_HEADER_FILES}" "${PROJECT_SOURCE_FILES}")
target_include_directories(Chip8Emulator PUBLIC "${PROJECT_INCLUDE_DIRECTORIES}")
target_compile_definitions(Chip8Emulator PUBLIC "$<$<CONFIG:Debug>:DEBUG_MODE>")

set_target_properties(Chip8Emulator PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY "$<IF:$<CONFIG:Debug>,${CMAKE_BINARY_DIR}/bin/debug,${CMAKE_BINARY_DIR}/bin/release>"
    LIBRARY_OUTPUT_DIRECTORY "$<IF:$<CONFIG:Debug>,${CMAKE_BINARY_DIR}/bin/debug,${CMAKE_BINARY_DIR}/bin/release>"
    ARCHIVE_OUTPUT_DIRECTORY "$<IF:$<CONFIG:Debug>,${CMAKE_BINARY_DIR}/bin/debug,${CMAKE_BINARY_DIR}/bin/release>")

add_subdirectory("external/SDL")
add_subdirectory("external/SDL_mixer")
add_subdirectory("external/json")

target_link_libraries(Chip8Emulator PRIVATE SDL3-static SDL3_mixer-static)

if (MSVC)
    target_compile_options(Chip8Emulator PRIVATE "/std:c++17") # Force MSVC to use C++17 standard
    target_link_options(Chip8Emulator PRIVATE "$<IF:$<CONFIG:Debug>,/SUBSYSTEM:CONSOLE,/SUBSYSTEM:WINDOWS>"
        "$<$<CONFIG:Release>:/ENTRY:mainCRTStartup>")
    target_compile_definitions(Chip8Emulator PUBLIC "USING_MSVC")
endif()

add_custom_command(TARGET Chip8Emulator POST_BUILD COMMAND "${CMAKE_COMMAND}" -E copy_if_different 
    "${CMAKE_SOURCE_DIR}/assets/beep.wav" "${CMAKE_BINARY_DIR}/bin/$<IF:$<CONFIG:Debug>,debug,release>/assets/beep.wav")

include(CTest)
enable_testing()

add_subdirectory("tests")