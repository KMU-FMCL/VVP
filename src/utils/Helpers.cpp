#include "vvp/utils/Helpers.h"

#include <cstring>
#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/core/ocl.hpp>

#include "absl/strings/str_format.h"
#include "absl/time/clock.h"
#include "absl/time/time.h"

namespace vv {
namespace utils {

Config parse_command_line_args(int argc, char* argv[]) {
  Config config;

  for (int i = 1; i < argc; i++) {
    std::string arg = argv[i];

    if (arg == "-h" || arg == "--help") {
      print_usage();
      exit(0);
    } else if (arg == "-i" || arg == "--inputfile") {
      if (i + 1 < argc) {
        config.input_file_path = argv[++i];
        config.use_camera = false;
      }
    } else if (arg == "-c" || arg == "--camera") {
      if (i + 1 < argc) {
        std::string value = argv[++i];
        if (value == "true" || value == "True" || value == "1") {
          config.use_camera = true;
        }
      }
    } else if (arg == "-cp" || arg == "--camera_port") {
      if (i + 1 < argc) {
        config.camera_port = std::stoi(argv[++i]);
      }
    } else if (arg == "-s" || arg == "--scale") {
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

std::string format_current_time(absl::string_view format) {
  absl::Time now = absl::Now();
  return absl::FormatTime(format, now, absl::LocalTimeZone());
}

std::string get_current_date_string() {
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
      << std::endl;
}

void print_opencv_info() {
  std::cout << "OpenCV Version: " << CV_VERSION << std::endl;

  // OpenCL 지원 확인
  std::cout << "OpenCL support: "
            << (cv::ocl::haveOpenCL() ? "Available" : "Not available")
            << std::endl;

  if (cv::ocl::haveOpenCL()) {
    cv::ocl::setUseOpenCL(true);
    std::cout << "Using OpenCL: " << (cv::ocl::useOpenCL() ? "Yes" : "No")
              << std::endl;

    // OpenCL 장치 정보 출력
    cv::ocl::Device device = cv::ocl::Device::getDefault();
    std::cout << "OpenCL Device: " << device.name() << std::endl;
    std::cout << "Vendor: " << device.vendorName() << std::endl;
  }
}

}  // namespace utils
}  // namespace vv
