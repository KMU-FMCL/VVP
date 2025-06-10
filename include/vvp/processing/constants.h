#ifndef VVP_PROCESSING_CONSTANTS_H_
#define VVP_PROCESSING_CONSTANTS_H_

#include <opencv2/core.hpp>  // For cv::Scalar and CV_PI

namespace vv {

/**
 * @brief Constants for visualization
 */
struct VisualizationConstants {
  static constexpr int kLineThickness = 2;
  static constexpr int kFpsTextPositionX = 1100;
  static constexpr int kFpsTextPositionY = 30;
  static constexpr double kFpsTextScale = 1.0;
  static constexpr int kVvTextPositionX = 10;
  static constexpr int kVvTextPositionY = 30;
  static constexpr double kVvTextScale = 2.0;
  static constexpr double kHistogramHeightScale = 0.8;
};

/**
 * @brief Constants for angle calculations and physics
 */
struct AngleConstants {
  static constexpr float kHistogramMinValue = 0.001F;
  static constexpr double kGravityAcceleration = 9.8;  // m/s^2
  static constexpr int kTickStep = 30;  // Histogram X-axis tick interval
  static constexpr double kRadToDeg =
      180.0;  // Radians to degrees conversion constant
  static constexpr double kDegToRad =
      CV_PI / 180.0;  // Degrees to radians conversion constant
  static constexpr double kRightAngle = 90.0;   // Right angle in degrees
  static constexpr double kFullCircle = 360.0;  // Full circle in degrees
};

/**
 * @brief Constants for image operations
 */
struct ImageConstants {
  static constexpr float kDivideByTwo =
      2.0F;  // Divisor to find center of image dimensions
  static constexpr int kMinPixelValue = 0;  // Minimum value for 8-bit pixel
  static constexpr int kMaxPixelValue =
      255;  // Maximum value for 8-bit pixel representation

  // 색상 상수를 위한 중첩 구조체
  struct Colors {
    static cv::Scalar const kWhite;  // White color (BGR)
    static cv::Scalar const kBlack;  // Black color (BGR)
    static cv::Scalar const kGreen;  // Green color (BGR)
    static cv::Scalar const kRed;    // Red color (BGR)
  };
};

}  // namespace vv

#endif  // VVP_PROCESSING_CONSTANTS_H_
