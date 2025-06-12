#ifndef VVP_UTILS_IMAGE_H_
#define VVP_UTILS_IMAGE_H_

#include <opencv2/core.hpp>

namespace vv {
namespace utils {

/**
 * @brief Resizes an image by a given scale factor.
 *
 * @param image The input image.
 * @param scale The scale factor. If scale <= 0 or scale == 1, the original
 * image is returned.
 * @return cv::Mat The resized image.
 */
auto resize_image(cv::Mat const& image, int scale) -> cv::Mat;

/**
 * @brief Rotates an image by a given angle around its center.
 *
 * @param image The input image.
 * @param angle The rotation angle in degrees.
 * @return cv::Mat The rotated image.
 */
auto rotate_image(cv::Mat const& image, double angle) -> cv::Mat;

}  // namespace utils
}  // namespace vv

#endif  // VVP_UTILS_IMAGE_H_
