#include "vvp/io/input_handler.h"
#include "absl/strings/str_format.h"  // For absl::StrFormat
#include "vvp/utils/path.h"             // For PathUtils::resolve_input_path

namespace vv::io {

InputHandler::InputHandler(Config config) : config_(std::move(config)) {
  if (!config_.use_camera && !config_.input_file_path.empty()) {
    resolved_input_path_ = utils::PathUtils::resolve_input_path(
        PROJECT_ROOT, config_.input_file_path);
  }
}

InputHandler::~InputHandler() {
  if (video_capture_.isOpened()) {
    video_capture_.release();
  }
}

auto InputHandler::open_video_source() -> absl::Status {
  if (config_.use_camera) {
    video_capture_.open(config_.camera_id);
    if (!video_capture_.isOpened()) {
      return absl::InternalError(absl::StrFormat(
          "Failed to open camera with ID: %d", config_.camera_id));
    }
    video_capture_.set(cv::CAP_PROP_FRAME_WIDTH, config_.camera_width);
    video_capture_.set(cv::CAP_PROP_FRAME_HEIGHT, config_.camera_height);
    video_capture_.set(cv::CAP_PROP_FPS, config_.camera_fps);

  } else {
    if (resolved_input_path_.empty()) {
      return absl::InvalidArgumentError("Input file path is not set.");
    }
    video_capture_.open(resolved_input_path_);
    if (!video_capture_.isOpened()) {
      return absl::InternalError(absl::StrFormat(
          "Failed to open video file: %s", resolved_input_path_));
    }
  }
  return absl::OkStatus();
}

auto InputHandler::read_next_frame(cv::Mat& frame) -> absl::Status {
  if (!video_capture_.isOpened()) {
    return absl::FailedPreconditionError(
        "Video source is not open. Call open_video_source() first.");
  }
  if (video_capture_.read(frame)) {
    if (frame.empty()) {
      // 비디오의 끝에 도달했거나 프레임이 비어있는 경우
      return absl::OutOfRangeError("End of video or empty frame read.");
    }
    return absl::OkStatus();
  }
  // read() 자체가 false를 반환하면 비디오의 끝 또는 읽기 오류
  return absl::OutOfRangeError("Failed to read next frame or end of video.");
}

auto InputHandler::get_video_capture() -> cv::VideoCapture& {
  return video_capture_;
}

auto InputHandler::get_video_capture() const -> cv::VideoCapture const& {
  return video_capture_;
}

auto InputHandler::get_fps() const -> double {
  if (video_capture_.isOpened()) {
    double fps = video_capture_.get(cv::CAP_PROP_FPS);
    return (fps > 0) ? fps
                     : 0.0;  // Return 0.0 if FPS is not available or invalid
  }
  return 0.0;
}

}  // namespace vv::io
