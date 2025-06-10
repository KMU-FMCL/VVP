#ifndef VVP_ESTIMATION_TYPES_H_
#define VVP_ESTIMATION_TYPES_H_

#include "absl/strings/string_view.h"
#include <opencv2/core.hpp>
#include <string>
#include <vector>

namespace vv {

/**
 * @brief Time format constants
 */
struct TimeConstants {
  static absl::string_view const kIsoFormat;  // YYYY-MM-DD HH:MM:SS format
};

/**
 * @brief Constants for HOG parameters
 */
struct HOGConstants {
  static constexpr int kDefaultBinCount = 180;  // Full angle range in degrees
  static constexpr double kDefaultThresholdValue = 0.25;
  static constexpr int kDefaultBlurKernelSize = 11;
  static constexpr double kDefaultBlurSigma = 3.0;
  static constexpr int kDefaultErodeKernelSize = 3;
};

/**
 * @brief Constants for VV Estimator
 */
struct VVConstants {
  static constexpr int kDefaultMinAngle =
      30;  // Default minimum angle (degrees)
  static constexpr int kDefaultMaxAngle =
      150;  // Default maximum angle (degrees)
  static constexpr double kDefaultSmoothingFactor =
      0.7;  // Temporal smoothing factor (0.0-1.0)
};

/**
 * @brief Physics constants
 */
struct PhysicsConstants {
  static constexpr double kGravityAcceleration =
      9.8;  // Gravity acceleration (m/s^2)
  static constexpr double kDefaultVerticalAngle =
      90.0;  // Default vertical angle (degrees)
  static constexpr double kDegreesToRadians =
      M_PI / 180.0;  // Degrees to radians conversion
};

// Program configuration structure
struct Config {
  bool use_camera = false;
  int camera_port = 0;
  std::string input_file_path = "./test.mp4";
  int scale = 2;
  bool save_results = true;
};

// HOG parameters structure
struct HOGParams {
  int bin_count = HOGConstants::kDefaultBinCount;
  double threshold_value = HOGConstants::kDefaultThresholdValue;
  int blur_kernel_size = HOGConstants::kDefaultBlurKernelSize;
  double blur_sigma = HOGConstants::kDefaultBlurSigma;
  int erode_kernel_size = HOGConstants::kDefaultErodeKernelSize;
};

// VVEstimator parameters structure
struct VVParams {
  int min_angle =
      VVConstants::kDefaultMinAngle;  // Minimum angle of interest (degrees)
  int max_angle =
      VVConstants::kDefaultMaxAngle;  // Maximum angle of interest (degrees)
  double smoothing_factor =
      VVConstants::kDefaultSmoothingFactor;  // Smoothing weight factor
                                             // (0.0-1.0)
};

// VV estimation result structure
struct VVResult {
  double angle = PhysicsConstants::kDefaultVerticalAngle;  // Vertical direction
                                                           // angle (degrees)
  double angle_rad = PhysicsConstants::kDefaultVerticalAngle *
                     PhysicsConstants::kDegreesToRadians;  // Vertical direction
                                                           // angle (radians)
  double acc_x = 0.0;  // X-direction acceleration (m/s^2)
  double acc_y =
      PhysicsConstants::kGravityAcceleration;  // Y-direction acceleration
                                               // (m/s^2)
  double fps = 0.0;  // Frames Per Second at the time of this result

  // Method to calculate acceleration from angle
  void update_acceleration();
};

// HOG calculation result structure
struct HOGResult {
  cv::Mat gradient_x;
  cv::Mat gradient_y;
  std::vector<float> histogram;
  cv::Mat magnitude;
  cv::Mat magnitude_filtered;
};

}  // namespace vv

#endif  // VVP_ESTIMATION_TYPES_H_
