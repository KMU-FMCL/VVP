#ifndef VVP_UTILS_TIME_H_
#define VVP_UTILS_TIME_H_

#include "absl/strings/string_view.h"
#include <string>

namespace vv::utils {
class TimeUtils {
 public:
  TimeUtils() = delete;  // 정적 유틸리티 클래스이므로 인스턴스화 방지

  /**
   * @brief 현재 시간을 ISO 형식 문자열로 반환합니다.
   * @return 현재 시간을 나타내는 문자열 (YYYYMMDD_HHMMSS 형식)
   */
  static auto get_current_time_stamp() -> std::string;

  /**
   * @brief 타임스탬프에서 시간 부분(HHMMSS)만 추출
   * @param timestamp 전체 타임스탬프 문자열 (YYYYMMDD_HHMMSS)
   * @return 시간 부분 문자열 (HHMMSS), 추출 실패 시 "000000"
   */
  [[nodiscard]] static auto extract_time_part(absl::string_view timestamp)
      -> std::string;
};

}  // namespace vv::utils

#endif  // VVP_UTILS_TIME_UTILS_H_
