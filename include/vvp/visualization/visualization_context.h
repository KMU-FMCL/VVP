#ifndef VVP_VISUALIZATION_VISUALIZATION_CONTEXT_H_
#define VVP_VISUALIZATION_VISUALIZATION_CONTEXT_H_

#include <opencv2/core/mat.hpp>
#include <vector>  // For potential future use with multiple images or data points

// Forward declarations if needed by context members
// struct VVResult;
// struct HOGResult;
// struct VVParams;

namespace vv::visualization {

struct VisualizationContext {
  cv::Mat& output_image;  // The final image to draw on
  cv::Mat input_image;  // The primary input image for visualization. Copied for
                        // safety.

  // Constructor
  explicit VisualizationContext(cv::Mat& out_img, cv::Mat const& in_img)
      : output_image(out_img),
        input_image(in_img.clone()) {}  // Clone input_image

  // Prevent copying to avoid issues with reference members and ensure clarity
  // on Mat ownership
  VisualizationContext(VisualizationContext const&) = delete;
  VisualizationContext& operator=(VisualizationContext const&) = delete;

  // Allow moving
  VisualizationContext(VisualizationContext&&) =
      default;  // Move construction is fine
  VisualizationContext& operator=(VisualizationContext&&) =
      delete;  // Move assignment is deleted due to reference member

  // Placeholder for other data needed by layers
  // const vv::HOGResult* hog_result = nullptr;
  // const vv::VVResult* vv_result = nullptr;
  // const vv::VVParams* vv_params = nullptr;
  // const cv::Mat* hog_histogram_image = nullptr;
  // double fps = 0.0;
};

}  // namespace vv::visualization

#endif  // VVP_VISUALIZATION_VISUALIZATION_CONTEXT_H_
