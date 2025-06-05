#include "vvp/estimation/VVEstimator.h"
#include "absl/strings/str_format.h"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <algorithm>
#include <numeric>

namespace vv {

VVEstimator::VVEstimator() : params_() {
  // 기본 파라미터 사용
}
//------------------------------------------------------------------------------
VVEstimator::VVEstimator(VVParams const& params) : params_(params) {
  // 사용자 정의 파라미터 사용
}

auto VVEstimator::estimate_vv(std::vector<float> const& hog_histogram,
                              VVResult const& previous_result,
                              double current_fps) -> VVResult {
  VVResult result;

  // 최대 3개의 피크 찾기
  std::vector<int> indices(hog_histogram.size());
  std::iota(indices.begin(), indices.end(), 0);

  // MIN_ANGLE ~ MAX_ANGLE 범위에서만 고려
  auto compare_func = [&hog_histogram](int idx1, int idx2) {
    return hog_histogram[idx1] > hog_histogram[idx2];
  };

  // 범위 내의 상위 3개 인덱스 찾기 (min_angle ~ max_angle)
  std::vector<int> best_indices;
  for (int angle : indices) {
    if (angle >= params_.min_angle && angle <= params_.max_angle) {
      best_indices.push_back(angle);
    }
  }

  std::partial_sort(
      best_indices.begin(),
      best_indices.begin() + std::min(VVEstimatorConstants::kTopPeakCount,
                                      static_cast<int>(best_indices.size())),
      best_indices.end(), compare_func);

  // 상위 3개 인덱스만 사용
  best_indices.resize(std::min(VVEstimatorConstants::kTopPeakCount,
                               static_cast<int>(best_indices.size())));

  if (best_indices.empty()) {
    // 유효한 인덱스가 없을 경우 이전 결과 반환
    // FPS 값은 현재 값으로 업데이트
    result = previous_result;
    result.fps = current_fps;
    return result;
  }

  // 가중 평균으로 VV 계산
  double sum_weights = 0.0;
  double sum_weighted_angles = 0.0;

  for (int idx : best_indices) {
    double weight = hog_histogram[idx];
    sum_weights += weight;
    sum_weighted_angles += idx * weight;
  }

  if (sum_weights > 0) {
    result.angle = sum_weighted_angles / sum_weights;
  } else {
    // 가중치 합이 0이면 이전 결과 사용
    result.angle = previous_result.angle;
  }

  // NaN 체크
  if (std::isnan(result.angle)) {
    result.angle = previous_result.angle;
  }

  // 시간적 스무딩 적용
  result.angle = params_.smoothing_factor * result.angle +
                 (1.0 - params_.smoothing_factor) * previous_result.angle;

  // 가속도 계산
  result.update_acceleration();

  // FPS 저장
  result.fps = current_fps;

  // 결과 저장
  results_.push_back(result);

  return result;
}

auto VVEstimator::get_all_results() const -> std::vector<VVResult> const& {
  return results_;
}

auto VVEstimator::create_histogram_visualization(
    std::vector<float> const& hog_histogram, VVResult const& vv_result,
    int width, int height) const -> cv::Mat {
  // 히스토그램이 비어있는 경우 검사
  if (hog_histogram.empty()) {
    return {height, width, CV_8UC3,
            cv::Scalar(VVEstimatorConstants::kWhiteColor,
                       VVEstimatorConstants::kWhiteColor,
                       VVEstimatorConstants::kWhiteColor)};
  }

  // 정규화를 위해 합계 계산
  float hist_sum =
      std::accumulate(hog_histogram.begin(), hog_histogram.end(), 0.0F);
  if (hist_sum <= 0) {
    return {height, width, CV_8UC3,
            cv::Scalar(VVEstimatorConstants::kWhiteColor,
                       VVEstimatorConstants::kWhiteColor,
                       VVEstimatorConstants::kWhiteColor)};
  }

  // 시각화를 위한 이미지 생성
  cv::Mat hist_image(height, width, CV_8UC3,
                     cv::Scalar(VVEstimatorConstants::kWhiteColor,
                                VVEstimatorConstants::kWhiteColor,
                                VVEstimatorConstants::kWhiteColor));

  // 히스토그램 막대 너비 계산
  int bar_width = std::max(1, width / static_cast<int>(hog_histogram.size()));

  // 히스토그램 최대값 찾기 (정규화용)
  float max_val =
      *std::max_element(hog_histogram.begin(), hog_histogram.end()) / hist_sum;
  float scale =
      VVEstimatorConstants::kHistogramHeightScale * static_cast<float>(height) /
      std::max(
          max_val,
          VVEstimatorConstants::kHistogramMinValue);  // 높이의 80%까지 사용

  // 히스토그램 그리기
  for (size_t i = 0; i < hog_histogram.size(); i++) {
    float norm_val = hog_histogram[i] / hist_sum;
    int bar_height = cvRound(norm_val * scale);

    // X축 반전 (180 -> 0)
    int pos_x = width - (static_cast<int>(i) * bar_width) - bar_width;

    cv::rectangle(hist_image, cv::Point(pos_x, height - bar_height),
                  cv::Point(pos_x + bar_width, height),
                  cv::Scalar(VVEstimatorConstants::kHistogramBarColor,
                             VVEstimatorConstants::kHistogramBarColor,
                             VVEstimatorConstants::kHistogramBarColor),
                  cv::FILLED);
  }

  // VV 각도 표시
  int vv_x =
      width - (static_cast<int>(vv_result.angle) * bar_width) - (bar_width / 2);
  cv::line(hist_image, cv::Point(vv_x, 0), cv::Point(vv_x, height),
           cv::Scalar(VVEstimatorConstants::kBlackColor,
                      VVEstimatorConstants::kGreenColor,
                      VVEstimatorConstants::kBlackColor),
           VVEstimatorConstants::kThickLineWidth, cv::LINE_AA);

  // 경계선 표시 (min_angle, max_angle)
  int min_x = width - (params_.min_angle * bar_width) - (bar_width / 2);
  int max_x = width - (params_.max_angle * bar_width) - (bar_width / 2);

  cv::line(hist_image, cv::Point(min_x, 0), cv::Point(min_x, height),
           cv::Scalar(VVEstimatorConstants::kBlackColor,
                      VVEstimatorConstants::kBlackColor,
                      VVEstimatorConstants::kBlackColor),
           VVEstimatorConstants::kThinLineWidth, cv::LINE_AA);
  cv::line(hist_image, cv::Point(max_x, 0), cv::Point(max_x, height),
           cv::Scalar(VVEstimatorConstants::kBlackColor,
                      VVEstimatorConstants::kBlackColor,
                      VVEstimatorConstants::kBlackColor),
           VVEstimatorConstants::kThinLineWidth, cv::LINE_AA);

  // X축 눈금 추가
  for (int angle = 0; angle <= VVEstimatorConstants::kMaxAngle;
       angle += VVEstimatorConstants::kTickStep) {
    int tick_x = width - (angle * bar_width) - (bar_width / 2);

    // 눈금 선
    cv::line(hist_image,
             cv::Point(tick_x, height - VVEstimatorConstants::kTickLength),
             cv::Point(tick_x, height),
             cv::Scalar(VVEstimatorConstants::kBlackColor,
                        VVEstimatorConstants::kBlackColor,
                        VVEstimatorConstants::kBlackColor),
             VVEstimatorConstants::kThinLineWidth, cv::LINE_AA);

    // 눈금 레이블
    cv::putText(hist_image, absl::StrFormat("%d", angle),
                cv::Point(tick_x - VVEstimatorConstants::kLabelOffset,
                          height - VVEstimatorConstants::kLabelOffset),
                cv::FONT_HERSHEY_SIMPLEX, VVEstimatorConstants::kLabelFontScale,
                cv::Scalar(VVEstimatorConstants::kBlackColor,
                           VVEstimatorConstants::kBlackColor,
                           VVEstimatorConstants::kBlackColor),
                VVEstimatorConstants::kThinLineWidth, cv::LINE_AA);
  }

  return hist_image;
}

}  // namespace vv
