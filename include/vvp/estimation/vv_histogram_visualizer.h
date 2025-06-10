#ifndef VVP_ESTIMATION_VV_HISTOGRAM_VISUALIZER_H_
#define VVP_ESTIMATION_VV_HISTOGRAM_VISUALIZER_H_

#include "vvp/estimation/types.h"
#include "vvp/estimation/vv_estimator_constants.h"
#include <opencv2/core.hpp>
#include <vector>

namespace vv {

/**
 * @brief HOG 히스토그램 시각화 클래스
 */
class VVHistogramVisualizer {
 public:
  /**
   * @brief 파라미터 지정 생성자
   * @param params Visual Vertical 추정 파라미터 (시각화에 필요한 min/max angle
   * 등)
   */
  explicit VVHistogramVisualizer(VVParams const& params);

  /**
   * @brief 히스토그램 시각화 이미지 생성
   * @param hogHistogram HOG 히스토그램
   * @param vvResult 현재 VV 결과
   * @param width 이미지 너비
   * @param height 이미지 높이
   * @return 히스토그램 시각화 이미지
   */
  auto create_histogram_visualization(std::vector<float> const& hog_histogram,
                                      VVResult const& vv_result, int width,
                                      int height) const -> cv::Mat;

 private:
  VVParams params_;  ///< 시각화에 필요한 파라미터 (min_angle, max_angle 등)
};

}  // namespace vv

#endif  // VVP_ESTIMATION_VV_HISTOGRAM_VISUALIZER_H_
