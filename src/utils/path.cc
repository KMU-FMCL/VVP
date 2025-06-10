#include "vvp/utils/path.h"
#include "absl/strings/str_cat.h"
#include "vvp/utils/helpers.h"     // For utils::get_current_date_string
#include "vvp/utils/time.h"  // For TimeUtils

// PROJECT_ROOT가 정의되어 있지 않다면 기본값 설정 (CMake에서 주입되는 것을
// 가정)
#ifndef PROJECT_ROOT
#define PROJECT_ROOT "."
#endif

namespace vv::utils {

auto PathUtils::resolve_input_path(absl::string_view project_root,
                                   absl::string_view input_path_str)
    -> std::string {
  std::filesystem::path in_path(input_path_str);
  if (!in_path.is_absolute()) {
    in_path = std::filesystem::path(project_root) / in_path;
  }
  return in_path.string();
}

auto PathUtils::get_base_output_directory(absl::string_view project_root,
                                          absl::string_view current_date)
    -> std::filesystem::path {
  return std::filesystem::path(project_root) / "results" / current_date;
}

auto PathUtils::generate_output_file_path(std::filesystem::path const& base_dir,
                                          absl::string_view original_filename,
                                          absl::string_view time_part,
                                          absl::string_view prefix,
                                          absl::string_view extension)
    -> std::string {
  std::string filename =
      absl::StrCat(prefix, original_filename, "_", time_part, extension);
  return (base_dir / filename).string();
}

auto PathUtils::generate_csv_output_path(Config const& config,
                                         absl::string_view project_root_str)
    -> std::string {
  std::string current_date = vv::utils::get_current_date_string();
  std::string current_timestamp =
      vv::utils::TimeUtils::get_current_time_stamp();
  std::string time_part =
      vv::utils::TimeUtils::extract_time_part(current_timestamp);

  std::filesystem::path base_output_dir =
      get_base_output_directory(project_root_str, current_date);

  if (config.use_camera) {
    return generate_output_file_path(base_output_dir, "camera", time_part,
                                     "VV_", ".csv");
  }
  std::filesystem::path input_path(config.input_file_path);
  std::string filename_without_ext = input_path.stem().string();
  return generate_output_file_path(base_output_dir, filename_without_ext,
                                   time_part, "VV_", ".csv");
}

auto PathUtils::generate_video_output_path(Config const& config,
                                           absl::string_view project_root_str)
    -> std::string {
  std::string current_date = vv::utils::get_current_date_string();
  std::string current_timestamp =
      vv::utils::TimeUtils::get_current_time_stamp();
  std::string time_part =
      vv::utils::TimeUtils::extract_time_part(current_timestamp);

  std::filesystem::path base_output_dir =
      get_base_output_directory(project_root_str, current_date);

  if (config.use_camera) {
    return generate_output_file_path(base_output_dir, "camera", time_part,
                                     "VV_Video_", ".mp4");
  }
  std::filesystem::path input_path(config.input_file_path);
  std::string filename_without_ext = input_path.stem().string();
  return generate_output_file_path(base_output_dir, filename_without_ext,
                                   time_part, "VV_Video_", ".mp4");
}

}  // namespace vv::utils
