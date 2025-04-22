#include "vvp/estimation/VVEstimator.hpp"

#include <algorithm>
#include <numeric>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

namespace vv {

VVEstimator::VVEstimator() : params_() {
  // 기본 파라미터 사용
}
//------------------------------------------------------------------------------
VVEstimator::VVEstimator(const VVParams& params) : params_(params) {
  // 사용자 정의 파라미터 사용
}

VVResult VVEstimator::estimate_vv(const std::vector<float>& hog_histogram,
                                  const VVResult& previous_result) {
  VVResult result;

  // 최대 3개의 피크 찾기
  std::vector<int> indices(hog_histogram.size());
  std::iota(indices.begin(), indices.end(), 0);

  // MIN_ANGLE ~ MAX_ANGLE 범위에서만 고려
  auto compare_func = [&hog_histogram](int i1, int i2) {
    return hog_histogram[i1] > hog_histogram[i2];
  };

  // 범위 내의 상위 3개 인덱스 찾기 (min_angle ~ max_angle)
  std::vector<int> best_indices;
  for (size_t i = 0; i < indices.size(); ++i) {
    int angle = indices[i];
    if (angle >= params_.min_angle && angle <= params_.max_angle) {
      best_indices.push_back(angle);
    }
  }

  std::partial_sort(
      best_indices.begin(),
      best_indices.begin() +
          std::min(kTopPeakCount, static_cast<int>(best_indices.size())),
      best_indices.end(), compare_func);

  // 상위 3개 인덱스만 사용
  best_indices.resize(
      std::min(kTopPeakCount, static_cast<int>(best_indices.size())));

  if (best_indices.empty()) {
    // 유효한 인덱스가 없을 경우 이전 결과 반환
    return previous_result;
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

  // 결과 저장
  results_.push_back(result);

  return result;
}

const std::vector<VVResult>& VVEstimator::get_all_results() const {
  return results_;
}

cv::Mat VVEstimator::create_histogram_visualization(
    const std::vector<float>& hog_histogram, const VVResult& vv_result,
    int width, int height) const {
  // 히스토그램이 비어있는 경우 검사
  if (hog_histogram.empty()) {
    return cv::Mat(height, width, CV_8UC3, cv::Scalar(255, 255, 255));
  }

  // 정규화를 위해 합계 계산
  float hist_sum =
      std::accumulate(hog_histogram.begin(), hog_histogram.end(), 0.0f);
  if (hist_sum <= 0) {
    return cv::Mat(height, width, CV_8UC3, cv::Scalar(255, 255, 255));
  }

  // 시각화를 위한 이미지 생성
  cv::Mat hist_image(height, width, CV_8UC3, cv::Scalar(255, 255, 255));

  // 히스토그램 막대 너비 계산
  int bar_width = std::max(1, width / static_cast<int>(hog_histogram.size()));

  // 히스토그램 최대값 찾기 (정규화용)
  float max_val =
      *std::max_element(hog_histogram.begin(), hog_histogram.end()) / hist_sum;
  float scale = kHistogramHeightScale * height /
                std::max(max_val, kHistogramMinValue);  // 높이의 80%까지 사용

  // 히스토그램 그리기
  for (size_t i = 0; i < hog_histogram.size(); i++) {
    float norm_val = hog_histogram[i] / hist_sum;
    int bar_height = cvRound(norm_val * scale);

    // X축 반전 (180 -> 0)
    int x = width - static_cast<int>(i) * bar_width - bar_width;

    cv::rectangle(hist_image, cv::Point(x, height - bar_height),
                  cv::Point(x + bar_width, height), cv::Scalar(100, 100, 100),
                  cv::FILLED);
  }

  // VV 각도 표시
  int vv_x =
      width - static_cast<int>(vv_result.angle) * bar_width - bar_width / 2;
  cv::line(hist_image, cv::Point(vv_x, 0), cv::Point(vv_x, height),
           cv::Scalar(0, 255, 0), kThickLineWidth, cv::LINE_AA);

  // 경계선 표시 (min_angle, max_angle)
  int x_min = width - params_.min_angle * bar_width - bar_width / 2;
  int x_max = width - params_.max_angle * bar_width - bar_width / 2;

  cv::line(hist_image, cv::Point(x_min, 0), cv::Point(x_min, height),
           cv::Scalar(0, 0, 0), kThinLineWidth, cv::LINE_AA);
  cv::line(hist_image, cv::Point(x_max, 0), cv::Point(x_max, height),
           cv::Scalar(0, 0, 0), kThinLineWidth, cv::LINE_AA);

  // X축 눈금 추가
  for (int angle = 0; angle <= 180; angle += kTickStep) {
    int x = width - angle * bar_width - bar_width / 2;

    // 눈금 선
    cv::line(hist_image, cv::Point(x, height - 5), cv::Point(x, height),
             cv::Scalar(0, 0, 0), kThinLineWidth, cv::LINE_AA);

    // 눈금 레이블
    cv::putText(hist_image, std::to_string(angle),
                cv::Point(x - 10, height - 10), cv::FONT_HERSHEY_SIMPLEX,
                kLabelFontScale, cv::Scalar(0, 0, 0), kThinLineWidth,
                cv::LINE_AA);
  }

  return hist_image;
}

}  // namespace vv
