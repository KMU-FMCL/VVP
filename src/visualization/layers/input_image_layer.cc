#include "vvp/visualization/layers/input_image_layer.h"
#include "vvp/visualization/visualization_context.h"
#include <opencv2/imgproc.hpp>  // For potential drawing functions, if needed in future
#include <iostream>             // For std::cerr

namespace vv::visualization {

InputImageLayer::InputImageLayer() = default;

void InputImageLayer::draw(VisualizationContext& context) {
  // Check if input_image exists and is not empty
  cv::Mat const& input = context.getInputImage();
  if (input.empty()) {
    std::cerr << "InputImageLayer: Input image is empty. Skipping."
              << std::endl;
    return;
  }

  // For InputImageLayer, we typically copy the input image to the
  // output/canvas. If output_image is not yet initialized or has a different
  // size/type, copyTo will handle it.
  cv::Mat& output = context.getOutputImage();
  // cv::Mat::copyTo handles allocation if 'output' is empty or has a different
  // size/type.
  input.copyTo(output);
}

}  // namespace vv::visualization
