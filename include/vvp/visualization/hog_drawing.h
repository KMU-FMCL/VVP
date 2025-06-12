// Placeholder for hog_drawing.h

#ifndef VVP_VISUALIZATION_HOG_DRAWING_H_
#define VVP_VISUALIZATION_HOG_DRAWING_H_

#include <opencv2/core/mat.hpp>

// Forward declarations if needed
namespace vv {
struct HOGResult;
struct HOGParams;
}  // namespace vv

namespace vv {
namespace visualization {

cv::Mat create_hog_images_row(vv::HOGResult const& hog_result);

}  // namespace visualization
}  // namespace vv

#endif  // VVP_VISUALIZATION_HOG_DRAWING_H_
