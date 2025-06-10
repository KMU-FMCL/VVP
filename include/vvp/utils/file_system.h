#ifndef VVP_UTILS_FILE_SYSTEM_H_
#define VVP_UTILS_FILE_SYSTEM_H_

#include "absl/status/status.h"
#include <filesystem>  // For std::filesystem::path

namespace vv::utils {

class FileSystemUtils {
 public:
  FileSystemUtils() = delete;  // 정적 유틸리티 클래스이므로 인스턴스화 방지

  /**
   * @brief 디렉토리가 존재하지 않으면 생성
   * @param dir 생성할 디렉토리 경로
   * @return 성공 상태
   */
  [[nodiscard]] static auto ensure_directory_exists(
      std::filesystem::path const& dir) -> absl::Status;
};

}  // namespace vv::utils

#endif  // VVP_UTILS_FILE_SYSTEM_UTILS_H_
