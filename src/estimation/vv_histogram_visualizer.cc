#include "vvp/estimation/vv_histogram_visualizer.h"
#include "absl/strings/str_format.h"
#include <opencv2/imgproc.hpp>
#include <algorithm>
#include <numeric>

namespace vv {

VVHistogramVisualizer::VVHistogramVisualizer(VVParams const& params)
    : params_(params) {}

auto VVHistogramVisualizer::create_histogram_visualization(
    std::vector<float> const& hog_histogram, VVResult const& vv_result,
    int width, int height) const -> cv::Mat {
  if (hog_histogram.empty()) {
    return {height, width, CV_8UC3,
            cv::Scalar(VVEstimatorConstants::kWhiteColor,
                       VVEstimatorConstants::kWhiteColor,
                       VVEstimatorConstants::kWhiteColor)};
  }

  float hist_sum =
      std::accumulate(hog_histogram.begin(), hog_histogram.end(), 0.0F);
  if (hist_sum <= 0) {
    return {height, width, CV_8UC3,
            cv::Scalar(VVEstimatorConstants::kWhiteColor,
                       VVEstimatorConstants::kWhiteColor,
                       VVEstimatorConstants::kWhiteColor)};
  }

  cv::Mat hist_image(height, width, CV_8UC3,
                     cv::Scalar(VVEstimatorConstants::kWhiteColor,
                                VVEstimatorConstants::kWhiteColor,
                                VVEstimatorConstants::kWhiteColor));

  int bar_width = std::max(1, width / static_cast<int>(hog_histogram.size()));

  float max_val =
      *std::max_element(hog_histogram.begin(), hog_histogram.end()) / hist_sum;
  float scale = VVEstimatorConstants::kHistogramHeightScale *
                static_cast<float>(height) /
                std::max(max_val, VVEstimatorConstants::kHistogramMinValue);

  for (size_t i = 0; i < hog_histogram.size(); i++) {
    float norm_val = hog_histogram[i] / hist_sum;
    int bar_height = cvRound(norm_val * scale);
    int pos_x = width - (static_cast<int>(i) * bar_width) - bar_width;
    cv::rectangle(hist_image, cv::Point(pos_x, height - bar_height),
                  cv::Point(pos_x + bar_width, height),
                  cv::Scalar(VVEstimatorConstants::kHistogramBarColor,
                             VVEstimatorConstants::kHistogramBarColor,
                             VVEstimatorConstants::kHistogramBarColor),
                  cv::FILLED);
  }

  int vv_x =
      width - (static_cast<int>(vv_result.angle) * bar_width) - (bar_width / 2);
  cv::line(hist_image, cv::Point(vv_x, 0), cv::Point(vv_x, height),
           cv::Scalar(VVEstimatorConstants::kBlackColor,
                      VVEstimatorConstants::kGreenColor,
                      VVEstimatorConstants::kBlackColor),
           VVEstimatorConstants::kThickLineWidth, cv::LINE_AA);

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

  for (int angle = 0; angle <= VVEstimatorConstants::kMaxAngle;
       angle += VVEstimatorConstants::kTickStep) {
    int tick_x = width - (angle * bar_width) - (bar_width / 2);
    cv::line(hist_image,
             cv::Point(tick_x, height - VVEstimatorConstants::kTickLength),
             cv::Point(tick_x, height),
             cv::Scalar(VVEstimatorConstants::kBlackColor,
                        VVEstimatorConstants::kBlackColor,
                        VVEstimatorConstants::kBlackColor),
             VVEstimatorConstants::kThinLineWidth, cv::LINE_AA);
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
