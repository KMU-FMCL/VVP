#include "vvp/io/output_handler.h"

#include <fstream>
#include <iostream>

#include "absl/strings/str_format.h"
#include "vvp/utils/file_system.h"
#include "vvp/utils/path.h"

namespace vv::io {

OutputHandler::OutputHandler(Config config) : config_(std::move(config)) {
  if (config_.save_results) {
    // Generate paths for both CSV and video
    csv_file_path_ =
        utils::PathUtils::generate_csv_output_path(config_, PROJECT_ROOT);
    video_file_path_ =
        utils::PathUtils::generate_video_output_path(config_, PROJECT_ROOT);

    // Ensure the output directory exists.
    // Since both files go to the same directory, we only need to check one.
    if (!video_file_path_.empty()) {
      std::filesystem::path parent_path =
          std::filesystem::path(video_file_path_).parent_path();
      absl::Status status =
          utils::FileSystemUtils::ensure_directory_exists(parent_path);

      if (!status.ok()) {
        std::cerr << "Error creating output directory: " << status.message()
                  << '\n';
      } else {
        std::cout << "Output will be saved to directory: "
                  << std::filesystem::absolute(parent_path).string() << '\n';
      }
    }
  }
}

OutputHandler::~OutputHandler() {
  if (video_writer_.isOpened()) {
    video_writer_.release();
  }
}

auto OutputHandler::setup_video_writer(int frame_width, int frame_height,
                                       double fps) -> absl::Status {
  if (!config_.save_results) {
    return absl::OkStatus();
  }
  if (video_file_path_.empty()) {
    return absl::FailedPreconditionError(
        "Video file path is not set. Cannot setup video writer.");
  }

  double output_fps = (fps > 0) ? fps : 30.0;
  // Use a standard MP4V codec.
  int fourcc = cv::VideoWriter::fourcc('m', 'p', '4', 'v');

  video_writer_.open(video_file_path_, fourcc, output_fps,
                     cv::Size(frame_width, frame_height), true);

  if (!video_writer_.isOpened()) {
    return absl::InternalError(absl::StrFormat(
        "Could not open video writer for path: %s", video_file_path_));
  }
  return absl::OkStatus();
}

auto OutputHandler::write_frame(cv::Mat const& frame) -> void {
  if (config_.save_results && video_writer_.isOpened()) {
    video_writer_.write(frame);
  }
}

auto OutputHandler::save_results_to_csv(
    std::vector<VVResult> const& results) -> absl::Status {
  if (!config_.save_results) {
    return absl::OkStatus();
  }
  if (csv_file_path_.empty()) {
    return absl::FailedPreconditionError(
        "CSV file path is not set. Cannot save results.");
  }
  if (results.empty()) {
    return absl::OkStatus();
  }

  std::ofstream csv_file(csv_file_path_);
  if (!csv_file.is_open()) {
    return absl::InternalError(absl::StrFormat(
        "Failed to open CSV file for writing: %s", csv_file_path_));
  }

  // Write header matching the VVResult struct
  csv_file << "FrameNumber,Angle(degrees),Angle(radians),AccelerationX,"
              "AccelerationY,FPS\n";

  int frame_number = 0;
  for (auto const& result : results) {
    csv_file << frame_number++ << "," << result.angle << "," << result.angle_rad
             << "," << result.acc_x << "," << result.acc_y << "," << result.fps
             << "\n";
  }

  csv_file.close();
  if (!csv_file) {
    return absl::InternalError(
        absl::StrFormat("Error writing to or closing CSV file: %s",
                        csv_file_path_));
  }
  return absl::OkStatus();
}

}  // namespace vv::io
