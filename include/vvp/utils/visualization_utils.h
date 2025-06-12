#ifndef VVP_UTILS_VISUALIZATION_UTILS_H_
#define VVP_UTILS_VISUALIZATION_UTILS_H_

#include "vvp/estimation/types.h"  // For HOGResult, VVResult
#include "vvp/processing/constants.h"  // For VisualizationConstants, ImageConstants, AngleConstants
#include <opencv2/core/mat.hpp>
#include <string>  // For absl::StrFormat, though it's via HOGResult

namespace vv {
namespace utils {

/**
 * @brief 결과 시각화 이미지 생성
 * @param input_image 원본 입력 이미지
 * @param calibrated_image 보정된 이미지
 * @param hog_result HOG 계산 결과
 * @param vv_result VV 추정 결과
 * @param histogram_image 히스토그램 이미지
 * @param fps 프레임 속도
 * @return 모든 결과가 결합된 시각화 이미지
 */
auto create_visualization(cv::Mat const& input_image,
                          cv::Mat const& calibrated_image,
                          vv::HOGResult const& hog_result,
                          vv::VVResult const& vv_result,
                          cv::Mat const& histogram_image, float fps) -> cv::Mat;

}  // namespace utils
}  // namespace vv

#endif  // VVP_UTILS_VISUALIZATION_UTILS_H_
