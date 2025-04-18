cmake_minimum_required(VERSION 3.10)
project(VisualVerticalEstimator VERSION 1.0.0 LANGUAGES CXX)

# C++17 표준 사용
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 빌드 타입이 설정되지 않은 경우 Release로 설정
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# OpenCV 찾기
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# 프로젝트 헤더 파일 포함
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# 하위 디렉토리 추가
add_subdirectory(src)

# 테스트 빌드 옵션 (기본값: OFF)
option(BUILD_TESTS "Build tests" OFF)
if(BUILD_TESTS)
    add_subdirectory(tests)
endif()

# 설치 디렉토리 설정
set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/install)

# 실행 파일 설치
install(TARGETS vv_estimator DESTINATION bin) 