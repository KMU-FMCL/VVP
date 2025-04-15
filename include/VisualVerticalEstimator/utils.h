#pragma once

#include <string>
#include <chrono>
#include <iostream>
#include <opencv2/core.hpp>

namespace vve {

/**
 * @brief 로깅 레벨 열거형
 */
enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR
};

/**
 * @brief 로깅 유틸리티 클래스
 */
class Logger {
public:
    /**
     * @brief 로거 초기화
     * @param level 로깅 레벨
     */
    static void init(LogLevel level = LogLevel::INFO);

    /**
     * @brief 디버그 로그 출력
     * @param message 로그 메시지
     */
    static void debug(const std::string& message);

    /**
     * @brief 정보 로그 출력
     * @param message 로그 메시지
     */
    static void info(const std::string& message);

    /**
     * @brief 경고 로그 출력
     * @param message 로그 메시지
     */
    static void warning(const std::string& message);

    /**
     * @brief 에러 로그 출력
     * @param message 로그 메시지
     */
    static void error(const std::string& message);

private:
    static LogLevel currentLevel;
    static void log(LogLevel level, const std::string& message);
};

/**
 * @brief 현재 시간을 지정된 포맷의 문자열로 반환
 * @param format 시간 포맷 문자열
 * @return 포맷된 시간 문자열
 */
std::string getCurrentTimeString(const std::string& format);

/**
 * @brief OpenCL 지원 여부 확인 및 설정
 * @return OpenCL 사용 가능 여부
 */
bool setupOpenCL();

/**
 * @brief 이미지 크기 조정
 * @param image 원본 이미지
 * @param scaleFactor 크기 조정 계수
 * @return 크기가 조정된 이미지
 */
cv::Mat resizeImage(const cv::Mat& image, int scaleFactor);

/**
 * @brief 성능 측정 유틸리티 클래스
 */
class PerformanceTimer {
public:
    /**
     * @brief 타이머 시작
     */
    void start();

    /**
     * @brief 타이머 중지 및 경과 시간 반환
     * @return 경과 시간(밀리초)
     */
    double stop();

    /**
     * @brief 현재 경과 시간 반환
     * @return 경과 시간(밀리초)
     */
    double elapsed() const;

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
    bool isRunning = false;
};

} // namespace vve