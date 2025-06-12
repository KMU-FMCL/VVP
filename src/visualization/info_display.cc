// Placeholder for info_display.cc

#include "vvp/visualization/info_display.h"
#include "vvp/fps/fps_counter.h"  // For FPSCounter class
#include <opencv2/imgproc.hpp>    // For cv::putText

namespace vv {
namespace visualization {

void draw_fps_info(cv::Mat& canvas, vv::FPSData const& fps_data) {
  // Implementation will be moved here
}

void draw_source_info(cv::Mat& canvas, std::string const& source_name) {
  // Implementation will be moved here
}

}  // namespace visualization
}  // namespace vv
