#include "vvp/utils/file_system.h"
#include "absl/strings/str_cat.h"  // For absl::StrCat
#include <exception>               // For std::exception
#include <iostream>                // For std::cout (optional logging)

namespace vv::utils {

auto FileSystemUtils::ensure_directory_exists(std::filesystem::path const& dir)
    -> absl::Status {
  try {
    if (!std::filesystem::exists(dir)) {
      if (std::filesystem::create_directories(dir)) {
        // std::cout << "Directory created: " << dir.string() << std::endl; //
        // 필요시 로그 추가
      } else {
        // std::cout << "Failed to create directory (already exists or other
        // issue): " << dir.string() << std::endl; // 필요시 로그 추가
      }
    }
    return absl::OkStatus();
  } catch (
      std::filesystem::filesystem_error const& e) {  // 구체적인 예외 타입 명시
    return absl::InternalError(absl::StrCat(
        "Could not create directory: ", dir.string(), " (", e.what(), ")"));
  } catch (std::exception const& e) {  // 일반적인 예외 처리
    return absl::InternalError(absl::StrCat(
        "An unexpected error occurred while creating directory: ", dir.string(),
        " (", e.what(), ")"));
  }
}

}  // namespace vv::utils
