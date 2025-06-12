#include "vvp/utils/image_utils.h"
#include "vvp/processing/constants.h"  // For ImageConstants::kDivideByTwo
#include <opencv2/imgproc.hpp>

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

}  // namespace utils
}  // namespace vv
