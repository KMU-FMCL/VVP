#include "vvp/visualization/visualization.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_format.h"
#include "vvp/visualization/hog_drawing.h"
#include "vvp/visualization/info_display.h"  // 새 include 추가
#include "vvp/visualization/vv_indicator_drawing.h"
#include <opencv2/imgproc.hpp>  // For drawing functions and cvtColor
#include <cmath>                // For std::cos, std::sin
#include <iostream>             // For std::cerr
#include <vector>               // For std::vector

// Constants (ImageConstants, VisualizationConstants, AngleConstants) and
// Types (HOGResult, VVResult) are included via
// "vvp/utils/visualization.h" which includes "vvp/processing/constants.h"
// and "vvp/processing/types.h".

namespace {  // Anonymous namespace for helper functions
cv::Mat prepare_top_row(cv::Mat const& input_image,
                        cv::Mat const& calibrated_image,
                        vv::VVResult const& vv_result,
                        vv::VVParams const& vv_params) {
  // 원본 이미지에 VV 표시 추가
  cv::Mat input_with_vv = input_image.clone();
  if (!input_with_vv.empty()) {
    vv::visualization::draw_vv_indicators(input_with_vv, vv_result, vv_params);
  }

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
  cv::Mat top_row_result;
  if (!input_with_vv.empty() && !calibrated_with_line.empty()) {
    cv::hconcat(input_with_vv, calibrated_with_line, top_row_result);
  } else if (!input_with_vv.empty()) {
    top_row_result = input_with_vv;
  } else if (!calibrated_with_line.empty()) {
    top_row_result = calibrated_with_line;
  }
  // If both are empty, top_row_result remains empty.
  return top_row_result;
}

cv::Mat prepare_histogram_image(cv::Mat const& original_histogram_image,
                                cv::Mat const& reference_row_for_size) {
  cv::Mat hist_img = original_histogram_image.clone();

  if (!reference_row_for_size
           .empty()) {  // Operations based on reference_row_for_size dimensions
    if (hist_img.empty()) {
      hist_img = cv::Mat(reference_row_for_size.rows /
                             static_cast<int>(vv::ImageConstants::kDivideByTwo),
                         reference_row_for_size.cols, CV_8UC3,
                         vv::ImageConstants::Colors::kWhite);
    } else if (hist_img.cols != reference_row_for_size.cols ||
               (hist_img.rows !=
                    reference_row_for_size.rows /
                        static_cast<int>(vv::ImageConstants::kDivideByTwo) &&
                reference_row_for_size.rows /
                        static_cast<int>(vv::ImageConstants::kDivideByTwo) >
                    0)) {
      int target_hist_height =
          reference_row_for_size.rows /
          static_cast<int>(vv::ImageConstants::kDivideByTwo);
      if (target_hist_height <= 0 && hist_img.rows > 0)
        target_hist_height =
            hist_img.rows;           // keep original if target is non-positive
      if (target_hist_height > 0) {  // only resize if target height is positive
        cv::resize(hist_img, hist_img,
                   cv::Size(reference_row_for_size.cols, target_hist_height));
      } else if (hist_img.cols !=
                 reference_row_for_size
                     .cols) {  // if target height is 0, just match width
        cv::Mat temp_hist = cv::Mat(hist_img.rows, reference_row_for_size.cols,
                                    hist_img.type());
        cv::resize(hist_img, temp_hist, temp_hist.size());
        hist_img = temp_hist;
      }
    }
  } else if (hist_img.empty()) {
    // If reference_row_for_size is empty, and histogram is also empty,
    // create a default small white image.
    // This case implies no other visual content to base dimensions on.
    int default_width = 200;   // Default width
    int default_height = 100;  // Default height
    hist_img = cv::Mat(default_height, default_width, CV_8UC3,
                       vv::ImageConstants::Colors::kWhite);
  }
  // If reference_row_for_size is empty but hist_img is not, hist_img is
  // returned as is.
  return hist_img;
}

cv::Mat assemble_final_image(cv::Mat const& top_row, cv::Mat const& middle_row,
                             cv::Mat const& histogram_row) {
  cv::Mat assembled_result;
  std::vector<cv::Mat> rows_to_concat;
  if (!top_row.empty()) rows_to_concat.push_back(top_row);
  if (!middle_row.empty()) rows_to_concat.push_back(middle_row);
  if (!histogram_row.empty()) rows_to_concat.push_back(histogram_row);

  if (!rows_to_concat.empty()) {
    try {
      cv::vconcat(rows_to_concat, assembled_result);
    } catch (cv::Exception const& e) {
      std::cerr << "OpenCV Exception in vconcat: " << e.what() << std::endl;
      // Handle error: e.g., return the first available image or an empty Mat
      if (!rows_to_concat.empty())
        assembled_result = rows_to_concat[0].clone();
      else
        assembled_result =
            cv::Mat();  // Should not happen if rows_to_concat was not empty
    }
  } else {
    assembled_result = cv::Mat();  // All parts are empty, return empty Mat
  }
  return assembled_result;
}

}  // namespace

namespace vv {
namespace visualization {

// Originally from ImageProcessor class, responsible for creating the main
// visualization
auto create_visualization(cv::Mat const& input_image,
                          cv::Mat const& calibrated_image,
                          vv::HOGResult const& hog_result,
                          vv::VVResult const& vv_result,
                          vv::VVParams const& vv_params,
                          cv::Mat const& histogram_image, float fps)
    -> cv::Mat {
  cv::Mat top_row =
      prepare_top_row(input_image, calibrated_image, vv_result, vv_params);

  // HOG 결과 이미지를 새 모듈 함수를 호출하여 생성
  cv::Mat middle_row = vv::visualization::create_hog_images_row(hog_result);

  // 모든 행 세로로 합치기 전에 크기 조정 확인
  if (!middle_row.empty() && !top_row.empty() &&
      middle_row.size() != top_row.size()) {
    cv::resize(middle_row, middle_row, top_row.size());
  }

  cv::Mat reference_row_for_hist_size = top_row.empty() ? middle_row : top_row;
  cv::Mat final_histogram_image =
      prepare_histogram_image(histogram_image, reference_row_for_hist_size);

  cv::Mat result =
      assemble_final_image(top_row, middle_row, final_histogram_image);

  // FPS 정보 추가
  if (!result.empty()) {  // fps > 0.0F 조건은 draw_fps_info 내부에서 처리
    vv::visualization::draw_fps_info(result, fps);
  }

  return result;
}

}  // namespace visualization
}  // namespace vv
