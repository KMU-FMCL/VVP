# src 디렉토리의 CMakeLists.txt

# 헤더 파일 목록
set(HEADERS
    constants.h
    video_processor.h
    hog_calculator.h
    orientation.h
    visualization.h
    utils.h
)

# 소스 파일 목록 (메인 CMakeLists.txt에서 이미 정의했지만 여기서도 명시적으로 정의)
set(SOURCES
    main.cpp
    video_processor.cpp
    hog_calculator.cpp
    orientation.cpp
    visualization.cpp
    utils.cpp
)

# 모든 소스 파일이 헤더 파일을 찾을 수 있도록 설정
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# 소스 그룹 설정 (Visual Studio에서 폴더 구조를 만듦)
source_group("Header Files" FILES ${HEADERS})
source_group("Source Files" FILES ${SOURCES})

# 빌드 시 소스 파일 의존성 설정
foreach(source ${SOURCES})
    set_source_files_properties(
        ${source}
        PROPERTIES
        COMPILE_FLAGS "${CMAKE_CXX_FLAGS}"
    )
endforeach()

# OpenCL 지원 확인 및 설정 (OpenCV가 OpenCL을 지원하는 경우)
if(OpenCV_VERSION VERSION_GREATER_EQUAL "3.0.0")
    find_package(OpenCL QUIET)
    if(OpenCL_FOUND)
        message(STATUS "OpenCL found: ${OpenCL_VERSION_STRING}")
        target_include_directories(${PROJECT_NAME} PRIVATE ${OpenCL_INCLUDE_DIRS})
        target_link_libraries(${PROJECT_NAME} PRIVATE ${OpenCL_LIBRARIES})
        add_definitions(-DUSE_OPENCL)
    else()
        message(STATUS "OpenCL not found. Building without OpenCL support.")
    endif()
endif()