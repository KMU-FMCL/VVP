# Add source files
set(SOURCES
    main.cc
    io/input_handler.cc
    io/output_handler.cc
    processing/image_processor.cc
    processing/constants.cc # Added constants.cc
    estimation/vv_estimator.cc
    estimation/vv_histogram_visualizer.cc
    estimation/types.cc
    utils/helpers.cc
    utils/config_loader.cc
    utils/display.cc
    utils/file_system.cc
    utils/path.cc
    utils/time.cc
    fps/fps_counter.cc
)

# Build executable
add_executable(vv_estimator ${SOURCES})

# -- yaml-cpp linking fallback across environments --
if(TARGET yaml-cpp::yaml-cpp)
    set(YAMLCPP_LIB yaml-cpp::yaml-cpp)
elseif(TARGET yaml-cpp)
    set(YAMLCPP_LIB yaml-cpp)
elseif(YAML_CPP_LIBRARIES)
    set(YAMLCPP_LIB ${YAML_CPP_LIBRARIES})
    # include dirs for variable-based yaml-cpp
    include_directories(${YAML_CPP_INCLUDE_DIRS})
else()
    message(FATAL_ERROR "yaml-cpp library not found; please install yaml-cpp")
endif()

# Link libraries
# define PROJECT_ROOT macro to locate config directory
# Macro PROJECT_ROOT expands to the project source directory
target_compile_definitions(vv_estimator PRIVATE PROJECT_ROOT="${CMAKE_SOURCE_DIR}")
target_link_libraries(vv_estimator
    PRIVATE
        ${OpenCV_LIBS}
        ${YAMLCPP_LIB}
        absl::strings
        absl::str_format
        absl::time
        absl::span
        absl::flat_hash_map
        absl::flat_hash_set
        absl::status
        absl::statusor
)

# Add compiler options
target_compile_options(vv_estimator PRIVATE 
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
) 

# Add clang-tidy integration for our targets (not external libraries)
option(ENABLE_CLANG_TIDY "Enable clang-tidy checks during build" ON)
if(ENABLE_CLANG_TIDY)
    find_program(CLANG_TIDY_EXE clang-tidy)
    if(CLANG_TIDY_EXE)
        message(STATUS "Found clang-tidy: ${CLANG_TIDY_EXE}")
        # Configure clang-tidy to run only on our specific target
        set_target_properties(vv_estimator PROPERTIES
            CXX_CLANG_TIDY "${CLANG_TIDY_EXE};-warnings-as-errors=*"
        )
    else()
        message(WARNING "clang-tidy requested but executable not found")
    endif()
endif()

# Add clang-format integration to format code before build
option(ENABLE_CLANG_FORMAT "Enable clang-format to automatically format code" ON)
if(ENABLE_CLANG_FORMAT)
    # Find clang-format executable
    find_program(CLANG_FORMAT_EXE clang-format)
    if(CLANG_FORMAT_EXE)
        message(STATUS "Found clang-format: ${CLANG_FORMAT_EXE}")
        
        # Get all source files that should be formatted
        file(GLOB_RECURSE ALL_SOURCE_FILES 
            ${CMAKE_SOURCE_DIR}/src/*.cc
            ${CMAKE_SOURCE_DIR}/src/*.h
            ${CMAKE_SOURCE_DIR}/include/*.h
        )
        
        # Create custom target for formatting
        add_custom_target(
            format
            COMMAND ${CLANG_FORMAT_EXE} -style=file -i ${ALL_SOURCE_FILES}
            COMMENT "Running clang-format to format all source files"
            VERBATIM
        )
        
        # Make formatting run before building the main target
        add_dependencies(vv_estimator format)
        
        message(STATUS "Code formatting will be automatically applied during build")
    else()
        message(WARNING "clang-format requested but executable not found")
    endif()
endif() 