// include/vvp/visualization/visualization.h
#ifndef VVP_VISUALIZATION_VISUALIZATION_H_
#define VVP_VISUALIZATION_VISUALIZATION_H_

#include "vvp/estimation/types.h"  // For HOGResult, VVResult (used by context indirectly)
#include "vvp/processing/constants.h"  // For various constants (used by context indirectly)
#include <opencv2/core/mat.hpp>
#include <string>

// Forward declaration
namespace vv::visualization {
class VisualizationContext;  // Forward declare VisualizationContext
}

namespace vv::visualization {

/**
 * @brief 결과 시각화 이미지 생성 (컨텍스트 기반)
 * @param context 시각화에 필요한 모든 데이터를 포함하는 컨텍스트 객체
 * @return 모든 결과가 결합된 시각화 이미지
 */
auto create_visualization(VisualizationContext& context) -> cv::Mat;

}  // namespace vv::visualization

#endif  // VVP_VISUALIZATION_VISUALIZATION_H_
