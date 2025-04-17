#include "VisualVerticalEstimator/constants.h"
#include "VisualVerticalEstimator/utils.h"
#include "VisualVerticalEstimator/hog_calculator.h"
#include "VisualVerticalEstimator/orientation.h"
#include "VisualVerticalEstimator/visualization.h"
#include "VisualVerticalEstimator/video_processor.h"
#include <iostream>
#include <string>
#include <memory>
#include <opencv2/core.hpp>

// CLI 인자 파싱을 위한 간단한 구조체
struct CommandLineArgs {
    std::string inputFile;
    bool useCamera = false;
    int cameraPort = 0;
    int scaleFactor = vve::DEFAULT_SCALE_FACTOR;
};

// 도움말 출력 함수
void printHelp(const char* programName) {
    std::cout << "사용법: " << programName << " [옵션]\n\n"
              << "옵션:\n"
              << "  -i, --inputfile <파일>   입력 비디오 파일 경로\n"
              << "  -c, --camera             카메라 입력 사용\n"
              << "  -cp, --camera_port <번호> 카메라 포트 번호 (기본값: 0)\n"
              << "  -s, --scale <계수>        입력 프레임 크기 조정 계수 (기본값: "
              << vve::DEFAULT_SCALE_FACTOR << ")\n"
              << "  -h, --help               이 도움말 메시지 출력\n\n"
              << "예시:\n"
              << "  비디오 파일 처리:\n"
              << "    " << programName << " -i ./test.mp4 -s 2\n\n"
              << "  카메라 입력 사용 (포트 0):\n"
              << "    " << programName << " --camera --camera_port 0 -s 1\n"
              << std::endl;
}

// 명령줄 인자 파싱 함수
CommandLineArgs parseCommandLine(int argc, char* argv[]) {
    CommandLineArgs args;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "-i" || arg == "--inputfile") {
            if (i + 1 < argc) {
                args.inputFile = argv[++i];
            } else {
                throw std::runtime_error("--inputfile 옵션에 파일 경로가 필요합니다.");
            }
        } else if (arg == "-c" || arg == "--camera") {
            args.useCamera = true;
        } else if (arg == "-cp" || arg == "--camera_port") {
            if (i + 1 < argc) {
                args.cameraPort = std::stoi(argv[++i]);
            } else {
                throw std::runtime_error("--camera_port 옵션에 포트 번호가 필요합니다.");
            }
        } else if (arg == "-s" || arg == "--scale") {
            if (i + 1 < argc) {
                args.scaleFactor = std::stoi(argv[++i]);
                if (args.scaleFactor <= 0) {
                    throw std::runtime_error("크기 조정 계수는 양수여야 합니다.");
                }
            } else {
                throw std::runtime_error("--scale 옵션에 크기 조정 계수가 필요합니다.");
            }
        } else if (arg == "-h" || arg == "--help") {
            printHelp(argv[0]);
            exit(0);
        } else {
            throw std::runtime_error("알 수 없는 옵션: " + arg);
        }
    }
    
    // 입력 소스 검증
    if (!args.useCamera && args.inputFile.empty()) {
        throw std::runtime_error("입력 소스가 지정되지 않았습니다. -i 또는 -c 옵션을 사용하세요.");
    }
    
    if (args.useCamera && !args.inputFile.empty()) {
        throw std::runtime_error("카메라와 비디오 파일을 동시에 지정할 수 없습니다.");
    }
    
    return args;
}

int main(int argc, char* argv[]) {
    try {
        // 로거 초기화
        vve::Logger::init(vve::LogLevel::INFO);
        
        // OpenCL 설정
        vve::setupOpenCL();
        
        // 명령줄 인자 파싱
        CommandLineArgs args = parseCommandLine(argc, argv);
        
        // HOG 계산기 초기화
        vve::HOGCalculator hogCalculator(
            vve::NUM_ORIENTATION_BINS,
            vve::GAUSSIAN_KERNEL_SIZE_X,
            vve::GAUSSIAN_KERNEL_SIZE_Y,
            vve::GAUSSIAN_SIGMA,
            vve::MAGNITUDE_THRESHOLD
        );
        
        // 방향성 분석기 초기화
        vve::OrientationAnalyzer orientationAnalyzer(
            vve::ORIENTATION_RANGE_START,
            vve::ORIENTATION_RANGE_END,
            vve::SMOOTHING_FACTOR,
            vve::TOP_N_PEAKS
        );
        
        // 시각화 도구 초기화
        vve::Visualizer visualizer(
            vve::LINE_THICKNESS,
            vve::ARROW_THICKNESS,
            vve::FONT_SCALE,
            vve::FONT_THICKNESS,
            vve::HIST_YLIM,
            vve::HIST_BAR_WIDTH
        );
        
        // 비디오 프로세서 초기화
        vve::VideoProcessor videoProcessor(
            hogCalculator,
            orientationAnalyzer,
            visualizer,
            args.scaleFactor
        );
        
        // 비디오 처리 시작
        bool success = false;
        if (args.useCamera) {
            success = videoProcessor.processCamera(args.cameraPort);
        } else {
            success = videoProcessor.processVideoFile(args.inputFile);
        }
        
        return success ? 0 : 1;
    } catch (const std::exception& e) {
        vve::Logger::error(std::string("오류: ") + e.what());
        printHelp(argv[0]);
        return 1;
    } catch (...) {
        vve::Logger::error("알 수 없는 오류가 발생했습니다.");
        return 1;
    }
}