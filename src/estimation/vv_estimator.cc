#include "vvp/estimation/vv_estimator.h"
// vv_estimator_constants.h is included via vv_estimator.h
// absl/strings/str_format.h is no longer needed here
// opencv2/highgui.hpp is no longer needed here
// opencv2/imgproc.hpp is no longer needed here
#include <algorithm>  // For std::min, std::partial_sort, std::iota
#include <cmath>      // For std::isnan
#include <numeric>    // For std::iota
#include <vector>     // Already included via header

namespace vv {

VVEstimator::VVEstimator() : params_() {
  // 기본 파라미터 사용
}
//------------------------------------------------------------------------------
VVEstimator::VVEstimator(VVParams const& params) : params_(params) {
  // 사용자 정의 파라미터 사용
}

// --- Start of new private helper functions for estimate_vv ---
auto VVEstimator::find_top_peak_indices(
    std::vector<float> const& hog_histogram) const -> std::vector<int> {
  std::vector<int> indices(hog_histogram.size());
  std::iota(indices.begin(), indices.end(), 0);

  auto compare_func = [&hog_histogram](int idx1, int idx2) {
    return hog_histogram[idx1] > hog_histogram[idx2];
  };

  std::vector<int> best_indices;
  for (int angle : indices) {
    if (angle >= params_.min_angle && angle <= params_.max_angle) {
      best_indices.push_back(angle);
    }
  }

  if (best_indices.empty()) {
    return {};
  }

  int num_peaks_to_find = std::min(VVEstimatorConstants::kTopPeakCount,
                                   static_cast<int>(best_indices.size()));

  std::partial_sort(best_indices.begin(),
                    best_indices.begin() + num_peaks_to_find,
                    best_indices.end(), compare_func);

  best_indices.resize(num_peaks_to_find);
  return best_indices;
}

auto VVEstimator::calculate_weighted_vv_angle(
    std::vector<float> const& hog_histogram,
    std::vector<int> const& peak_indices, double previous_angle) -> double {
  if (peak_indices.empty()) {
    return previous_angle;
  }

  double sum_weights = 0.0;
  double sum_weighted_angles = 0.0;

  for (int idx : peak_indices) {
    double weight = hog_histogram[idx];
    sum_weights += weight;
    sum_weighted_angles += idx * weight;
  }

  if (sum_weights > 0) {
    return sum_weighted_angles / sum_weights;
  }
  return previous_angle;
}

auto VVEstimator::apply_temporal_smoothing(double current_angle,
                                           double previous_angle) const
    -> double {
  return (params_.smoothing_factor * current_angle) +
         ((1.0 - params_.smoothing_factor) * previous_angle);
}
// --- End of new private helper functions ---

auto VVEstimator::estimate_vv(std::vector<float> const& hog_histogram,
                              VVResult const& previous_result,
                              double current_fps) -> VVResult {
  VVResult result;

  std::vector<int> top_peak_indices = find_top_peak_indices(hog_histogram);

  if (top_peak_indices.empty()) {
    result = previous_result;
    // result.fps will be set later
  } else {
    double calculated_angle = calculate_weighted_vv_angle(
        hog_histogram, top_peak_indices, previous_result.angle);

    if (std::isnan(calculated_angle)) {
      result.angle = previous_result.angle;
    } else {
      result.angle = calculated_angle;
    }
    result.angle =
        apply_temporal_smoothing(result.angle, previous_result.angle);
  }

  result.update_acceleration();
  result.fps = current_fps;
  results_.push_back(result);

  return result;
}

auto VVEstimator::get_all_results() const -> std::vector<VVResult> const& {
  return results_;
}

}  // namespace vv
