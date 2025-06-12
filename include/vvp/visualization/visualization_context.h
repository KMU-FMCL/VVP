#ifndef VVP_VISUALIZATION_VISUALIZATION_CONTEXT_H_
#define VVP_VISUALIZATION_VISUALIZATION_CONTEXT_H_

#include <opencv2/core/mat.hpp>
#include <vector>  // For potential future use with multiple images or data points

// Forward declarations if needed by context members
// struct VVResult;
// struct HOGResult;
// struct VVParams;

namespace vv {
namespace visualization {

struct VisualizationContext {
  cv::Mat& output_image;  // The final image to draw on

  // Constructor
  explicit VisualizationContext(cv::Mat& out_img) : output_image(out_img) {}

  // Prevent copying to avoid issues with reference members
  VisualizationContext(VisualizationContext const&) = delete;
  VisualizationContext& operator=(VisualizationContext const&) = delete;

  // Allow moving
  VisualizationContext(VisualizationContext&&) = default;
  VisualizationContext& operator=(VisualizationContext&&) = default;

  // Placeholder for other data needed by layers
  // cv::Mat input_image;
  // const vv::HOGResult* hog_result = nullptr;
  // const vv::VVResult* vv_result = nullptr;
  // const vv::VVParams* vv_params = nullptr;
  // const cv::Mat* hog_histogram_image = nullptr;
  // double fps = 0.0;
};

}  // namespace visualization
}  // namespace vv

#endif  // VVP_VISUALIZATION_VISUALIZATION_CONTEXT_H_
