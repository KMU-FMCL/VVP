#ifndef VVP_IO_IOHANDLER_H_
#define VVP_IO_IOHANDLER_H_

#include "absl/status/status.h"
#include "absl/strings/string_view.h"
#include "vvp/estimation/types.h"
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <filesystem>
#include <string>
#include <vector>

namespace vv {

/**
 * @brief 입출력 처리 클래스
 *
 * 비디오 파일/카메라 읽기, 결과 저장, 화면 출력 등을 담당하는 클래스입니다.
 */
class IOHandler {
 public:
  // 상수 정의
  static constexpr double kDefaultFps =
      30.0;  // 기본 FPS 값 (비디오 소스에서 FPS를 가져올 수 없을 때 사용)

  /**
   * @brief 생성자
   * @param config 프로그램 설정
   */
  explicit IOHandler(Config config);

  /**
   * @brief 소멸자
   */
  ~IOHandler();

  // Rule of Five 준수: 복사 및 이동 생성자/대입 연산자 비활성화
  IOHandler(IOHandler const&) = delete;                     // 복사 생성자
  auto operator=(IOHandler const&) -> IOHandler& = delete;  // 복사 대입 연산자
  IOHandler(IOHandler&&) = delete;                          // 이동 생성자
  auto operator=(IOHandler&&) -> IOHandler& = delete;       // 이동 대입 연산자

  /**
   * @brief 비디오 입력 준비
   * @return 성공 상태
   */
  [[nodiscard]] auto open_video_source() -> absl::Status;

  /**
   * @brief 다음 프레임 읽기
   * @param[out] frame 읽은 프레임이 저장될 Mat
   * @return 프레임을 성공적으로 읽었는지 상태
   */
  [[nodiscard]] auto read_next_frame(cv::Mat& frame) -> absl::Status;

  /**
   * @brief 결과 비디오 파일 준비
   * @param width 비디오 너비
   * @param height 비디오 높이
   * @return 성공 상태
   */
  [[nodiscard]] auto setup_video_writer(int width, int height) -> absl::Status;

  /**
   * @brief 프레임을 결과 비디오에 쓰기
   * @param frame
   */
  auto write_frame(cv::Mat const& frame) -> void;

  /**
   * @brief 처리 결과 표시
   * @param frame 표시할 프레임
   * @param wait_key 키 입력 대기 시간 (ms)
   * @return 입력된 키 (ESC: 27)
   */
  [[nodiscard]] static auto display_frame(cv::Mat const& frame,
                                          int wait_key = 1) -> int;

  /**
   * @brief 처리 결과 CSV 파일로 저장
   * @param results VV 결과 벡터
   * @return 성공 상태
   */
  [[nodiscard]] auto save_results_to_csv(std::vector<VVResult> const& results)
      -> absl::Status;

  /**
   * @brief VideoCapture 객체 얻기
   * @return VideoCapture 참조
   */
  [[nodiscard]] auto get_video_capture() -> cv::VideoCapture&;

  /**
   * @brief 결과 파일 경로 생성
   * @param prefix 파일 접두사
   * @param extension 파일 확장자
   * @return 생성된 파일 경로
   */
  [[nodiscard]] static auto generate_output_file_path(
      absl::string_view prefix, absl::string_view extension) -> std::string;

 private:
  Config config_;
  cv::VideoCapture video_capture_;
  cv::VideoWriter video_writer_;
  std::string csv_file_path_;
  std::string video_file_path_;

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

  /**
   * @brief 디렉토리가 존재하지 않으면 생성
   * @param dir 생성할 디렉토리 경로
   * @return 성공 상태
   */
  [[nodiscard]] static auto ensure_directory_exists(
      std::filesystem::path const& dir) -> absl::Status;
};

}  // namespace vv

#endif  // VVP_IO_IOHANDLER_H_
