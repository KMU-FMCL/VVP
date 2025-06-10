#include "vvp/utils/time.h"
#include "absl/time/clock.h"
#include "absl/time/time.h"  // For absl::FormatTime and absl::LocalTimeZone

namespace vv::utils {

auto TimeUtils::get_current_time_stamp() -> std::string {
  absl::Time now = absl::Now();
  // Format as "YYYYMMDD_HHMMSS"
  return absl::FormatTime("%Y%m%d_%H%M%S", now, absl::LocalTimeZone());
}

auto TimeUtils::extract_time_part(absl::string_view timestamp) -> std::string {
  size_t pos = timestamp.find('_');
  if (pos != absl::string_view::npos && pos + 1 < timestamp.size()) {
    return std::string(timestamp.substr(pos + 1));
  }
  return "000000";  // 기본값 또는 오류 시 반환 값
}

}  // namespace vv::utils
