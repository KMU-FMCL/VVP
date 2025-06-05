#ifndef VVP_PROCESSING_IMAGEPROCESSOR_H_
#define VVP_PROCESSING_IMAGEPROCESSOR_H_

#include "vvp/estimation/Types.h"
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

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

/**
 * @brief 이미지 처리 클래스
 *
 * HOG 계산 및 이미지 변환, 처리를 담당하는 클래스입니다.
 */
class ImageProcessor {
 public:
  /**
   * @brief 생성자
   * @param params HOG 계산 파라미터
   */
  explicit ImageProcessor(HOGParams const& params = HOGParams());

  /**
   * @brief HOG(Histogram of Oriented Gradients) 계산
   * @param image 입력 이미지
   * @return HOG 계산 결과 (그래디언트, 히스토그램, 매그니튜드 등)
   */
  auto compute_hog(cv::Mat const& image) -> HOGResult;

  /**
   * @brief Resize image
   * @param image Input image
   * @param scale Scaling factor
   * @return Resized image
   */
  static auto resize_image(cv::Mat const& image, int scale) -> cv::Mat;

  /**
   * @brief Rotate image
   * @param image Input image
   * @param angle Rotation angle (degrees)
   * @return Rotated image
   */
  static auto rotate_image(cv::Mat const& image, double angle) -> cv::Mat;

  /**
   * @brief 결과 시각화 이미지 생성
   * @param inputImage 원본 입력 이미지
   * @param calibratedImage 보정된 이미지
   * @param hogResult HOG 계산 결과
   * @param vvResult VV 추정 결과
   * @param histogramImage 히스토그램 이미지
   * @param fps 프레임 속도
   * @return 모든 결과가 결합된 시각화 이미지
   */
  static auto create_visualization(cv::Mat const& input_image,
                                   cv::Mat const& calibrated_image,
                                   HOGResult const& hog_result,
                                   VVResult const& vv_result,
                                   cv::Mat const& histogram_image,
                                   float fps = 0.0F) -> cv::Mat;

 private:
  HOGParams params_;
  cv::Mat erode_kernel_;

  /**
   * @brief 이미지에 VV 각도 선 그리기
   * @param image 대상 이미지
   * @param vvResult VV 추정 결과
   * @return VV 표시가 추가된 이미지
   */
  static auto draw_vv_indicators(cv::Mat image, VVResult const& vv_result)
      -> cv::Mat;
};

}  // namespace vv

#endif  // VVP_PROCESSING_IMAGEPROCESSOR_H_
