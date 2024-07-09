if (BUILD_EMULATOR_TESTS)
    include_directories("${PROJECT_SOURCE_DIR}/external/glad" "${PROJECT_SOURCE_DIR}/external/glfw/include" 
        "${PROJECT_SOURCE_DIR}/src")

    set(TEST_TARGETS window)
    add_executable(window window.cpp "../src/core/window.h" "../src/core/window.cpp" "../external/glad/glad/glad.c")
    
    foreach(TEST_TARGET IN LISTS TEST_TARGETS)
        set_target_properties("${TEST_TARGET}" PROPERTIES 
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/tests/$<IF:$<CONFIG:Debug>,debug,release>"
            LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/tests/$<IF:$<CONFIG:Debug>,debug,release>"
            ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/tests/$<IF:$<CONFIG:Debug>,debug,release>"
            FOLDER "Tests")

        target_link_libraries("${TEST_TARGET}" PRIVATE glfw)
    endforeach()

    add_test(NAME WindowTest COMMAND window)
endif()