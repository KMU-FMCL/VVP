#ifndef VVP_PROCESSING_IMAGEPROCESSOR_H_
#define VVP_PROCESSING_IMAGEPROCESSOR_H_

#include "vvp/estimation/types.h"
#include "vvp/processing/constants.h"  // Added constants include
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

namespace vv {

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
