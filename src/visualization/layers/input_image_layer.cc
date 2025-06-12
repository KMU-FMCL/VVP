#include "vvp/visualization/layers/input_image_layer.h"
#include <opencv2/imgproc.hpp>  // For cv::cvtColor if needed, or just for Mat operations

namespace vv::visualization {

InputImageLayer::InputImageLayer() = default;

void InputImageLayer::draw(VisualizationContext& context) {
  if (context.input_image.empty()) {
    // Or log an error/warning
    return;
  }
  // The output_image is a reference, so we draw onto it.
  // copyTo will handle allocation if output_image is empty or wrong size/type.
  context.input_image.copyTo(context.output_image);
}

}  // namespace vv::visualization
