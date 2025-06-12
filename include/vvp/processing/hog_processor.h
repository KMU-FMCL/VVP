#ifndef VVP_PROCESSING_HOGPROCESSOR_H_
#define VVP_PROCESSING_HOGPROCESSOR_H_

#include "vvp/estimation/types.h"
#include "vvp/processing/constants.h"  // Added constants include
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

namespace vv {

/**
 * @brief HOG 처리 클래스
 *
 * HOG(Histogram of Oriented Gradients) 계산 및 관련 처리를 담당하는
 * 클래스입니다.
 */
class HogProcessor {
 public:
  /**
   * @brief 생성자
   * @param params HOG 계산 파라미터
   */
  explicit HogProcessor(HOGParams const& params = HOGParams());

  /**
   * @brief HOG(Histogram of Oriented Gradients) 계산
   * @param image 입력 이미지
   * @return HOG 계산 결과 (그래디언트, 히스토그램, 매그니튜드 등)
   */
  auto compute_hog(cv::Mat const& image) -> HOGResult;

 private:
  HOGParams params_;
  cv::Mat erode_kernel_;
};

}  // namespace vv

#endif  // VVP_PROCESSING_HOGPROCESSOR_H_
