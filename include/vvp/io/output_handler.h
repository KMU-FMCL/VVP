#ifndef VVP_IO_OUTPUT_HANDLER_H_
#define VVP_IO_OUTPUT_HANDLER_H_

#include "absl/status/status.h"
#include "opencv2/core/mat.hpp"    // For cv::Mat
#include "opencv2/videoio.hpp"     // For cv::VideoWriter
#include "vvp/estimation/types.h"  // For Config, VVResult
#include <filesystem>              // For std::filesystem::path
#include <string>
#include <vector>

// PROJECT_ROOT 매크로가 정의되지 않은 경우를 대비한 기본값 설정
#ifndef PROJECT_ROOT
#define PROJECT_ROOT "."
#endif

namespace vv::io {

class OutputHandler {
 public:
  explicit OutputHandler(Config config);
  ~OutputHandler();

  // Rule of Five
  OutputHandler(OutputHandler const&) = delete;
  auto operator=(OutputHandler const&) -> OutputHandler& = delete;
  OutputHandler(OutputHandler&&) = delete;
  auto operator=(OutputHandler&&) -> OutputHandler& = delete;

  /**
   * @brief 결과 비디오 파일을 쓰기 위해 VideoWriter를 설정합니다.
   * @param frame_width 비디오 프레임의 너비.
   * @param frame_height 비디오 프레임의 높이.
   * @param fps 비디오의 초당 프레임 수.
   * @return 작업 성공 여부를 나타내는 absl::Status.
   */
  [[nodiscard]] auto setup_video_writer(int frame_width, int frame_height,
                                        double fps) -> absl::Status;

  /**
   * @brief 주어진 프레임을 비디오 파일에 씁니다.
   * @param frame 쓰여질 cv::Mat 객체.
   */
  auto write_frame(cv::Mat const& frame) -> void;

  /**
   * @brief 처리 결과를 CSV 파일에 저장합니다.
   * @param results 저장할 VVResult 객체의 벡터.
   * @return 작업 성공 여부를 나타내는 absl::Status.
   */
  [[nodiscard]] auto save_results_to_csv(std::vector<VVResult> const& results)
      -> absl::Status;

 private:
  Config config_;
  cv::VideoWriter video_writer_;
  std::string csv_file_path_;
  std::string video_file_path_;
};

}  // namespace vv::io

#endif  // VVP_IO_OUTPUT_HANDLER_H_
