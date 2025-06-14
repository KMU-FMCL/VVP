#include "vvp/visualization/layers/vv_indicator_drawing_layer.h"
#include "absl/strings/str_cat.h"  // For absl::StrCat
#include "opencv2/imgproc.hpp"     // For cv::putText, cv::line, cv::arrowedLine
#include "vvp/estimation/types.h"  // For VVResult, VVParams
#include "vvp/processing/constants.h"  // For VisualizationConstants, ImageConstants, AngleConstants
#include <iostream>  // For std::cerr
#include <string>    // For std::string (though absl::StrCat is used)

namespace {

void draw_angle_text(cv::Mat& canvas, double angle) {
  cv::putText(canvas, absl::StrCat(" VV_dig=", static_cast<int>(angle)),
              cv::Point(vv::VisualizationConstants::kVvTextPositionX,
                        vv::VisualizationConstants::kVvTextPositionY),
              cv::FONT_HERSHEY_PLAIN, vv::VisualizationConstants::kVvTextScale,
              vv::ImageConstants::Colors::kGreen,
              vv::VisualizationConstants::kLineThickness, cv::LINE_AA);
}

void draw_reference_lines(cv::Mat& canvas) {
  int const center_y =
      canvas.rows / static_cast<int>(vv::ImageConstants::kDivideByTwo);
  int const center_x =
      canvas.cols / static_cast<int>(vv::ImageConstants::kDivideByTwo);

  // Horizontal line
  cv::line(canvas, cv::Point(0, center_y), cv::Point(canvas.cols, center_y),
           vv::ImageConstants::Colors::kBlack,
           vv::VisualizationConstants::kLineThickness, cv::LINE_4);

  // Vertical line
  cv::line(canvas, cv::Point(center_x, center_y),
           cv::Point(center_x, canvas.rows), vv::ImageConstants::Colors::kBlack,
           vv::VisualizationConstants::kLineThickness, cv::LINE_4);
}

void draw_vv_line(cv::Mat& canvas, double angle) {
  double const radians =
      (vv::AngleConstants::kRightAngle - angle) * vv::AngleConstants::kDegToRad;
  double const length = static_cast<double>(canvas.rows) /
                        static_cast<double>(vv::ImageConstants::kDivideByTwo);
  double const delta_x = length * std::cos(radians);
  double const delta_y = length * std::sin(radians);

  cv::Point const center(
      canvas.cols / static_cast<int>(vv::ImageConstants::kDivideByTwo),
      canvas.rows / static_cast<int>(vv::ImageConstants::kDivideByTwo));
  cv::Point const end(static_cast<int>(center.x + delta_x),
                      static_cast<int>(center.y - delta_y));

  cv::line(canvas, center, end, vv::ImageConstants::Colors::kGreen,
           vv::VisualizationConstants::kLineThickness, cv::LINE_AA);
}

void draw_acceleration_vector(cv::Mat& canvas, double acc_x, double acc_y) {
  double const length = static_cast<double>(canvas.rows) /
                        static_cast<double>(vv::ImageConstants::kDivideByTwo);
  double const acc_scale_factor =
      length / vv::AngleConstants::kGravityAcceleration;
  cv::Point const center(
      canvas.cols / static_cast<int>(vv::ImageConstants::kDivideByTwo),
      canvas.rows / static_cast<int>(vv::ImageConstants::kDivideByTwo));
  cv::Point const acc_vec(
      static_cast<int>(center.x + (acc_x * acc_scale_factor)),
      static_cast<int>(center.y - (acc_y * acc_scale_factor)));

  cv::arrowedLine(canvas, center, acc_vec, vv::ImageConstants::Colors::kRed,
                  vv::VisualizationConstants::kLineThickness, cv::LINE_AA);
}

}  // namespace

namespace vv {
namespace VvIndicatorDrawingLayer {

void VvIndicatorDrawingLayer::draw(
    vv::visualization::VisualizationContext& context) {
  try {
    cv::Mat& canvas = context.getOutputImage();
    vv::VVResult const& vv_result = context.getVvResult();
    [[maybe_unused]] vv::VVParams const& vv_params = context.getVvParams();

    draw_angle_text(canvas, vv_result.angle);
    draw_reference_lines(canvas);
    draw_vv_line(canvas, vv_result.angle);
    draw_acceleration_vector(canvas, vv_result.acc_x, vv_result.acc_y);
  } catch (std::exception const& e) {
    std::cerr << "Error drawing VV indicators: " << e.what() << '\n';
  }
}

}  // namespace VvIndicatorDrawingLayer
}  // namespace vv
