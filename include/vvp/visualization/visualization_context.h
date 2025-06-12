// include/vvp/visualization/visualization_context.h

#ifndef VVP_VISUALIZATION_VISUALIZATION_CONTEXT_H_
#define VVP_VISUALIZATION_VISUALIZATION_CONTEXT_H_

#include "vvp/estimation/types.h"  // For HOGResult, VVResult, VVParams
#include <opencv2/core/mat.hpp>

namespace vv::visualization {

class VisualizationContext {
 public:
  VisualizationContext() = default;  // 기본 생성자 사용
  ~VisualizationContext() = default;

  // 복사 및 할당 금지
  VisualizationContext(VisualizationContext const&) = delete;
  VisualizationContext& operator=(VisualizationContext const&) = delete;

  // 이동 생성 및 할당 허용 (기본값으로도 충분하지만 명시)
  VisualizationContext(VisualizationContext&&) = default;
  VisualizationContext& operator=(VisualizationContext&&) = default;

  // --- Input Image (기존 멤버, setter/getter 스타일로 통일) ---
  void setInputImage(cv::Mat const& image) {
    input_image_ = image.clone();  // 안전을 위해 복사
  }
  cv::Mat const& getInputImage() const { return input_image_; }
  cv::Mat& getInputImage() {  // Non-const version if modification is needed
    return input_image_;
  }

  // --- Calibrated Image ---
  void setCalibratedImage(cv::Mat const& image) {
    calibrated_image_ = image.clone();  // 안전을 위해 복사
  }
  cv::Mat const& getCalibratedImage() const { return calibrated_image_; }
  cv::Mat& getCalibratedImage() { return calibrated_image_; }

  // --- HOG Result ---
  void setHogResult(vv::HOGResult const& hog_result) {
    hog_result_ = hog_result;  // HOGResult가 복사 가능한 구조체라고 가정
  }
  vv::HOGResult const& getHogResult() const { return hog_result_; }
  vv::HOGResult& getHogResult() { return hog_result_; }

  // --- VV Result ---
  void setVvResult(vv::VVResult const& vv_result) {
    vv_result_ = vv_result;  // VVResult가 복사 가능한 구조체라고 가정
  }
  vv::VVResult const& getVvResult() const { return vv_result_; }
  vv::VVResult& getVvResult() { return vv_result_; }

  // --- VV Params ---
  void setVvParams(vv::VVParams const& vv_params) {
    vv_params_ = vv_params;  // VVParams가 복사 가능한 구조체라고 가정
  }
  vv::VVParams const& getVvParams() const { return vv_params_; }
  vv::VVParams& getVvParams() { return vv_params_; }

  // --- Histogram Image ---
  void setHistogramImage(cv::Mat const& image) {
    histogram_image_ = image.clone();  // 안전을 위해 복사
  }
  cv::Mat const& getHistogramImage() const { return histogram_image_; }
  cv::Mat& getHistogramImage() { return histogram_image_; }

  // --- FPS ---
  void setFps(float fps) { fps_ = fps; }
  float getFps() const { return fps_; }

  // --- Output Image (최종 결과물을 담을 이미지) ---
  // 이 이미지는 create_visualization 함수 시작 시 생성되거나 외부에서 전달받아
  // 컨텍스트에 설정될 수 있습니다. 레이어들은 이 이미지에 직접 그리지 않고,
  // 각자의 결과물을 반환하여 create_visualization 함수가 최종 조합합니다.
  // 또는, 마지막 레이어가 이 output_image_에 직접 그리도록 할 수도 있습니다.
  // 현재 설계에서는 create_visualization 함수가 레이어들의 결과(cv::Mat)를 받아
  // 조합하므로, output_image_는 create_visualization 함수 내의 로컬 변수로
  // 처리하는 것이 더 적합할 수 있습니다. 우선은 멤버로 두되, 사용 방식은 추후
  // create_visualization 수정 시 구체화합니다.
  void setOutputImage(cv::Mat& image) {  // 참조로 설정
    output_image_ = image;
  }
  cv::Mat& getOutputImage() {  // 직접적인 수정이 가능하도록 non-const 참조 반환
    return output_image_;
  }
  cv::Mat const& getOutputImage() const { return output_image_; }

 private:
  cv::Mat input_image_;
  cv::Mat calibrated_image_;
  vv::HOGResult hog_result_;
  vv::VVResult vv_result_;
  vv::VVParams vv_params_;
  cv::Mat histogram_image_;
  float fps_ = 0.0f;

  cv::Mat
      output_image_;  // 최종 이미지를 위한 멤버, 참조가 아닌 실제 객체로 변경
                      // 또는 create_visualization의 로컬 변수로 처리 고려
};

}  // namespace vv::visualization

#endif  // VVP_VISUALIZATION_VISUALIZATION_CONTEXT_H_
