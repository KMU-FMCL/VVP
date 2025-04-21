#include "vvp/utils/Helpers.hpp"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <cstring>
#include <opencv2/core.hpp>
#include <opencv2/core/ocl.hpp>

namespace vv {
namespace utils {

Config parseCommandLineArgs(int argc, char* argv[]) {
    Config config;
    
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            printUsage();
            exit(0);
        }
        else if (arg == "-i" || arg == "--inputfile") {
            if (i + 1 < argc) {
                config.inputFilePath = argv[++i];
                config.useCamera = false;
            }
        }
        else if (arg == "-c" || arg == "--camera") {
            if (i + 1 < argc) {
                std::string value = argv[++i];
                if (value == "true" || value == "True" || value == "1") {
                    config.useCamera = true;
                }
            }
        }
        else if (arg == "-cp" || arg == "--camera_port") {
            if (i + 1 < argc) {
                config.cameraPort = std::stoi(argv[++i]);
            }
        }
        else if (arg == "-s" || arg == "--scale") {
            if (i + 1 < argc) {
                config.scale = std::stoi(argv[++i]);
                if (config.scale <= 0) {
                    config.scale = 1;
                }
            }
        }
    }
    
    return config;
}

std::string formatCurrentTime(const std::string& format) {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), format.c_str());
    
    return ss.str();
}

std::string getCurrentDateString() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    // "%Y%m%d" 포맷 사용 - 년월일만 반환
    ss << std::put_time(std::localtime(&time), "%Y%m%d");
    
    return ss.str();
}

void printUsage() {
    std::cout << "Visual Vertical Estimator\n"
              << "-------------------------\n"
              << "Usage:\n"
              << "  vv_estimator -i <inputfile> [options]\n"
              << "  vv_estimator -c true -cp <camera_port> [options]\n\n"
              << "Options:\n"
              << "  -h, --help               Show this help message\n"
              << "  -i, --inputfile <path>   Specify input video file path\n"
              << "  -c, --camera <bool>      Use camera as input source (true/false)\n"
              << "  -cp, --camera_port <n>   Specify camera port number (default: 0)\n"
              << "  -s, --scale <n>          Image scaling factor (default: 2)\n\n"
              << "Examples:\n"
              << "  vv_estimator -i ./test.mp4 --scale 2\n"
              << "  vv_estimator --camera true --camera_port 0 --scale 1\n"
              << std::endl;
}

void printOpenCVInfo() {
    std::cout << "OpenCV Version: " << CV_VERSION << std::endl;
    
    // OpenCL 지원 확인
    std::cout << "OpenCL support: " << (cv::ocl::haveOpenCL() ? "Available" : "Not available") << std::endl;
    
    if (cv::ocl::haveOpenCL()) {
        cv::ocl::setUseOpenCL(true);
        std::cout << "Using OpenCL: " << (cv::ocl::useOpenCL() ? "Yes" : "No") << std::endl;
        
        // OpenCL 장치 정보 출력
        cv::ocl::Device device = cv::ocl::Device::getDefault();
        std::cout << "OpenCL Device: " << device.name() << std::endl;
        std::cout << "Vendor: " << device.vendorName() << std::endl;
    }
}

} // namespace utils
} // namespace vv 