// src/visualization/hog_drawing.cc
#include "vvp/visualization/hog_drawing.h"
#include "vvp/estimation/types.h"      // For vv::HOGResult definition
#include "vvp/processing/constants.h"  // HOG related constants, ImageConstants
#include <opencv2/imgproc.hpp>         // For cv::cvtColor, cv::hconcat

namespace vv {
namespace visualization {

cv::Mat create_hog_images_row(vv::HOGResult const& hog_result) {
  cv::Mat hog_magnitude_display;
  cv::Mat hog_magnitude_filtered_display;

  if (!hog_result.magnitude.empty()) {
    hog_result.magnitude.convertTo(hog_magnitude_display, CV_8U,
                                   vv::ImageConstants::kMaxPixelValue);
    cv::cvtColor(hog_magnitude_display, hog_magnitude_display,
                 cv::COLOR_GRAY2BGR);
  }
  if (!hog_result.magnitude_filtered.empty()) {
    hog_result.magnitude_filtered.convertTo(hog_magnitude_filtered_display,
                                            CV_8U,
                                            vv::ImageConstants::kMaxPixelValue);
    cv::cvtColor(hog_magnitude_filtered_display, hog_magnitude_filtered_display,
                 cv::COLOR_GRAY2BGR);
  }

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
