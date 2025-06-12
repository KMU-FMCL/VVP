// Placeholder for vv_indicator_drawing.h

#ifndef VVP_VISUALIZATION_VV_INDICATOR_DRAWING_H_
#define VVP_VISUALIZATION_VV_INDICATOR_DRAWING_H_

#include <opencv2/core/mat.hpp>

// Forward declarations if needed
namespace vv {
struct VVResult;
struct VVParams;
}  // namespace vv

namespace vv {
namespace visualization {

void draw_vv_indicators(cv::Mat& canvas, vv::VVResult const& vv_result,
                        vv::VVParams const& vv_params);

}  // namespace visualization
}  // namespace vv

#endif  // VVP_VISUALIZATION_VV_INDICATOR_DRAWING_H_
