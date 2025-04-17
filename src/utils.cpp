#include "VisualVerticalEstimator/utils.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <opencv2/core/ocl.hpp>
#include <opencv2/imgproc.hpp>

namespace vve {

// Logger 클래스 정적 멤버 초기화
LogLevel Logger::currentLevel = LogLevel::INFO;

void Logger::init(LogLevel level) {
    currentLevel = level;
}

void Logger::debug(const std::string& message) {
    log(LogLevel::DEBUG, message);
}

void Logger::info(const std::string& message) {
    log(LogLevel::INFO, message);
}

void Logger::warning(const std::string& message) {
    log(LogLevel::WARNING, message);
}

void Logger::error(const std::string& message) {
    log(LogLevel::ERROR, message);
}

void Logger::log(LogLevel level, const std::string& message) {
    if (level < currentLevel) {
        return;
    }

    std::string levelStr;
    switch (level) {
        case LogLevel::DEBUG:
            levelStr = "DEBUG";
            break;
        case LogLevel::INFO:
            levelStr = "INFO";
            break;
        case LogLevel::WARNING:
            levelStr = "WARNING";
            break;
        case LogLevel::ERROR:
            levelStr = "ERROR";
            break;
    }

    std::cout << getCurrentTimeString("%Y-%m-%d %H:%M:%S") << " - " 
              << levelStr << " - " << message << std::endl;
}

std::string getCurrentTimeString(const std::string& format) {
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t_now), format.c_str());
    return ss.str();
}

bool setupOpenCL() {
    bool success = false;
    
    try {
        cv::ocl::setUseOpenCL(false);  // 기본적으로 비활성화
        
        if (cv::ocl::haveOpenCL()) {
            cv::ocl::Context context;
            if (context.create(cv::ocl::Device::TYPE_ALL)) {
                cv::ocl::Device device = context.device(0);
                std::string deviceName = device.name();
                std::string vendorName = device.vendorName();
                
                Logger::info("OpenCL 장치 발견: " + deviceName + " (" + vendorName + ")");
                cv::ocl::setUseOpenCL(true);
                Logger::info("OpenCL 활성화됨: " + std::string(cv::ocl::useOpenCL() ? "true" : "false"));
                success = true;
            }
        } else {
            Logger::warning("OpenCL을 사용할 수 없거나 지원되지 않습니다.");
        }
    } catch (const cv::Exception& e) {
        Logger::warning("OpenCL 초기화 실패: " + std::string(e.what()));
    }
    
    return success;
}

cv::Mat resizeImage(const cv::Mat& image, int scaleFactor) {
    if (scaleFactor <= 0) {
        Logger::warning("크기 조정 계수는 양수여야 합니다. 기본값 1 사용.");
        scaleFactor = 1;
    }
    
    if (scaleFactor == 1) {
        return image.clone();
    }
    
    cv::Mat resized;
    cv::resize(image, resized, cv::Size(image.cols / scaleFactor, image.rows / scaleFactor), 
               0, 0, cv::INTER_AREA);
    return resized;
}

void PerformanceTimer::start() {
    startTime = std::chrono::high_resolution_clock::now();
    isRunning = true;
}

double PerformanceTimer::stop() {
    if (!isRunning) {
        return 0.0;
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    isRunning = false;
    
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    return duration.count() / 1000.0;  // 밀리초 단위로 반환
}

double PerformanceTimer::elapsed() const {
    if (!isRunning) {
        return 0.0;
    }
    
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - startTime);
    return duration.count() / 1000.0;  // 밀리초 단위로 반환
}

} // namespace vve