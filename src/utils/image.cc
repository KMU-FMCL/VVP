#include "vvp/utils/image.h"
#include "vvp/processing/constants.h"  // For ImageConstants::kDivideByTwo
#include <opencv2/imgproc.hpp>
#include <iostream>  // For std::cerr in assemble_final_image
#include <vector>    // For std::vector in assemble_final_image

namespace vv {
namespace utils {

auto resize_image(cv::Mat const& image, int scale) -> cv::Mat {
  if (scale <= 0 || scale == 1) {
    return image.clone();
  }

  cv::Mat resized;
  cv::resize(image, resized, cv::Size(image.cols / scale, image.rows / scale),
             0, 0, cv::INTER_LINEAR);

  return resized;
}

auto rotate_image(cv::Mat const& image, double angle) -> cv::Mat {
  cv::Point2f center(
      static_cast<float>(image.cols) / ImageConstants::kDivideByTwo,
      static_cast<float>(image.rows) / ImageConstants::kDivideByTwo);
  cv::Mat rot_mat = cv::getRotationMatrix2D(center, angle, 1.0);
  cv::Mat rotated;

  cv::warpAffine(image, rotated, rot_mat, image.size(), cv::INTER_LINEAR);

  return rotated;
}

auto assemble_final_image(cv::Mat const& top_row, cv::Mat const& middle_row,
                          cv::Mat const& histogram_row) -> cv::Mat {
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

}  // namespace utils
}  // namespace vv
