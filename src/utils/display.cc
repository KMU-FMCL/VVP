#include "vvp/utils/display.h"
#include "opencv2/highgui.hpp"  // For cv::imshow, cv::waitKey

namespace vv::utils {

auto DisplayUtils::display_frame(std::string const& window_name,
                                 cv::Mat const& frame, int wait_key_ms) -> int {
  cv::imshow(window_name, frame);
  return cv::waitKey(wait_key_ms);
}

}  // namespace vv::utils
