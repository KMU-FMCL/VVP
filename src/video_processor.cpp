#include "VisualVerticalEstimator/video_processor.h"
#include "VisualVerticalEstimator/constants.h"
#include "VisualVerticalEstimator/utils.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

namespace vve {

VideoProcessor::VideoProcessor(
    HOGCalculator& hogCalculator,
    OrientationAnalyzer& orientationAnalyzer,
    Visualizer& visualizer,
    int scaleFactor
) : hogCalculator(hogCalculator),
    orientationAnalyzer(orientationAnalyzer),
    visualizer(visualizer),
    scaleFactor(scaleFactor),
    fps(0.0) {
}

bool VideoProcessor::processVideoFile(const std::string& inputFile) {
    Logger::info("비디오 파일 처리 시작: " + inputFile);
    
    // 비디오 캡처 초기화
    cv::VideoCapture cap = initializeCapture(inputFile, false);
    if (!cap.isOpened()) {
        Logger::error("비디오 파일을 열 수 없습니다: " + inputFile);
        return false;
    }
    
    // 출력 파일 경로 생성
    std::string outputFile = "results/output_" + getCurrentTimeString(ISOTIMEFORMAT) + ".mp4";
    
    // 비디오 작성기 초기화
    cv::VideoWriter writer = initializeWriter(cap, outputFile);
    if (!writer.isOpened()) {
        Logger::error("출력 비디오 파일을 생성할 수 없습니다: " + outputFile);
        return false;
    }
    
    // 성능 측정 타이머
    PerformanceTimer timer;
    
    // 프레임 처리 루프
    cv::Mat frame;
    while (cap.read(frame)) {
        if (frame.empty()) {
            Logger::warning("빈 프레임을 받았습니다. 종료합니다.");
            break;
        }
        
        // 프레임 처리 시간 측정 시작
        timer.start();
        
        // 프레임 처리
        cv::Mat processedFrame = processFrame(frame);
        
        // 처리 시간 측정 종료 및 FPS 계산
        calculateFPS(timer.stop());
        
        // 결과 저장
        writer.write(processedFrame);
        
        // 결과 표시
        cv::imshow("Visual Vertical Estimator", processedFrame);
        
        // 키 입력 처리
        if (!handleKeyInput(cv::waitKey(1))) {
            Logger::info("사용자가 처리를 중단했습니다.");
            break;
        }
    }
    
    // 리소스 해제
    cap.release();
    writer.release();
    cv::destroyAllWindows();
    
    Logger::info("비디오 파일 처리 완료: " + inputFile);
    Logger::info("출력 파일 저장됨: " + outputFile);
    
    return true;
}

bool VideoProcessor::processCamera(int cameraIndex) {
    Logger::info("카메라 입력 처리 시작 (인덱스: " + std::to_string(cameraIndex) + ")");
    
    // 카메라 캡처 초기화
    cv::VideoCapture cap = initializeCapture(std::to_string(cameraIndex), true);
    if (!cap.isOpened()) {
        Logger::error("카메라를 열 수 없습니다 (인덱스: " + std::to_string(cameraIndex) + ")");
        return false;
    }
    
    // 출력 파일 경로 생성
    std::string outputFile = "results/camera_" + getCurrentTimeString(ISOTIMEFORMAT) + ".mp4";
    
    // 비디오 작성기 초기화
    cv::VideoWriter writer = initializeWriter(cap, outputFile);
    if (!writer.isOpened()) {
        Logger::error("출력 비디오 파일을 생성할 수 없습니다: " + outputFile);
        return false;
    }
    
    // 성능 측정 타이머
    PerformanceTimer timer;
    
    // 프레임 처리 루프
    cv::Mat frame;
    while (cap.read(frame)) {
        if (frame.empty()) {
            Logger::warning("빈 프레임을 받았습니다. 재시도합니다.");
            continue;
        }
        
        // 프레임 처리 시간 측정 시작
        timer.start();
        
        // 프레임 처리
        cv::Mat processedFrame = processFrame(frame);
        
        // 처리 시간 측정 종료 및 FPS 계산
        calculateFPS(timer.stop());
        
        // 결과 저장
        writer.write(processedFrame);
        
        // 결과 표시
        cv::imshow("Visual Vertical Estimator", processedFrame);
        
        // 키 입력 처리
        if (!handleKeyInput(cv::waitKey(1))) {
            Logger::info("사용자가 처리를 중단했습니다.");
            break;
        }
    }
    
    // 리소스 해제
    cap.release();
    writer.release();
    cv::destroyAllWindows();
    
    Logger::info("카메라 입력 처리 완료");
    Logger::info("출력 파일 저장됨: " + outputFile);
    
    return true;
}

cv::VideoCapture VideoProcessor::initializeCapture(const std::string& source, bool isCamera) {
    cv::VideoCapture cap;
    
    if (isCamera) {
        int cameraIndex = std::stoi(source);
        cap.open(cameraIndex);
        
        // 카메라 속성 설정
        if (cap.isOpened()) {
            cap.set(cv::CAP_PROP_FRAME_WIDTH, 1280);
            cap.set(cv::CAP_PROP_FRAME_HEIGHT, 720);
            cap.set(cv::CAP_PROP_FPS, 30);
        }
    } else {
        cap.open(source);
    }
    
    return cap;
}

cv::VideoWriter VideoProcessor::initializeWriter(const cv::VideoCapture& cap, const std::string& outputFile) {
    // 원본 비디오 속성 가져오기
    int width = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH));
    int height = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT));
    double fps = cap.get(cv::CAP_PROP_FPS);
    
    // 크기 조정 적용
    width /= scaleFactor;
    height /= scaleFactor;
    
    // 코덱 설정 (MP4 파일에 H.264 코덱 사용)
    int fourcc = cv::VideoWriter::fourcc('H', '2', '6', '4');
    
    return cv::VideoWriter(outputFile, fourcc, fps, cv::Size(width, height));
}

cv::Mat VideoProcessor::processFrame(const cv::Mat& frame) {
    // 프레임 크기 조정
    cv::Mat resized = resizeImage(frame, scaleFactor);
    
    // HOG 특징 계산
    HOGCalculator::HOGFeatures hogFeatures = hogCalculator.calculate(resized);
    
    // 주요 방향 계산
    double orientation = orientationAnalyzer.calculateDominantOrientation(hogFeatures.histogram);
    
    // 결과 시각화
    cv::Mat result = visualizer.visualizeResults(
        resized, orientation, hogFeatures.histogram, hogFeatures.filteredMagnitude);
    
    return result;
}

bool VideoProcessor::handleKeyInput(int key) {
    // ESC 키 또는 'q' 키를 누르면 종료
    if (key == 27 || key == 'q' || key == 'Q') {
        return false;
    }
    
    return true;
}

void VideoProcessor::calculateFPS(double elapsedMs) {
    // 이동 평균으로 FPS 계산
    if (elapsedMs > 0) {
        double currentFPS = 1000.0 / elapsedMs;
        fps = fps == 0.0 ? currentFPS : fps * 0.9 + currentFPS * 0.1;
    }
}

} // namespace vve