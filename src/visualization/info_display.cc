// Placeholder for info_display.cc

#include "vvp/visualization/info_display.h"
#include "absl/strings/str_format.h"  // For absl::StrFormat
#include <opencv2/imgproc.hpp>  // For cv::putText, cv::FONT_HERSHEY_SIMPLEX

// constants.h는 info_display.h를 통해 포함됨

namespace vv {
namespace visualization {

void draw_fps_info(cv::Mat& image, float fps) {            // 수정된 시그니처
  if (!image.empty() && fps > 0.0F) {                      // 직접 fps 사용
    cv::putText(image, absl::StrFormat("FPS: %.1f", fps),  // 직접 fps 사용
                cv::Point(VisualizationConstants::kFpsTextPositionX,
                          VisualizationConstants::kFpsTextPositionY),
                cv::FONT_HERSHEY_SIMPLEX, VisualizationConstants::kFpsTextScale,
                ImageConstants::Colors::kGreen,
                VisualizationConstants::kLineThickness);
  }
}

// draw_source_info 함수는 아직 구현하지 않음 (필요시 추후 구현)
// 사용자 요청 시 또는 다음 단계에서 파일 이름 등을 표시할 때 이 함수를 구체화할
// 수 있습니다.
void draw_source_info(cv::Mat& /*canvas*/, std::string const& /*source_name*/) {
  // TODO(luen): Implement to draw source name or other info if needed
}

}  // namespace visualization
}  // namespace vv
