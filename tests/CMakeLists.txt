# 테스트에 필요한 Google Test 찾기
find_package(GTest QUIET)

# 테스트 활성화
enable_testing()

if(NOT GTEST_FOUND)
    # GTest를 찾을 수 없는 경우 GitHub에서 가져오기
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0  # 최신 버전으로 업데이트
    )
    # CMake 3.14+ 이상에서 사용 가능
    FetchContent_MakeAvailable(googletest)
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
        target_link_libraries(${test_name} PRIVATE ${GTEST_BOTH_LIBRARIES})
    else()
        target_link_libraries(${test_name} PRIVATE gtest gtest_main)
    endif()
    
    # OpenCV 라이브러리 링크
    target_link_libraries(${test_name} PRIVATE ${OpenCV_LIBS})
    
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