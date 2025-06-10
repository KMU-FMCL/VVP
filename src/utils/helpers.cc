#include "vvp/utils/helpers.h"
#include "absl/strings/str_format.h"
#include "absl/time/clock.h"
#include "absl/time/time.h"
#include <opencv2/core.hpp>
#include <opencv2/core/ocl.hpp>
#include <cstring>
#include <iostream>

namespace vv::utils {

namespace {

// 개별 명령줄 인자 처리 함수
void handle_help_option(Config& /*config*/) {
  print_usage();
  exit(0);
}

void handle_input_file_option(Config& config, int& arg_index,
                              absl::Span<char* const> args) {
  if (arg_index + 1 < args.size()) {
    config.input_file_path = args[++arg_index];
    config.use_camera = false;
  }
}

void handle_camera_option(Config& config, int& arg_index,
                          absl::Span<char* const> args) {
  if (arg_index + 1 < args.size()) {
    std::string value = args[++arg_index];
    if (value == "true" || value == "True" || value == "1") {
      config.use_camera = true;
    }
  }
}

void handle_camera_port_option(Config& config, int& arg_index,
                               absl::Span<char* const> args) {
  if (arg_index + 1 < args.size()) {
    config.camera_port = std::stoi(args[++arg_index]);
  }
}

void handle_scale_option(Config& config, int& arg_index,
                         absl::Span<char* const> args) {
  if (arg_index + 1 < args.size()) {
    config.scale = std::stoi(args[++arg_index]);
    if (config.scale <= 0) {
      config.scale = 1;
    }
  }
}

}  // namespace

auto parse_command_line_args(absl::Span<char* const> args) -> Config {
  Config config;

  // 빈 인자 목록이거나 프로그램 이름만 있는 경우 기본 설정 반환
  if (args.size() <= 1) {
    return config;
  }

  for (int arg_index = 1; arg_index < args.size(); arg_index++) {
    std::string arg = args[arg_index];

    if (arg == "-h" || arg == "--help") {
      handle_help_option(config);
    } else if (arg == "-i" || arg == "--inputfile") {
      handle_input_file_option(config, arg_index, args);
    } else if (arg == "-c" || arg == "--camera") {
      handle_camera_option(config, arg_index, args);
    } else if (arg == "-cp" || arg == "--camera_port") {
      handle_camera_port_option(config, arg_index, args);
    } else if (arg == "-s" || arg == "--scale") {
      handle_scale_option(config, arg_index, args);
    }
  }

  return config;
}

auto format_current_time(absl::string_view format) -> std::string {
  absl::Time now = absl::Now();
  return absl::FormatTime(format, now, absl::LocalTimeZone());
}

auto get_current_date_string() -> std::string {
  absl::Time now = absl::Now();
  return absl::FormatTime("%Y%m%d", now, absl::LocalTimeZone());
}

void print_usage() {
  std::cout
      << "Visual Vertical Estimator\n"
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
      << '\n';
}

void print_opencv_info() {
  std::cout << "OpenCV Version: " << CV_VERSION << '\n';

  // OpenCL 지원 확인
  std::cout << "OpenCL support: "
            << (cv::ocl::haveOpenCL() ? "Available" : "Not available") << '\n';

  if (cv::ocl::haveOpenCL()) {
    cv::ocl::setUseOpenCL(true);
    std::cout << "Using OpenCL: " << (cv::ocl::useOpenCL() ? "Yes" : "No")
              << '\n';

    // OpenCL 장치 정보 출력
    cv::ocl::Device const& device = cv::ocl::Device::getDefault();
    std::cout << "OpenCL Device: " << device.name() << '\n';
    std::cout << "Vendor: " << device.vendorName() << '\n';
  }
}

}  // namespace vv::utils
