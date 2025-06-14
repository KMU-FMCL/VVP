// src/visualization/visualization.cc
#include "vvp/visualization/visualization.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_format.h"
#include "vvp/utils/image.h"
#include "vvp/visualization/hog_drawing.h"
#include "vvp/visualization/info_display.h"
#include "vvp/visualization/layers/hog_drawing_layer.h"
#include "vvp/visualization/layers/input_image_layer.h"
#include "vvp/visualization/layers/vv_indicator_drawing_layer.h"
#include "vvp/visualization/visualization_context.h"  // Added
#include "vvp/visualization/vv_indicator_drawing.h"
#include <opencv2/imgproc.hpp>
#include <cmath>

namespace {  // Anonymous namespace for helper functions
// Helper functions (prepare_top_row, get_target_histogram_size,
// prepare_histogram_image) remain unchanged for now, but will take data from
// context via create_visualization
cv::Mat prepare_top_row(vv::visualization::VisualizationContext& main_context) {
  cv::Mat const& input_image = main_context.getInputImage();
  cv::Mat const& calibrated_image = main_context.getCalibratedImage();

  cv::Mat input_with_vv;
  if (!input_image.empty()) {
    input_with_vv = input_image.clone();  // Work on a copy

    // Prepare a context specifically for the VvIndicatorDrawingLayer
    vv::visualization::VisualizationContext layer_context;
    layer_context.setOutputImage(
        input_with_vv);  // Layer will draw on its internal copy of
                         // input_with_vv
    layer_context.setVvResult(main_context.getVvResult());
    layer_context.setVvParams(main_context.getVvParams());
    // TODO(Cascade): Copy other relevant parts from main_context if
    // VvIndicatorDrawingLayer needs them in the future

    vv::VvIndicatorDrawingLayer::VvIndicatorDrawingLayer vv_layer;
    vv_layer.draw(layer_context);  // Modifies layer_context.output_image_

    input_with_vv = layer_context.getOutputImage();  // Get the modified image
  } else {
    input_with_vv = cv::Mat();  // Ensure it's an empty Mat if input was empty
  }

  cv::Mat calibrated_with_line;
  if (!calibrated_image.empty()) {
    calibrated_with_line = calibrated_image.clone();
    cv::line(
        calibrated_with_line,
        cv::Point(0, calibrated_with_line.rows /
                         static_cast<int>(vv::ImageConstants::kDivideByTwo)),
        cv::Point(calibrated_with_line.cols,
                  calibrated_with_line.rows /
                      static_cast<int>(vv::ImageConstants::kDivideByTwo)),
        vv::ImageConstants::Colors::kBlack,
        vv::VisualizationConstants::kLineThickness, cv::LINE_AA);
  } else {
    calibrated_with_line = cv::Mat();  // Ensure it's an empty Mat
  }

  cv::Mat top_row_result;
  if (!input_with_vv.empty() && !calibrated_with_line.empty()) {
    cv::hconcat(input_with_vv, calibrated_with_line, top_row_result);
  } else if (!input_with_vv.empty()) {
    top_row_result = input_with_vv;
  } else if (!calibrated_with_line.empty()) {
    top_row_result = calibrated_with_line;
  }
  // If both are empty, top_row_result will be an empty Mat by default.
  return top_row_result;
}

cv::Size get_target_histogram_size(cv::Mat const& original_histogram_image,
                                   cv::Mat const& reference_row_for_size) {
  int target_width;
  int target_height;

  if (!reference_row_for_size.empty()) {
    target_width = reference_row_for_size.cols;
    target_height = reference_row_for_size.rows /
                    static_cast<int>(vv::ImageConstants::kDivideByTwo);
    if (target_height <= 0 && !original_histogram_image.empty()) {
      target_height = original_histogram_image.rows;
    }
  } else if (!original_histogram_image.empty()) {
    return original_histogram_image.size();
  } else {
    target_width = 200;
    target_height = 100;
  }
  if (target_height <= 0) target_height = 100;

  return cv::Size(target_width, target_height);
}

cv::Mat prepare_histogram_image(
    cv::Mat const&
        original_histogram_image,  // from context.getHistogramImage()
    cv::Mat const& reference_row_for_size) {
  cv::Mat hist_img = original_histogram_image.clone();
  cv::Size target_size = get_target_histogram_size(original_histogram_image,
                                                   reference_row_for_size);

  if (hist_img.empty()) {
    hist_img =
        cv::Mat(target_size, CV_8UC3, vv::ImageConstants::Colors::kWhite);
  } else if (hist_img.size() != target_size) {
    cv::resize(hist_img, hist_img, target_size);
  }
  return hist_img;
}

}  // namespace

namespace vv::visualization {

auto create_visualization(VisualizationContext& context) -> cv::Mat {
  InputImageLayer input_layer;
  input_layer.draw(context);

  HogDrawingLayer hog_layer;
  hog_layer.draw(context);
  // 1. Get input image using InputImageLayer
  // For now, directly use the image from context, assuming it's already set.
  // In a full layer-based system, you'd do:
  // InputImageLayer input_layer;
  // cv::Mat base_image = input_layer.draw(context);
  // context.setOutputImage(base_image); // Or similar logic
  // For this step, we'll assume context.getInputImage() is the base.
  // And context.getOutputImage() will be built upon.

  // Retrieve data from context
  cv::Mat const& input_image = context.getInputImage();
  cv::Mat const& calibrated_image = context.getCalibratedImage();
  vv::HOGResult const& hog_result = context.getHogResult();
  vv::VVResult const& vv_result = context.getVvResult();
  vv::VVParams const& vv_params = context.getVvParams();
  cv::Mat const& histogram_image = context.getHistogramImage();
  float fps = context.getFps();

  // --- Original logic using data from context ---
  cv::Mat top_row = prepare_top_row(context);

  cv::Mat middle_row = vv::visualization::create_hog_images_row(hog_result);

  if (!middle_row.empty() && !top_row.empty() &&
      middle_row.cols != top_row.cols) {  // Check width for consistency
    // Attempt to resize middle_row to match top_row's width, maintaining aspect
    // ratio or specific logic This part might need careful handling depending
    // on desired output For simplicity, if widths differ significantly, we
    // might skip or log error Or, if top_row is the reference, resize
    // middle_row to top_row.size() as before
    cv::resize(middle_row, middle_row,
               cv::Size(top_row.cols, middle_row.rows));  // Resize width only
  }
  if (!middle_row.empty() && !top_row.empty() &&
      middle_row.size() != top_row.size()) {  // Fallback to original resize if
                                              // width adjustment isn't enough
    cv::resize(middle_row, middle_row, top_row.size());
  }

  cv::Mat reference_row_for_hist_size = top_row.empty() ? middle_row : top_row;
  cv::Mat final_histogram_image =
      prepare_histogram_image(histogram_image, reference_row_for_hist_size);

  cv::Mat result = vv::utils::assemble_final_image(top_row, middle_row,
                                                   final_histogram_image);

  if (!result.empty()) {
    vv::visualization::draw_fps_info(result, fps);
  }

  context.setOutputImage(result);

  return context.getOutputImage();
}

}  // namespace vv::visualization
