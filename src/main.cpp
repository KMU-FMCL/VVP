#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "visual_vertical/ImageProcessor.hpp"
#include "visual_vertical/VVEstimator.hpp"
#include "visual_vertical/IOHandler.hpp"
#include "visual_vertical/utils/Helpers.hpp"
#include "visual_vertical/fps/FPSCounter.hpp"

int main(int argc, char* argv[]) {
    // OpenCV 정보 출력
    vv::utils::printOpenCVInfo();
    
    // 명령줄 인자 파싱
    vv::Config config = vv::utils::parseCommandLineArgs(argc, argv);
    
    // 입출력 핸들러 초기화
    vv::IOHandler ioHandler(config);
    if (!ioHandler.openVideoSource()) {
        std::cerr << "Error: Could not open video source." << std::endl;
        return 1;
    }
    
    // 이미지 처리기 및 VV 추정기 초기화
    vv::ImageProcessor imageProcessor;
    vv::VVEstimator vvEstimator;
    
    // FPS 카운터 초기화
    vv::FPSCounter fpsCounter;
    
    // 첫 프레임 읽기 및 비디오 출력 설정
    cv::Mat frame;
    if (!ioHandler.readNextFrame(frame)) {
        std::cerr << "Error: Could not read first frame." << std::endl;
        return 1;
    }
    
    // 이미지 크기 조정
    frame = imageProcessor.resizeImage(frame, config.scale);
    
    // 비디오 작성기 설정
    int originalWidth = frame.cols;
    int originalHeight = frame.rows;
    
    // 결과 비디오 크기 계산 (원본 이미지 * 2 + 히스토그램)
    int resultWidth = originalWidth * 2;
    int resultHeight = static_cast<int>(originalHeight * 2.6);
    
    if (!ioHandler.setupVideoWriter(resultWidth, resultHeight)) {
        std::cerr << "Warning: Could not setup video writer." << std::endl;
    }
    
    // 이전 VV 결과 초기화
    vv::VVResult previousResult;
    
    // 메인 처리 루프
    while (true) {
        // FPS 측정 시작
        fpsCounter.tickStart();
        
        // 프레임 읽기
        if (!ioHandler.readNextFrame(frame)) {
            break;
        }
        
        // 이미지 크기 조정
        frame = imageProcessor.resizeImage(frame, config.scale);
        
        // HOG 계산
        vv::HOGResult hogResult = imageProcessor.computeHOG(frame);
        
        // VV 추정
        vv::VVResult vvResult = vvEstimator.estimateVV(hogResult.histogram, previousResult);
        previousResult = vvResult;
        
        // 이미지 회전 (보정)
        cv::Mat calibratedImage = imageProcessor.rotateImage(frame, 90 - vvResult.angle);
        
        // 히스토그램 시각화 생성
        cv::Mat histogramImage = vvEstimator.createHistogramVisualization(
            hogResult.histogram,
            vvResult,
            resultWidth,
            originalHeight * 0.6
        );
        
        // 시각화 이미지 생성
        cv::Mat visualizationResult = imageProcessor.createVisualization(
            frame,
            calibratedImage,
            hogResult,
            vvResult,
            histogramImage,
            fpsCounter.getFPS() // FPS 정보 전달
        );
        
        // 결과 표시 및 저장
        int key = ioHandler.displayFrame(visualizationResult);
        ioHandler.writeFrame(visualizationResult);
        
        // FPS 측정 종료
        fpsCounter.tickEnd();
        
        // ESC 키가 눌리면 종료
        if (key == 27) {
            break;
        }
    }
    
    // 결과 CSV 저장
    if (config.saveResults) {
        ioHandler.saveResultsToCSV(vvEstimator.getAllResults());
    }
    
    // 평균 FPS 출력
    std::cout << "Average FPS: " << fpsCounter.getAverageFPS() << std::endl;
    std::cout << "Total frames processed: " << fpsCounter.getFrameCount() << std::endl;
    std::cout << "Total processing time: " << fpsCounter.getTotalProcessingTimeSec() << " seconds" << std::endl;
    
    std::cout << "Processing complete." << std::endl;
    return 0;
} 