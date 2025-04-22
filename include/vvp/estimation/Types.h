#ifndef VVP_ESTIMATION_TYPES_H_
#define VVP_ESTIMATION_TYPES_H_

#include <string>
#include <vector>

#include <opencv2/core.hpp>

#include "absl/strings/string_view.h"

namespace vv {

// 시간 형식 상수
const absl::string_view kIsoTimeFormat = "%Y%m%d_%H%M%S";

// 프로그램 설정 구조체
struct Config {
  bool use_camera = false;
  int camera_port = 0;
  std::string input_file_path = "./test.mp4";
  int scale = 2;
  bool save_results = true;
};

// HOG 파라미터 구조체
struct HOGParams {
  int bin_count = 180;
  double threshold_value = 0.25;
  int blur_kernel_size = 11;
  double blur_sigma = 3.0;
  int erode_kernel_size = 3;
};

// VVEstimator 파라미터 구조체
struct VVParams {
  int min_angle = 30;             // 관심 각도 최소 (도)
  int max_angle = 150;            // 관심 각도 최대 (도)
  double smoothing_factor = 0.7;  // 스무딩 가중치 (0.0-1.0)
};

// VV 추정 결과 구조체
struct VVResult {
  double angle = 90.0;            // 수직 방향 각도 (도)
  double angle_rad = M_PI / 2.0;  // 수직 방향 각도 (라디안)
  double acc_x = 0.0;             // X방향 가속도 (m/s^2)
  double acc_y = 9.8;             // Y방향 가속도 (m/s^2)

  // 각도에서 가속도 계산 메서드
  void update_acceleration() {
    angle_rad = angle * M_PI / 180.0;
    acc_x = 9.8 * std::cos(angle_rad);
    acc_y = 9.8 * std::sin(angle_rad);
  }
};

// HOG 계산 결과 구조체
struct HOGResult {
  cv::Mat gradient_x;
  cv::Mat gradient_y;
  std::vector<float> histogram;
  cv::Mat magnitude;
  cv::Mat magnitude_filtered;
};

}  // namespace vv

#endif  // VVP_ESTIMATION_TYPES_H_
