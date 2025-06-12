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

void draw_hog_elements(cv::Mat& canvas, vv::HOGResult const& hog_result,
                       vv::HOGParams const& hog_params);

}  // namespace visualization
}  // namespace vv

#endif  // VVP_VISUALIZATION_HOG_DRAWING_H_
