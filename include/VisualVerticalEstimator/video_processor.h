#pragma once

#include <string>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include "VisualVerticalEstimator/hog_calculator.h"
#include "VisualVerticalEstimator/orientation.h"
#include "VisualVerticalEstimator/visualization.h"

namespace vve {

/**
 * @brief 비디오 처리 클래스
 */
class VideoProcessor {
public:
    /**
     * @brief 생성자
     * @param hogCalculator HOG 계산기
     * @param orientationAnalyzer 방향성 분석기
     * @param visualizer 시각화 도구
     * @param scaleFactor 입력 프레임 크기 조정 계수
     */
    VideoProcessor(
        HOGCalculator& hogCalculator,
        OrientationAnalyzer& orientationAnalyzer,
        Visualizer& visualizer,
        int scaleFactor = 2
    );

    /**
     * @brief 소멸자
     */
    virtual ~VideoProcessor() = default;

    /**
     * @brief 비디오 파일 처리
     * @param inputFile 입력 비디오 파일 경로
     * @return 처리 성공 여부
     */
    bool processVideoFile(const std::string& inputFile);

    /**
     * @brief 카메라 입력 처리
     * @param cameraIndex 카메라 인덱스
     * @return 처리 성공 여부
     */
    bool processCamera(int cameraIndex = 0);

    /**
     * @brief 크기 조정 계수 설정
     * @param factor 크기 조정 계수
     */
    void setScaleFactor(int factor);

    /**
     * @brief 현재 FPS 값 가져오기
     * @return 현재 FPS 값
     */
    double getFPS() const;

private:
    HOGCalculator& hogCalculator;
    OrientationAnalyzer& orientationAnalyzer;
    Visualizer& visualizer;
    int scaleFactor;
    double fps;

    /**
     * @brief 비디오 캡처 초기화
     * @param source 입력 소스 (파일 경로 또는 카메라 인덱스)
     * @param isCamera 카메라 입력 여부
     * @return 초기화된 VideoCapture 객체
     */
    cv::VideoCapture initializeCapture(const std::string& source, bool isCamera);

    /**
     * @brief 비디오 작성기 초기화
     * @param cap 비디오 캡처 객체
     * @param outputFile 출력 파일 경로
     * @return 초기화된 VideoWriter 객체
     */
    cv::VideoWriter initializeWriter(const cv::VideoCapture& cap, const std::string& outputFile);

    /**
     * @brief 단일 프레임 처리
     * @param frame 입력 프레임
     * @return 처리된 프레임
     */
    cv::Mat processFrame(const cv::Mat& frame);

    /**
     * @brief 키 입력 처리
     * @param key 키 코드
     * @return 처리 계속 여부
     */
    bool handleKeyInput(int key);

    /**
     * @brief FPS 계산
     * @param elapsedMs 프레임 처리에 소요된 시간 (밀리초)
     */
    void calculateFPS(double elapsedMs);
};

} // namespace vve