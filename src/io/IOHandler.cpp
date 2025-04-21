#include "vvp/io/IOHandler.hpp"
#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <filesystem> // C++17 파일 시스템 기능 추가
#include "vvp/utils/Helpers.hpp" // 추가 (getCurrentDateString 사용 위해)

namespace vv {

IOHandler::IOHandler(const Config& config)
    : m_config(config) {
    
    // Resolve relative input path against project root
    if (!m_config.useCamera) {
        std::filesystem::path inPath(m_config.inputFilePath);
        if (!inPath.is_absolute()) {
            inPath = std::filesystem::path(PROJECT_ROOT) / inPath;
            m_config.inputFilePath = inPath.string();
        }
    }
    // 현재 날짜 및 전체 타임스탬프 가져오기
    std::string currentDate = utils::getCurrentDateString(); // "YYYYMMDD"
    std::string currentTimestamp = getCurrentTimeStamp(); // "YYYYMMDD_HHMMSS"
    // 타임스탬프에서 시간 부분(HHMMSS)만 추출
    std::string timePart = extractTimePart(currentTimestamp);

    // 날짜 기반 결과 디렉토리 경로 설정 (project root/results)
    std::filesystem::path baseResultDir = std::filesystem::path(PROJECT_ROOT) / "results";
    std::filesystem::path dateResultDir = baseResultDir / currentDate; // 예: ../results/20230417

    // 날짜 기반 결과 하위 디렉토리 생성 (존재하지 않을 경우)
    // 날짜 기반 결과 디렉토리 생성
    ensureDirectoryExists(dateResultDir);
    std::cout << "Results will be saved to directory: " << std::filesystem::absolute(dateResultDir).string() << std::endl;

    // 비디오, CSV 파일 경로 초기화 (파일 이름에 시간 포함)
    if (m_config.useCamera) {
        // 카메라 입력의 경우, 파일 이름에 'camera'와 시간만 포함
        std::string csvFileName = "camera_" + timePart + ".csv";
        std::string videoFileName = "camera_" + timePart + ".mp4";
        m_csvFilePath = (dateResultDir / csvFileName).string();
        m_videoFilePath = (dateResultDir / videoFileName).string();
    } else {
        // 파일 경로 처리에 std::filesystem 사용
        std::filesystem::path inputPath(m_config.inputFilePath);
        
        // stem()은 확장자를 제외한 파일 이름만 가져옵니다
        std::string filenameWithoutExt = inputPath.stem().string();
            
        // 최종 파일 이름 구성: VV_ + 원본이름 + _ + 시간 + 확장자
        std::string csvFileName = "VV_" + filenameWithoutExt + "_" + timePart + ".csv";
        std::string videoFileName = "VV_Video_" + filenameWithoutExt + "_" + timePart + ".mp4";
        m_csvFilePath = (dateResultDir / csvFileName).string();
        m_videoFilePath = (dateResultDir / videoFileName).string();
    }
}

IOHandler::~IOHandler() {
    // 리소스 정리
    if (m_videoCapture.isOpened()) {
        m_videoCapture.release();
    }
    
    if (m_videoWriter.isOpened()) {
        m_videoWriter.release();
    }
    
    cv::destroyAllWindows();
}

bool IOHandler::openVideoSource() {
    if (m_config.useCamera) {
        m_videoCapture.open(m_config.cameraPort, cv::CAP_DSHOW);
    } else {
        m_videoCapture.open(m_config.inputFilePath);
    }
    
    if (!m_videoCapture.isOpened()) {
        std::cerr << "Error: Could not open video source: " 
                 << (m_config.useCamera ? "Camera #" + std::to_string(m_config.cameraPort) 
                                        : m_config.inputFilePath)
                 << std::endl;
        return false;
    }
    
    return true;
}

bool IOHandler::readNextFrame(cv::Mat& frame) {
    if (!m_videoCapture.isOpened()) {
        return false;
    }
    
    return m_videoCapture.read(frame);
}

bool IOHandler::setupVideoWriter(int width, int height) {
    if (!m_videoCapture.isOpened()) {
        return false;
    }
    
    // 비디오 코덱 및 프레임 레이트 설정
    int fourcc = cv::VideoWriter::fourcc('m', 'p', '4', 'v');
    double fps = m_videoCapture.get(cv::CAP_PROP_FPS);
    
    if (fps <= 0) {
        fps = 30.0;  // 기본값 설정
    }
    
    // VideoWriter 열기 전에 디렉토리 존재 여부 재확인
    // 비디오 저장 디렉토리 생성
    std::filesystem::path videoPath(m_videoFilePath);
    ensureDirectoryExists(videoPath.parent_path());
    
    m_videoWriter.open(m_videoFilePath, fourcc, fps, cv::Size(width, height));
    
    if (!m_videoWriter.isOpened()) {
        std::cerr << "Error: Could not create video writer for: " << m_videoFilePath << std::endl;
        return false;
    }
    
    std::cout << "Video will be saved to: " << m_videoFilePath << std::endl;
    return true;
}

void IOHandler::writeFrame(const cv::Mat& frame) {
    if (m_videoWriter.isOpened()) {
        m_videoWriter.write(frame);
    }
}

int IOHandler::displayFrame(const cv::Mat& frame, int waitKey) {
    cv::imshow("Visual Vertical Estimation", frame);
    return cv::waitKey(waitKey);
}

bool IOHandler::saveResultsToCSV(const std::vector<VVResult>& results) {
    if (results.empty()) {
        std::cerr << "Error: No results to save." << std::endl;
        return false;
    }
    
    // CSV 파일 열기 전에 디렉토리 존재 여부 재확인
    // CSV 저장 디렉토리 생성
    std::filesystem::path csvPath(m_csvFilePath);
    ensureDirectoryExists(csvPath.parent_path());
    
    std::ofstream outFile(m_csvFilePath);
    if (!outFile.is_open()) {
        std::cerr << "Error: Could not open file for writing: " << m_csvFilePath << std::endl;
        return false;
    }
    
    // CSV 헤더 작성
    outFile << "VV_acc_x[m/s^2],VV_acc_y[m/s^2],VV_acc_rad,VV_acc_dig" << std::endl;
    
    // 데이터 작성
    for (const auto& result : results) {
        outFile << result.accX << ","
                << result.accY << ","
                << result.angleRad << ","
                << result.angle << std::endl;
    }
    
    outFile.close();
    std::cout << "Results saved to: " << m_csvFilePath << std::endl;
    
    return true;
}

cv::VideoCapture& IOHandler::getVideoCapture() {
    return m_videoCapture;
}

std::string IOHandler::generateOutputFilePath(const std::string& prefix, const std::string& extension) const {
    // 현재 날짜 기준 디렉토리와 시간을 포함한 파일 이름 생성
    std::string currentDate = utils::getCurrentDateString();
    std::filesystem::path baseResultDir = "../results";
    std::filesystem::path dateResultDir = baseResultDir / currentDate; // baseResultDir 객체에 / 연산자 사용
    std::string timestamp = getCurrentTimeStamp(); 
    
    // 타임스탬프에서 시간 부분 추출
    std::string timePart = "000000"; // 기본값
    size_t underscorePos = timestamp.find('_');
    if (underscorePos != std::string::npos && underscorePos + 1 < timestamp.length()) {
        timePart = timestamp.substr(underscorePos + 1); // "HHMMSS" 추출
    }
    
    return (dateResultDir / (prefix + "_" + timePart + extension)).string();
}

std::string IOHandler::getCurrentTimeStamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), ISO_TIME_FORMAT.c_str());
    
    return ss.str();
}

} // namespace vv 

//------------------------------------------------------------------------------
// Helper implementations
//------------------------------------------------------------------------------
namespace vv {

std::string IOHandler::extractTimePart(const std::string& timestamp) {
    size_t pos = timestamp.find('_');
    if (pos != std::string::npos && pos + 1 < timestamp.size()) {
        return timestamp.substr(pos + 1);
    }
    return "000000";
}

void IOHandler::ensureDirectoryExists(const std::filesystem::path& dir) {
    try {
        std::filesystem::create_directories(dir);
    } catch (const std::exception& e) {
        std::cerr << "Warning: Could not create directory: " << dir << " (" << e.what() << ")" << std::endl;
    }
}
} // namespace vv