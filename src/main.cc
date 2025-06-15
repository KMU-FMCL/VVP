#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_format.h"
#include "absl/strings/string_view.h"
#include "absl/types/span.h"
#include "vvp/app/runner.h"  // Added Runner header
#include "vvp/estimation/vv_estimator.h"
#include "vvp/estimation/vv_histogram_visualizer.h"  // Added for VVHistogramVisualizer
#include "vvp/fps/fps_counter.h"
#include "vvp/io/input_handler.h"   // Changed from io_handler.h
#include "vvp/io/output_handler.h"  // Added
#include "vvp/processing/hog_processor.h"
#include "vvp/utils/config_loader.h"  // YAML config loader
#include "vvp/utils/display.h"        // For DisplayUtils
#include "vvp/utils/helpers.h"        // print_opencv_info
#include "vvp/utils/image.h"          // Added image.h
#include "vvp/utils/time.h"
#include "vvp/visualization/visualization.h"          // Moved visualization.h
#include "vvp/visualization/visualization_context.h"  // For VisualizationContext
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

// 결과 비디오 높이 계산을 위한 상수
constexpr double kResultHeightFactor =
    2.6;  // 원본 높이의 2.6배 (히스토그램 공간 포함)
// 수직 기준 각도 (도)
constexpr int kVerticalAngleDegrees = 90;  // 이미지 회전 기준점(수직 방향)
// 히스토그램 높이 비율 (원본 이미지 높이 대비)
constexpr double kHistogramHeightFactor = 0.6;  // 원본 높이의 60%
// 키보드 키 코드
constexpr int kEscKeyCode = 27;  // ESC 키의 ASCII 코드

namespace {
auto print_help() -> void {
  std::cout << "Visual Vertical Estimator - Help\n"
            << "================================\n"
            << "Usage: vv_estimator [OPTIONS] [CONFIG_PATH]\n\n"
            << "Options:\n"
            << "  --help    Display this help message and exit\n\n"
            << "Arguments:\n"
            << "  CONFIG_PATH    Path to YAML configuration file (default: "
               "./config/config.yaml)\n"
            << '\n';
}
}  // namespace

auto main(int argc, char* argv[]) -> int {
  try {
    // 명령줄 인자를 안전하게 접근하기 위해 absl::Span 사용
    absl::Span<char*> args(argv, argc);

    // 도움말 옵션 확인
    for (int i = 1; i < argc; i++) {
      absl::string_view arg(args[i]);
      if (arg == "--help" || arg == "-h") {
        print_help();
        return 0;
      }
    }

    // Runner 인스턴스 생성 및 실행
    vv::Runner runner(argc, argv);
    return runner.Run();
  } catch (std::exception const& e) {
    std::cerr << "Unhandled exception reached main: " << e.what() << std::endl;
    return 1;
  } catch (...) {
    std::cerr << "Unknown unhandled exception reached main." << std::endl;
    return 1;
  }
}
