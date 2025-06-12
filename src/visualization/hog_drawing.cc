// src/visualization/hog_drawing.cc
#include "vvp/visualization/hog_drawing.h"
#include "vvp/estimation/types.h"      // For vv::HOGResult definition
#include "vvp/processing/constants.h"  // HOG related constants, ImageConstants
#include <opencv2/imgproc.hpp>         // For cv::cvtColor, cv::hconcat

namespace {
// Helper function to convert a single-channel matrix to a displayable BGR
// image.
cv::Mat convert_to_displayable_image(cv::Mat const& input_mat) {
  if (input_mat.empty()) {
    return {};
  }
  cv::Mat displayable;
  input_mat.convertTo(displayable, CV_8U, vv::ImageConstants::kMaxPixelValue);
  cv::cvtColor(displayable, displayable, cv::COLOR_GRAY2BGR);
  return displayable;
}
}  // namespace

namespace vv {
namespace visualization {

cv::Mat create_hog_images_row(vv::HOGResult const& hog_result) {
  cv::Mat hog_magnitude_display =
      convert_to_displayable_image(hog_result.magnitude);
  cv::Mat hog_magnitude_filtered_display =
      convert_to_displayable_image(hog_result.magnitude_filtered);

  cv::Mat middle_row;
  if (!hog_magnitude_display.empty() &&
      !hog_magnitude_filtered_display.empty()) {
    cv::hconcat(hog_magnitude_display, hog_magnitude_filtered_display,
                middle_row);
  } else if (!hog_magnitude_display.empty()) {
    middle_row = hog_magnitude_display;
  } else if (!hog_magnitude_filtered_display.empty()) {
    middle_row = hog_magnitude_filtered_display;
  }
  // If both are empty, middle_row remains empty by default construction.

  return middle_row;
}

}  // namespace visualization
}  // namespace vv
