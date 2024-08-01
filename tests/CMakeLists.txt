if (BUILD_EMULATOR_TESTS)
    include_directories("${PROJECT_SOURCE_DIR}/external/SDL/include" "${PROJECT_SOURCE_DIR}/src" "${PROJECT_BINARY_DIR}/tests")

    configure_file("config.h.in" "config.h")

    set(TEST_TARGETS window interpreter)
    add_executable(window "window.cpp" "../src/vector.h" "../src/core/window.h" "../src/core/window.cpp" "../src/core/renderer.h" 
        "../src/core/renderer.cpp")

    add_executable(interpreter "interpreter.cpp" "../src/core/interpreter.h" "../src/core/interpreter.cpp")
    target_compile_definitions(interpreter PUBLIC INTERPRETER_IMPL_TEST)
    
    foreach(TEST_TARGET IN LISTS TEST_TARGETS)
        set_target_properties("${TEST_TARGET}" PROPERTIES 
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/tests/$<IF:$<CONFIG:Debug>,debug,release>"
            LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/tests/$<IF:$<CONFIG:Debug>,debug,release>"
            ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/tests/$<IF:$<CONFIG:Debug>,debug,release>"
            FOLDER "Tests")

        target_link_libraries("${TEST_TARGET}" PRIVATE SDL3-static)
    endforeach()

    add_test(NAME window COMMAND window)
    add_test(NAME interpreter COMMAND interpreter)
endif()