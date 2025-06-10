#include "vvp/io/output_handler.h"
#include "absl/strings/str_format.h"      // For absl::StrFormat
#include "vvp/utils/file_system.h"      // For FileSystemUtils
#include "vvp/utils/path_utils.h"         // For PathUtils
#include <fstream>                        // For std::ofstream
#include <iostream>                       // For std::cout (temporary logging)

namespace vv::io {

OutputHandler::OutputHandler(Config config) : config_(std::move(config)) {
  if (config_.save_csv_results) {
    csv_file_path_ =
        utils::PathUtils::generate_csv_output_path(config_, PROJECT_ROOT);
    if (!csv_file_path_.empty()) {
      std::filesystem::path csv_parent_path =
          std::filesystem::path(csv_file_path_).parent_path();
      absl::Status status =
          utils::FileSystemUtils::ensure_directory_exists(csv_parent_path);
      if (!status.ok()) {
        std::cout << "Error creating CSV directory: " << status.message()
                  << '\n';
      } else {
        std::cout << "CSV results will be saved to directory: "
                  << std::filesystem::absolute(csv_parent_path).string()
                  << '\n';
      }
    }
  }

  if (config_.save_output_video) {
    video_file_path_ =
        utils::PathUtils::generate_video_output_path(config_, PROJECT_ROOT);
    if (!video_file_path_.empty()) {
      std::filesystem::path video_parent_path =
          std::filesystem::path(video_file_path_).parent_path();

      bool already_created = false;
      if (config_.save_csv_results && !csv_file_path_.empty()) {
        if (std::filesystem::path(csv_file_path_).parent_path() ==
            video_parent_path) {
          already_created = true;
        }
      }

      if (!already_created) {
        absl::Status status =
            utils::FileSystemUtils::ensure_directory_exists(video_parent_path);
        if (!status.ok()) {
          std::cout << "Error creating video directory: " << status.message()
                    << '\n';
        } else {
          std::cout << "Video results will be saved to directory: "
                    << std::filesystem::absolute(video_parent_path).string()
                    << '\n';
        }
      } else if (config_.save_csv_results &&
                 csv_file_path_.empty()) {  // CSV 저장 안하지만 비디오는
                                            // 저장하고, 디렉토리 생성 필요
        absl::Status status =
            utils::FileSystemUtils::ensure_directory_exists(video_parent_path);
        if (!status.ok()) {
          std::cout << "Error creating video directory: " << status.message()
                    << '\n';
        } else {
          std::cout << "Video results will be saved to directory: "
                    << std::filesystem::absolute(video_parent_path).string()
                    << '\n';
        }
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
  if (!config_.save_output_video) {
    return absl::OkStatus();
  }
  if (video_file_path_.empty()) {
    return absl::FailedPreconditionError(
        "Video file path is not set. Cannot setup video writer.");
  }

  double output_fps = (fps > 0) ? fps : 30.0;
  int fourcc = cv::VideoWriter::fourcc('M', 'J', 'P', 'G');  // Default MJPG
  if (!config_.output_video_codec.empty() &&
      config_.output_video_codec.length() == 4) {
    fourcc = cv::VideoWriter::fourcc(
        config_.output_video_codec[0], config_.output_video_codec[1],
        config_.output_video_codec[2], config_.output_video_codec[3]);
  }

  video_writer_.open(video_file_path_, fourcc, output_fps,
                     cv::Size(frame_width, frame_height), true);

  if (!video_writer_.isOpened()) {
    return absl::InternalError(absl::StrFormat(
        "Could not open video writer for path: %s with codec %s",
        video_file_path_, config_.output_video_codec));
  }
  return absl::OkStatus();
}

auto OutputHandler::write_frame(cv::Mat const& frame) -> void {
  if (config_.save_output_video && video_writer_.isOpened()) {
    video_writer_.write(frame);
  }
}

auto OutputHandler::save_results_to_csv(std::vector<VVResult> const& results)
    -> absl::Status {
  if (!config_.save_csv_results) {
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

  csv_file << "FrameNumber,Timestamp(ms),EstimatedAngle(degrees),"
              "ProcessingTime(ms)\n";
  for (auto const& result : results) {
    csv_file << result.frame_number << "," << result.timestamp_ms << ","
             << result.estimated_angle_degrees << ","
             << result.processing_time_ms << "\n";
  }

  csv_file.close();
  if (!csv_file) {
    return absl::InternalError(absl::StrFormat(
        "Error writing to or closing CSV file: %s", csv_file_path_));
  }
  return absl::OkStatus();
}

}  // namespace vv::io
