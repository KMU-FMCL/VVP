# 소스 파일 추가
set(SOURCES
    main.cpp
    io/IOHandler.cpp
    processing/ImageProcessor.cpp
    estimation/VVEstimator.cpp
    estimation/Types.cpp
    utils/Helpers.cpp
    utils/ConfigLoader.cpp
    fps/FPSCounter.cpp
)

# 실행 파일 빌드
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

# 라이브러리 연결
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
)

# 컴파일 옵션 추가
target_compile_options(vv_estimator PRIVATE 
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
) 