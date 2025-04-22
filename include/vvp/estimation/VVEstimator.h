#ifndef VVP_ESTIMATION_VVESTIMATOR_H_
#define VVP_ESTIMATION_VVESTIMATOR_H_

#include <vector>

#include <opencv2/core.hpp>

#include "vvp/estimation/Types.h"

namespace vv {

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
  explicit VVEstimator(const VVParams& params);

  /**
   * @brief HOG 히스토그램에서 VV 각도 추정
   * @param hogHistogram HOG 히스토그램
   * @param previousResult 이전 프레임의 VV 결과 (스무딩을 위해 사용)
   * @return 추정된 VV 결과
   */
  auto estimate_vv(const std::vector<float>& hog_histogram,
                   const VVResult& previous_result) -> VVResult;

  /**
   * @brief 모든 VV 결과 얻기
   * @return 지금까지 계산된 모든 VV 결과 벡터
   */
  auto get_all_results() const -> const std::vector<VVResult>&;

  /**
   * @brief 히스토그램 시각화 이미지 생성
   * @param hogHistogram HOG 히스토그램
   * @param vvResult 현재 VV 결과
   * @param width 이미지 너비
   * @param height 이미지 높이
   * @return 히스토그램 시각화 이미지
   */
  auto create_histogram_visualization(const std::vector<float>& hog_histogram,
                                      const VVResult& vv_result,
                                      int width,
                                      int height) const -> cv::Mat;

 private:
  std::vector<VVResult> results_;  ///< 모든 프레임의 VV 결과 저장
  VVParams params_;                ///< estimator 파라미터 저장

  // 상수 정의
  static constexpr int kTopPeakCount = 3;  ///< 상위 피크 개수
  static constexpr float kHistogramHeightScale =
      0.8F;  ///< 히스토그램 높이 스케일
  static constexpr float kHistogramMinValue = 0.001F;  ///< 히스토그램 최소값
  static constexpr int kTickStep = 30;            ///< 히스토그램 X축 눈금 간격
  static constexpr int kThickLineWidth = 2;       ///< 굵은 선 두께
  static constexpr int kThinLineWidth = 1;        ///< 얇은 선 두께
  static constexpr double kLabelFontScale = 0.4;  ///< 레이블 폰트 크기
  static constexpr int kWhiteColor = 255;         ///< 흰색 RGB 값
};

}  // namespace vv

#endif  // VVP_ESTIMATION_VVESTIMATOR_H_
