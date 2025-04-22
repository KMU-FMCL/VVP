# 테스트에 필요한 Google Test 찾기
find_package(GTest QUIET)

# 테스트 활성화
enable_testing()

if(NOT GTEST_FOUND)
    # 시스템 GTest 패키지가 없으면 세 가지 방법 중 하나를 시도
    # 1) 테스트 디렉토리의 googletest 서브모듈 사용
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/googletest/CMakeLists.txt)
        add_subdirectory(
            ${CMAKE_CURRENT_SOURCE_DIR}/googletest
            ${CMAKE_CURRENT_BINARY_DIR}/googletest
        )
    else()
        # 2) FetchContent를 이용해 원격에서 GoogleTest 다운로드
        include(FetchContent)
        FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG v1.14.0
        )
        FetchContent_MakeAvailable(googletest)
    endif()
endif()


# 구현 파일 목록
set(IMPLEMENTATION_SOURCES
    ${CMAKE_SOURCE_DIR}/src/estimation/VVEstimator.cpp
    ${CMAKE_SOURCE_DIR}/src/processing/ImageProcessor.cpp
    ${CMAKE_SOURCE_DIR}/src/io/IOHandler.cpp
    ${CMAKE_SOURCE_DIR}/src/estimation/Types.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/Helpers.cpp
)

# 테스트 소스 파일 목록
set(TEST_SOURCES
    test_vv_estimator.cpp
    test_image_processor.cpp
)

# 테스트 타겟 목록 저장
set(TEST_TARGETS)

# 각 테스트 대상 생성
foreach(test_source ${TEST_SOURCES})
    get_filename_component(test_name ${test_source} NAME_WE)
    add_executable(${test_name} ${test_source} ${IMPLEMENTATION_SOURCES})
    list(APPEND TEST_TARGETS ${test_name})
    
    if(GTEST_FOUND)
        # 시스템에서 찾은 GTest 헤더 및 라이브러리 사용
        target_include_directories(${test_name} PRIVATE ${GTEST_INCLUDE_DIRS})
        target_link_libraries(${test_name} PRIVATE ${GTEST_BOTH_LIBRARIES})
    else()
        target_link_libraries(${test_name} PRIVATE gtest gtest_main)
    endif()

    # 프로젝트 루트 경로 정의 (IOHandler 등에서 사용)
    target_compile_definitions(${test_name} PRIVATE PROJECT_ROOT="${CMAKE_SOURCE_DIR}")
    
    # OpenCV 라이브러리 링크
    target_link_libraries(${test_name} PRIVATE ${OpenCV_LIBS})
    
    # Abseil 라이브러리 링크
    target_link_libraries(${test_name} PRIVATE
        absl::strings
        absl::str_format
        absl::time
        absl::span
        absl::flat_hash_map
        absl::flat_hash_set
    )
    
    # 테스트에 사용되는 헤더 파일 디렉토리 추가
    target_include_directories(${test_name} PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
    )
    
    # Google Test를 사용하는 경우, 스레드 라이브러리 필요
    find_package(Threads REQUIRED)
    target_link_libraries(${test_name} PRIVATE Threads::Threads)
    
    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()

# 테스트 실행 대상 추가
add_custom_target(run_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS ${TEST_TARGETS}
) 