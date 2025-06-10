#ifndef VVP_ESTIMATION_VVESTIMATOR_H_
#define VVP_ESTIMATION_VVESTIMATOR_H_

#include "vvp/estimation/types.h"
#include "vvp/estimation/vv_estimator_constants.h"  // Added
#include <opencv2/core.hpp>
#include <vector>
// #include <optional> // Not strictly needed if calculate_weighted_vv_angle
// handles default

namespace vv {

// VVEstimatorConstants struct definition REMOVED

/**
 * @brief Visual Vertical 추정 클래스
 *
 * HOG 히스토그램에서 Visual Vertical 각도를 추정하는 클래스입니다.
 */
class VVEstimator {
 public:
  /**
   * @brief 기본 생성자 (내부 기본 파라미터 사용)
   */
  VVEstimator();

  /**
   * @brief 파라미터 지정 생성자
   * @param params Visual Vertical 추정 파라미터
   */
  explicit VVEstimator(VVParams const& params);

  /**
   * @brief HOG 히스토그램에서 VV 각도 추정
   * @param hogHistogram HOG 히스토그램
   * @param previousResult 이전 프레임의 VV 결과 (스무딩을 위해 사용)
   * @param currentFps 현재 프레임의 FPS 값
   * @return 추정된 VV 결과
   */
  auto estimate_vv(std::vector<float> const& hog_histogram,
                   VVResult const& previous_result, double current_fps)
      -> VVResult;

  /**
   * @brief 모든 VV 결과 얻기
   * @return 지금까지 계산된 모든 VV 결과 벡터
   */
  auto get_all_results() const -> std::vector<VVResult> const&;

  // create_histogram_visualization declaration REMOVED

 private:
  std::vector<VVResult> results_;  ///< 모든 프레임의 VV 결과 저장
  VVParams params_;                ///< estimator 파라미터 저장

  // ADDED private member functions for estimate_vv refactoring
  auto find_top_peak_indices(std::vector<float> const& hog_histogram) const
      -> std::vector<int>;
  static auto calculate_weighted_vv_angle(
      std::vector<float> const& hog_histogram,  // static added
      std::vector<int> const& peak_indices,
      double previous_angle) -> double;  // const removed
  auto apply_temporal_smoothing(double current_angle,
                                double previous_angle) const -> double;
};

}  // namespace vv

#endif  // VVP_ESTIMATION_VVESTIMATOR_H_
