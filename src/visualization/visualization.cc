#include "vvp/visualization/visualization.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_format.h"
#include <opencv2/imgproc.hpp>  // For drawing functions and cvtColor
#include <cmath>                // For std::cos, std::sin
#include <iostream>             // For std::cerr
#include <vector>               // For std::vector

// Constants (ImageConstants, VisualizationConstants, AngleConstants) and
// Types (HOGResult, VVResult) are included via
// "vvp/utils/visualization.h" which includes "vvp/processing/constants.h"
// and "vvp/processing/types.h".

namespace {  // Anonymous namespace for helper functions

// Originally from ImageProcessor class, responsible for drawing VV indicators
auto draw_vv_indicators_internal(cv::Mat image, vv::VVResult const& vv_result)
    -> cv::Mat {
  try {
    // VV 각도 텍스트 추가
    cv::putText(
        image, absl::StrCat(" VV_dig=", static_cast<int>(vv_result.angle)),
        cv::Point(vv::VisualizationConstants::kVvTextPositionX,
                  vv::VisualizationConstants::kVvTextPositionY),
        cv::FONT_HERSHEY_PLAIN, vv::VisualizationConstants::kVvTextScale,
        vv::ImageConstants::Colors::kGreen,
        vv::VisualizationConstants::kLineThickness, cv::LINE_AA);

    // 수평선과 수직선 추가
    cv::line(image,
             cv::Point(0, image.rows / static_cast<int>(
                                           vv::ImageConstants::kDivideByTwo)),
             cv::Point(image.cols,
                       image.rows /
                           static_cast<int>(vv::ImageConstants::kDivideByTwo)),
             vv::ImageConstants::Colors::kBlack,
             vv::VisualizationConstants::kLineThickness, cv::LINE_4);

    cv::line(
        image,
        cv::Point(
            image.cols / static_cast<int>(vv::ImageConstants::kDivideByTwo),
            image.rows / static_cast<int>(vv::ImageConstants::kDivideByTwo)),
        cv::Point(
            image.cols / static_cast<int>(vv::ImageConstants::kDivideByTwo),
            image.rows),
        vv::ImageConstants::Colors::kBlack,
        vv::VisualizationConstants::kLineThickness, cv::LINE_4);

    // VV 선 그리기
    double radians = (vv::AngleConstants::kRightAngle - vv_result.angle) *
                     vv::AngleConstants::kDegToRad;
    double length = static_cast<double>(image.rows) /
                    static_cast<double>(vv::ImageConstants::kDivideByTwo);
    double delta_x = length * std::cos(radians);
    double delta_y = length * std::sin(radians);

    cv::Point center(
        image.cols / static_cast<int>(vv::ImageConstants::kDivideByTwo),
        image.rows / static_cast<int>(vv::ImageConstants::kDivideByTwo));
    cv::Point end(static_cast<int>(center.x + delta_x),
                  static_cast<int>(center.y - delta_y));

    cv::line(image, center, end, vv::ImageConstants::Colors::kGreen,
             vv::VisualizationConstants::kLineThickness, cv::LINE_AA);

    // 가속도 벡터 그리기 (acc_x, acc_y)
    double acc_scale_factor = length / vv::AngleConstants::kGravityAcceleration;
    cv::Point acc_vec(
        static_cast<int>(center.x + (vv_result.acc_x * acc_scale_factor)),
        static_cast<int>(center.y - (vv_result.acc_y * acc_scale_factor)));
    cv::arrowedLine(image, center, acc_vec, vv::ImageConstants::Colors::kRed,
                    vv::VisualizationConstants::kLineThickness, cv::LINE_AA);

    return image;
  } catch (std::exception const& e) {
    std::cerr << "Error drawing VV indicators: " << e.what() << '\n';
    return image;  // Return original image on error
  }
}

}  // namespace

namespace vv {
namespace utils {

// Originally from ImageProcessor class, responsible for creating the main
// visualization
auto create_visualization(cv::Mat const& input_image,
                          cv::Mat const& calibrated_image,
                          vv::HOGResult const& hog_result,
                          vv::VVResult const& vv_result,
                          cv::Mat const& histogram_image, float fps)
    -> cv::Mat {
  // 원본 이미지에 VV 표시 추가
  cv::Mat input_with_vv =
      draw_vv_indicators_internal(input_image.clone(), vv_result);

  // 보정된 이미지에 수평선 추가
  cv::Mat calibrated_with_line = calibrated_image.clone();
  if (!calibrated_with_line.empty()) {
    cv::line(
        calibrated_with_line,
        cv::Point(0, calibrated_with_line.rows /
                         static_cast<int>(vv::ImageConstants::kDivideByTwo)),
        cv::Point(calibrated_with_line.cols,
                  calibrated_with_line.rows /
                      static_cast<int>(vv::ImageConstants::kDivideByTwo)),
        vv::ImageConstants::Colors::kBlack,
        vv::VisualizationConstants::kLineThickness, cv::LINE_AA);
  }

  // 상단 이미지 가로로 합치기 (원본 + 보정)
  cv::Mat top_row;
  if (!input_with_vv.empty() && !calibrated_with_line.empty()) {
    cv::hconcat(input_with_vv, calibrated_with_line, top_row);
  } else if (!input_with_vv.empty()) {
    top_row = input_with_vv;
  } else if (!calibrated_with_line.empty()) {
    top_row = calibrated_with_line;
  }
  // If both are empty, top_row remains empty.

  // HOG 결과 이미지 생성
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

  // 중간 이미지 가로로 합치기 (HOG 매그니튜드 + 필터링된 매그니튜드)
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
  // If both are empty, middle_row remains empty.

  // 모든 행 세로로 합치기 전에 크기 조정 확인
  if (!middle_row.empty() && !top_row.empty() &&
      middle_row.size() != top_row.size()) {
    cv::resize(middle_row, middle_row, top_row.size());
  }

  // 히스토그램 이미지가 없거나 너비가 다른 경우 수정
  cv::Mat final_histogram_image = histogram_image;
  if (!top_row.empty()) {  // Operations based on top_row dimensions
    if (final_histogram_image.empty()) {
      final_histogram_image = cv::Mat(
          top_row.rows / static_cast<int>(vv::ImageConstants::kDivideByTwo),
          top_row.cols, CV_8UC3, vv::ImageConstants::Colors::kWhite);
    } else if (final_histogram_image.cols != top_row.cols ||
               (final_histogram_image.rows !=
                    top_row.rows /
                        static_cast<int>(vv::ImageConstants::kDivideByTwo) &&
                top_row.rows /
                        static_cast<int>(vv::ImageConstants::kDivideByTwo) >
                    0)) {
      // Adjust size if cols don't match or if rows don't match the intended
      // half-height of top_row
      int target_hist_height =
          top_row.rows / static_cast<int>(vv::ImageConstants::kDivideByTwo);
      if (target_hist_height <= 0 && final_histogram_image.rows > 0)
        target_hist_height =
            final_histogram_image
                .rows;               // keep original if target is non-positive
      if (target_hist_height > 0) {  // only resize if target height is positive
        cv::resize(final_histogram_image, final_histogram_image,
                   cv::Size(top_row.cols, target_hist_height));
      } else if (final_histogram_image.cols !=
                 top_row.cols) {  // if target height is 0, just match width
        cv::Mat temp_hist = cv::Mat(final_histogram_image.rows, top_row.cols,
                                    final_histogram_image.type());
        cv::resize(final_histogram_image, temp_hist, temp_hist.size());
        final_histogram_image = temp_hist;
      }
    }
  } else if (final_histogram_image.empty() &&
             (!middle_row.empty() || !hog_magnitude_display.empty() ||
              !hog_magnitude_filtered_display.empty())) {
    // If top_row is empty, but other content exists, create a default histogram
    // placeholder This case needs careful handling based on desired output when
    // top_row is missing. For now, let's assume if top_row is empty, histogram
    // might be based on middle_row or be a default size. This part of logic
    // might need refinement based on expected behavior for empty top_row.
    // Creating a small default white image if other rows are present.
    int default_width;
    if (!middle_row.empty()) {
      default_width = middle_row.cols;
    } else if (!hog_magnitude_display.empty()) {
      default_width = hog_magnitude_display.cols;
    } else if (!hog_magnitude_filtered_display.empty()) {
      default_width = hog_magnitude_filtered_display.cols;
    } else {
      default_width = 200;  // Default width if all relevant images are empty
    }
    int default_height =
        middle_row.empty()
            ? 100
            : middle_row.rows /
                  static_cast<int>(vv::ImageConstants::kDivideByTwo);
    if (default_height <= 0) default_height = 50;  // ensure positive height
    if (default_width <= 0) default_width = 200;   // ensure positive width
    final_histogram_image = cv::Mat(default_height, default_width, CV_8UC3,
                                    vv::ImageConstants::Colors::kWhite);
  }

  // 수직 합치기
  cv::Mat result;
  std::vector<cv::Mat> rows_to_concat;
  if (!top_row.empty()) rows_to_concat.push_back(top_row);
  if (!middle_row.empty()) rows_to_concat.push_back(middle_row);
  if (!final_histogram_image.empty())
    rows_to_concat.push_back(final_histogram_image);

  if (!rows_to_concat.empty()) {
    try {
      cv::vconcat(rows_to_concat, result);
    } catch (cv::Exception const& e) {
      std::cerr << "OpenCV Exception in vconcat: " << e.what() << std::endl;
      // Handle error: e.g., return the first available image or an empty Mat
      if (!rows_to_concat.empty())
        result = rows_to_concat[0].clone();
      else
        result = cv::Mat();
    }
  } else {
    result = cv::Mat();  // All parts are empty, return empty Mat
  }

  // FPS 정보 추가
  if (!result.empty() && fps > 0.0F) {
    cv::putText(result, absl::StrFormat("FPS: %.1f", fps),
                cv::Point(vv::VisualizationConstants::kFpsTextPositionX,
                          vv::VisualizationConstants::kFpsTextPositionY),
                cv::FONT_HERSHEY_SIMPLEX,
                vv::VisualizationConstants::kFpsTextScale,
                vv::ImageConstants::Colors::kGreen,
                vv::VisualizationConstants::kLineThickness);
  }

  return result;
}

}  // namespace utils
}  // namespace vv
