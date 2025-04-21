# 소스 파일 추가
set(SOURCES
    main.cpp
    io/IOHandler.cpp
    processing/ImageProcessor.cpp
    estimation/VVEstimator.cpp
    estimation/Types.cpp
    utils/Helpers.cpp
    fps/FPSCounter.cpp
)

# 실행 파일 빌드
add_executable(vv_estimator ${SOURCES})

# 라이브러리 연결
target_link_libraries(vv_estimator ${OpenCV_LIBS})

# 컴파일 옵션 추가
target_compile_options(vv_estimator PRIVATE 
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
) 