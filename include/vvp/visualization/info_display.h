// Placeholder for info_display.h

#ifndef VVP_VISUALIZATION_INFO_DISPLAY_H_
#define VVP_VISUALIZATION_INFO_DISPLAY_H_

#include "vvp/processing/constants.h"  // For ImageConstants::Colors and VisualizationConstants
#include <opencv2/core/mat.hpp>  // OpenCV Mat 타입을 위해 필요
#include <string>

namespace vv {
namespace visualization {

/**
 * @brief Draws FPS information on the given image.
 *
 * @param image The image to draw on.
 * @param fps The current FPS value.
 */
void draw_fps_info(cv::Mat& image, float fps);

// 기존 draw_source_info 함수는 그대로 둡니다.
void draw_source_info(cv::Mat& canvas, std::string const& source_name);

}  // namespace visualization
}  // namespace vv

#endif  // VVP_VISUALIZATION_INFO_DISPLAY_H_
