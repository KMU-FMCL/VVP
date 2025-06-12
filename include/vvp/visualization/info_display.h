// Placeholder for info_display.h

#ifndef VVP_VISUALIZATION_INFO_DISPLAY_H_
#define VVP_VISUALIZATION_INFO_DISPLAY_H_

#include <opencv2/core/mat.hpp>
#include <string>

// Forward declarations if needed
namespace vv {
struct FPSData;
}

namespace vv {
namespace visualization {

void draw_fps_info(cv::Mat& canvas, vv::FPSData const& fps_data);
void draw_source_info(cv::Mat& canvas, std::string const& source_name);

}  // namespace visualization
}  // namespace vv

#endif  // VVP_VISUALIZATION_INFO_DISPLAY_H_
