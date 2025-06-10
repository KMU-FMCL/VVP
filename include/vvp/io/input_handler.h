#ifndef VVP_IO_INPUT_HANDLER_H_
#define VVP_IO_INPUT_HANDLER_H_

#include "absl/status/status.h"
#include "opencv2/core/mat.hpp"    // For cv::Mat
#include "opencv2/videoio.hpp"     // For cv::VideoCapture
#include "vvp/estimation/types.h"  // For Config
#include <string>

// PROJECT_ROOT 매크로가 정의되지 않은 경우를 대비한 기본값 설정
#ifndef PROJECT_ROOT
#define PROJECT_ROOT "."
#endif

namespace vv::io {

class InputHandler {
 public:
  explicit InputHandler(Config const& config);
  ~InputHandler();

  // Rule of Five
  InputHandler(InputHandler const&) = delete;
  auto operator=(InputHandler const&) -> InputHandler& = delete;
  InputHandler(InputHandler&&) = delete;
  auto operator=(InputHandler&&) -> InputHandler& = delete;

  /**
   * @brief 비디오 입력 소스를 엽니다 (파일 또는 카메라).
   * @return 작업 성공 여부를 나타내는 absl::Status.
   */
  [[nodiscard]] auto open_video_source() -> absl::Status;

  /**
   * @brief 비디오 소스에서 다음 프레임을 읽습니다.
   * @param[out] frame 읽은 프레임이 저장될 cv::Mat 객체.
   * @return 작업 성공 여부를 나타내는 absl::Status. 프레임 읽기 실패 또는
   * 비디오 종료 시 에러 상태 반환.
   */
  [[nodiscard]] auto read_next_frame(cv::Mat& frame) -> absl::Status;

  /**
   * @brief 내부 VideoCapture 객체에 대한 참조를 반환합니다.
   * @return cv::VideoCapture 객체에 대한 참조.
   */
  [[nodiscard]] auto get_video_capture() -> cv::VideoCapture&;

  /**
   * @brief 내부 VideoCapture 객체의 프레임 속도를 반환합니다.
   * @return 프레임 속도.
   */
  [[nodiscard]] auto get_fps() const -> double;

  /**
   * @brief VideoCapture 객체에 대한 상수 참조를 반환합니다.
   * @return cv::VideoCapture 객체에 대한 상수 참조.
   */
  [[nodiscard]] auto get_video_capture() const -> cv::VideoCapture const&;

 private:
  Config config_;  // 입력 관련 설정만 사용 예정
  cv::VideoCapture video_capture_;
  std::string resolved_input_path_;  // 실제 사용될 입력 경로
};

}  // namespace vv::io

#endif  // VVP_IO_INPUT_HANDLER_H_
