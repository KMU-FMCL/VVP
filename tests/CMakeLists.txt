# Find Google Test for testing
find_package(GTest QUIET)

# Enable testing
enable_testing()

if(NOT GTEST_FOUND)
    # Try one of three methods if system GTest package is not found
    # 1) Use googletest submodule in the test directory
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/googletest/CMakeLists.txt)
        add_subdirectory(
            ${CMAKE_CURRENT_SOURCE_DIR}/googletest
            ${CMAKE_CURRENT_BINARY_DIR}/googletest
        )
    else()
        # 2) Download GoogleTest remotely using FetchContent
        include(FetchContent)
        FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG v1.14.0
        )
        FetchContent_MakeAvailable(googletest)
    endif()
endif()


# Implementation files list
set(IMPLEMENTATION_SOURCES
    ${CMAKE_SOURCE_DIR}/src/estimation/VVEstimator.cpp
    ${CMAKE_SOURCE_DIR}/src/processing/ImageProcessor.cpp
    ${CMAKE_SOURCE_DIR}/src/io/IOHandler.cpp
    ${CMAKE_SOURCE_DIR}/src/estimation/Types.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/Helpers.cpp
)

# Test source files list
set(TEST_SOURCES
    test_vv_estimator.cpp
    test_image_processor.cpp
)

# Store test targets list
set(TEST_TARGETS)

# Create each test target
foreach(test_source ${TEST_SOURCES})
    get_filename_component(test_name ${test_source} NAME_WE)
    add_executable(${test_name} ${test_source} ${IMPLEMENTATION_SOURCES})
    list(APPEND TEST_TARGETS ${test_name})
    
    if(GTEST_FOUND)
        # Use GTest headers and libraries found in the system
        target_include_directories(${test_name} PRIVATE ${GTEST_INCLUDE_DIRS})
        target_link_libraries(${test_name} PRIVATE ${GTEST_BOTH_LIBRARIES})
    else()
        target_link_libraries(${test_name} PRIVATE gtest gtest_main)
    endif()

    # Define project root path (used in IOHandler, etc.)
    target_compile_definitions(${test_name} PRIVATE PROJECT_ROOT="${CMAKE_SOURCE_DIR}")
    
    # Link OpenCV libraries
    target_link_libraries(${test_name} PRIVATE ${OpenCV_LIBS})
    
    # Link Abseil libraries
    target_link_libraries(${test_name} PRIVATE
        absl::strings
        absl::str_format
        absl::time
        absl::span
        absl::flat_hash_map
        absl::flat_hash_set
        absl::status
        absl::statusor
    )
    
    # Add header file directories used for testing
    target_include_directories(${test_name} PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
    )
    
    # Thread library required when using Google Test
    find_package(Threads REQUIRED)
    target_link_libraries(${test_name} PRIVATE Threads::Threads)
    
    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()

# Add test execution target
add_custom_target(run_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS ${TEST_TARGETS}
) 