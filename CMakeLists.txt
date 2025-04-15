cmake_minimum_required(VERSION 3.10)
project(VisualVerticalEstimator VERSION 1.0.0 LANGUAGES CXX)

# C++17 표준 사용
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 빌드 타입이 지정되지 않았을 경우 Release로 설정
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# 컴파일러 경고 및 최적화 옵션
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
elseif(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2")
endif()

# OpenCV 찾기
find_package(OpenCV REQUIRED)
if(NOT OpenCV_FOUND)
    message(FATAL_ERROR "OpenCV not found. Please install OpenCV first.")
endif()

# 헤더 파일 디렉토리 설정
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${OpenCV_INCLUDE_DIRS}
)

# 실행 파일 생성을 위한 소스 파일 목록
set(SOURCES
    src/main.cpp
    src/video_processor.cpp
    src/hog_calculator.cpp
    src/orientation.cpp
    src/visualization.cpp
    src/utils.cpp
)

# 실행 파일 생성
add_executable(${PROJECT_NAME} ${SOURCES})

# 라이브러리 링크
target_link_libraries(${PROJECT_NAME} PRIVATE ${OpenCV_LIBS})

# 설치 설정
install(TARGETS ${PROJECT_NAME} DESTINATION bin)

# 데이터 및 결과 디렉토리 생성
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/data)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/results)

# 빌드 디렉토리에 테스트 비디오 복사 (있을 경우)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../test.mp4")
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/../test.mp4" DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/data")
endif()

# 출력 메시지
message(STATUS "Project: ${PROJECT_NAME} v${PROJECT_VERSION}")
message(STATUS "OpenCV version: ${OpenCV_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# src 디렉토리의 CMakeLists.txt 포함
add_subdirectory(src)