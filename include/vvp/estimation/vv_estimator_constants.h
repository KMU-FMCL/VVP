#ifndef VVP_ESTIMATION_VV_ESTIMATOR_CONSTANTS_H_
#define VVP_ESTIMATION_VV_ESTIMATOR_CONSTANTS_H_

namespace vv {

/**
 * @brief VVEstimator 상수 정의
 */
struct VVEstimatorConstants {
  static constexpr int kTopPeakCount = 3;  ///< 상위 피크 개수
  static constexpr float kHistogramHeightScale =
      0.8F;  ///< 히스토그램 높이 스케일
  static constexpr float kHistogramMinValue = 0.001F;  ///< 히스토그램 최소값
  static constexpr int kTickStep = 30;            ///< 히스토그램 X축 눈금 간격
  static constexpr int kThickLineWidth = 2;       ///< 굵은 선 두께
  static constexpr int kThinLineWidth = 1;        ///< 얇은 선 두께
  static constexpr double kLabelFontScale = 0.4;  ///< 레이블 폰트 크기
  static constexpr int kWhiteColor = 255;         ///< 흰색 RGB 값

  // 추가된 상수 정의
  static constexpr int kHistogramBarColor = 150;  ///< 히스토그램 막대 색상
  static constexpr int kBlackColor = 0;           ///< 검은색 RGB 값
  static constexpr int kGreenColor = 255;         ///< 녹색 채널 값
  static constexpr int kMaxAngle = 180;           ///< 최대 각도
  static constexpr int kTickLength = 5;           ///< 눈금 선 길이
  static constexpr int kLabelOffset = 10;         ///< 레이블 오프셋
};

}  // namespace vv

#endif  // VVP_ESTIMATION_VV_ESTIMATOR_CONSTANTS_H_
