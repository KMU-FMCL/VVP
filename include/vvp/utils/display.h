#ifndef VVP_UTILS_DISPLAY_H_
#define VVP_UTILS_DISPLAY_H_

#include "opencv2/core/mat.hpp"  // For cv::Mat
#include <string>

namespace vv::utils {
class DisplayUtils {
 public:
  DisplayUtils() = delete;  // 정적 유틸리티 클래스

  /**
   * @brief 프레임을 화면에 표시하고 키 입력을 기다립니다.
   * @param window_name 창 이름
   * @param frame 표시할 프레임
   * @param wait_key_ms cv::waitKey에 전달될 대기 시간 (밀리초)
   * @return 눌린 키의 ASCII 코드, 시간 초과 시 -1
   */
  [[nodiscard]] static auto display_frame(std::string const& window_name,
                                          cv::Mat const& frame, int wait_key_ms)
      -> int;
};

}  // namespace vv::utils

#endif  // VVP_UTILS_DISPLAY_UTILS_H_
