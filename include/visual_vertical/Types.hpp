#pragma once

#include <string>
#include <vector>
#include <opencv2/core.hpp>

namespace vv {

// 시간 형식 상수
const std::string ISO_TIME_FORMAT = "%Y%m%d_%H%M%S";

// 프로그램 설정 구조체
struct Config {
    bool useCamera = false;
    int cameraPort = 0;
    std::string inputFilePath = "./test.mp4";
    int scale = 2;
    bool saveResults = true;
};

// HOG 파라미터 구조체
struct HOGParams {
    int binCount = 180;
    double thresholdValue = 0.25;
    int blurKernelSize = 11;
    double blurSigma = 3.0;
    int erodeKernelSize = 3;
};

// VV 추정 결과 구조체
struct VVResult {
    double angle = 90.0;          // 수직 방향 각도 (도)
    double angleRad = M_PI / 2.0; // 수직 방향 각도 (라디안)
    double accX = 0.0;            // X방향 가속도 (m/s^2)
    double accY = 9.8;            // Y방향 가속도 (m/s^2)
    
    // 각도에서 가속도 계산 메서드
    void updateAcceleration() {
        angleRad = angle * M_PI / 180.0;
        accX = 9.8 * std::cos(angleRad);
        accY = 9.8 * std::sin(angleRad);
    }
};

// HOG 계산 결과 구조체
struct HOGResult {
    cv::Mat gradientX;
    cv::Mat gradientY;
    std::vector<float> histogram;
    cv::Mat magnitude;
    cv::Mat magnitudeFiltered;
};

} // namespace vv 