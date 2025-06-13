#include "vvp/visualization/layers/hog_drawing_layer.h"
#include "vvp/visualization/visualization_context.h"
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>

namespace vv::visualization {

void HogDrawingLayer::draw(VisualizationContext& context) {
  cv::Mat& canvas = context.getOutputImage();
  cv::Mat const& hog_image = context.getHogVisualization();

  if (hog_image.empty()) {
    return;
  }

  if (canvas.empty()) {
    canvas = hog_image.clone();
    return;
  }

  cv::Mat hog_resized;
  // HOG 이미지의 높이를 캔버스 높이에 맞게 조절
  cv::resize(
      hog_image, hog_resized,
      cv::Size(hog_image.cols * canvas.rows / hog_image.rows, canvas.rows));

  cv::hconcat(canvas, hog_resized, canvas);
}

}  // namespace vv::visualization
