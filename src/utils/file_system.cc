#include "vvp/utils/file_system.h"
#include "absl/strings/str_cat.h"  // For absl::StrCat
#include <exception>               // For std::exception
#include <iostream>                // For std::cout (optional logging)

namespace vv::utils {

auto FileSystemUtils::ensure_directory_exists(std::filesystem::path const& dir)
    -> absl::Status {
  try {
    // create_directories does nothing if the directory already exists.
    // It will throw an exception on other errors.
    std::filesystem::create_directories(dir);
    return absl::OkStatus();
  } catch (std::filesystem::filesystem_error const&
               e) {  // More specific exception type
    return absl::InternalError(absl::StrCat(
        "Could not create directory: ", dir.string(), " (", e.what(), ")"));
  } catch (std::exception const& e) {  // Generic exception for other cases
    return absl::InternalError(absl::StrCat(
        "An unexpected error occurred while creating directory: ", dir.string(),
        " (", e.what(), ")"));
  }
}

}  // namespace vv::utils
