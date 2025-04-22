#pragma once

#include <vector>

#include <opencv2/core.hpp>

#include "vvp/estimation/Types.hpp"

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
  VVResult estimateVV(const std::vector<float>& hogHistogram,
                      const VVResult& previousResult);

  /**
   * @brief 모든 VV 결과 얻기
   * @return 지금까지 계산된 모든 VV 결과 벡터
   */
  const std::vector<VVResult>& getAllResults() const;

  /**
   * @brief 히스토그램 시각화 이미지 생성
   * @param hogHistogram HOG 히스토그램
   * @param vvResult 현재 VV 결과
   * @param width 이미지 너비
   * @param height 이미지 높이
   * @return 히스토그램 시각화 이미지
   */
  cv::Mat createHistogramVisualization(const std::vector<float>& hogHistogram,
                                       const VVResult& vvResult, int width,
                                       int height) const;

 private:
  std::vector<VVResult> m_results;  ///< 모든 프레임의 VV 결과 저장
  VVParams m_params;                ///< estimator 파라미터 저장
};

}  // namespace vv