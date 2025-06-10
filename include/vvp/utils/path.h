#ifndef VVP_UTILS_PATH_H_
#define VVP_UTILS_PATH_H_

#include "absl/strings/string_view.h"
#include "vvp/estimation/types.h"  // For Config
#include <filesystem>
#include <string>

namespace vv::utils {
class PathUtils {
 public:
  PathUtils() = delete;  // 정적 유틸리티 클래스

  /**
   * @brief 입력 파일 경로를 절대 경로로 해석합니다. (상대 경로일 경우
   * PROJECT_ROOT 기준)
   * @param input_path 원본 입력 파일 경로
   * @return 해석된 절대 경로 문자열
   */
  [[nodiscard]] static auto resolve_input_path(absl::string_view project_root,
                                               absl::string_view input_path)
      -> std::string;

  /**
   * @brief 출력 파일의 기본 디렉토리 경로를 생성합니다.
   * (PROJECT_ROOT/results/YYYYMMDD)
   * @param project_root 프로젝트 루트 디렉토리
   * @param current_date 현재 날짜 문자열 (YYYYMMDD)
   * @return 생성된 기본 출력 디렉토리 경로
   */
  [[nodiscard]] static auto get_base_output_directory(
      absl::string_view project_root, absl::string_view current_date)
      -> std::filesystem::path;

  /**
   * @brief 최종 출력 파일 경로를 생성합니다.
   * @param base_dir 기본 출력 디렉토리
   * @param original_filename 원본 파일 이름 (확장자 제외, 카메라 입력 시
   * "camera")
   * @param time_part 시간 부분 문자열 (HHMMSS)
   * @param prefix 파일명 접두사 (예: "VV_", "VV_Video_")
   * @param extension 파일 확장자 (예: ".csv", ".mp4")
   * @return 완전한 출력 파일 경로 문자열
   */
  [[nodiscard]] static auto generate_output_file_path(
      std::filesystem::path const& base_dir,
      absl::string_view original_filename, absl::string_view time_part,
      absl::string_view prefix, absl::string_view extension) -> std::string;

  // Config를 직접 사용하는 헬퍼 함수들
  /**
   * @brief Config를 기반으로 출력 CSV 파일 경로를 생성합니다.
   * @param config 프로그램 설정
   * @param project_root 프로젝트 루트 경로
   * @return CSV 파일 경로
   */
  [[nodiscard]] static auto generate_csv_output_path(
      Config const& config, absl::string_view project_root) -> std::string;

  /**
   * @brief Config를 기반으로 출력 비디오 파일 경로를 생성합니다.
   * @param config 프로그램 설정
   * @param project_root 프로젝트 루트 경로
   * @return 비디오 파일 경로
   */
  [[nodiscard]] static auto generate_video_output_path(
      Config const& config, absl::string_view project_root) -> std::string;
};

}  // namespace vv::utils

#endif  // VVP_UTILS_PATH_UTILS_H_
