// Placeholder for vv_indicator_drawing.cc

#include "vvp/visualization/vv_indicator_drawing.h"
#include "absl/strings/str_cat.h"  // For absl::StrCat
#include "opencv2/imgproc.hpp"     // For cv::putText, cv::line, cv::arrowedLine
#include "vvp/estimation/types.h"  // For VVResult, VVParams
#include "vvp/processing/constants.h"  // For VisualizationConstants, ImageConstants, AngleConstants
#include <iostream>  // For std::cerr
#include <string>    // For std::string (though absl::StrCat is used)

namespace vv {
namespace visualization {

void draw_vv_indicators(cv::Mat& canvas, vv::VVResult const& vv_result,
                        [[maybe_unused]] vv::VVParams const& vv_params) {
  try {
    // VV 각도 텍스트 추가
    cv::putText(
        canvas, absl::StrCat(" VV_dig=", static_cast<int>(vv_result.angle)),
        cv::Point(vv::VisualizationConstants::kVvTextPositionX,
                  vv::VisualizationConstants::kVvTextPositionY),
        cv::FONT_HERSHEY_PLAIN, vv::VisualizationConstants::kVvTextScale,
        vv::ImageConstants::Colors::kGreen,
        vv::VisualizationConstants::kLineThickness, cv::LINE_AA);

    // 수평선과 수직선 추가
    cv::line(canvas,
             cv::Point(0, canvas.rows / static_cast<int>(
                                            vv::ImageConstants::kDivideByTwo)),
             cv::Point(canvas.cols,
                       canvas.rows /
                           static_cast<int>(vv::ImageConstants::kDivideByTwo)),
             vv::ImageConstants::Colors::kBlack,
             vv::VisualizationConstants::kLineThickness, cv::LINE_4);

    cv::line(
        canvas,
        cv::Point(
            canvas.cols / static_cast<int>(vv::ImageConstants::kDivideByTwo),
            canvas.rows / static_cast<int>(vv::ImageConstants::kDivideByTwo)),
        cv::Point(
            canvas.cols / static_cast<int>(vv::ImageConstants::kDivideByTwo),
            canvas.rows),
        vv::ImageConstants::Colors::kBlack,
        vv::VisualizationConstants::kLineThickness, cv::LINE_4);

    // VV 선 그리기
    double radians = (vv::AngleConstants::kRightAngle - vv_result.angle) *
                     vv::AngleConstants::kDegToRad;
    double length = static_cast<double>(canvas.rows) /
                    static_cast<double>(vv::ImageConstants::kDivideByTwo);
    double delta_x = length * std::cos(radians);
    double delta_y = length * std::sin(radians);

    cv::Point center(
        canvas.cols / static_cast<int>(vv::ImageConstants::kDivideByTwo),
        canvas.rows / static_cast<int>(vv::ImageConstants::kDivideByTwo));
    cv::Point end(static_cast<int>(center.x + delta_x),
                  static_cast<int>(center.y - delta_y));

    cv::line(canvas, center, end, vv::ImageConstants::Colors::kGreen,
             vv::VisualizationConstants::kLineThickness, cv::LINE_AA);

    // 가속도 벡터 그리기 (acc_x, acc_y)
    // Note: Ensure AngleConstants::kGravityAcceleration is appropriate for
    // scaling or consider passing a scale factor via vv_params if it can vary.
    double acc_scale_factor = length / vv::AngleConstants::kGravityAcceleration;
    cv::Point acc_vec(
        static_cast<int>(center.x + (vv_result.acc_x * acc_scale_factor)),
        static_cast<int>(center.y - (vv_result.acc_y * acc_scale_factor)));
    cv::arrowedLine(canvas, center, acc_vec, vv::ImageConstants::Colors::kRed,
                    vv::VisualizationConstants::kLineThickness, cv::LINE_AA);

  } catch (std::exception const& e) {
    std::cerr << "Error drawing VV indicators: " << e.what() << '\n';
    // canvas is modified in place, so no return needed, but error is logged.
  }
}

}  // namespace visualization
}  // namespace vv
