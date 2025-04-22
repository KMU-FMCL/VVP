#include "vvp/utils/ConfigLoader.h"

#include <yaml-cpp/yaml.h>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_cat.h"

namespace vv {
namespace utils {

absl::StatusOr<ConfigAll> ConfigLoader::load(const std::string& filepath) {
  ConfigAll cfg;
  try {
    YAML::Node root = YAML::LoadFile(filepath);
    // I/O settings
    if (root["io"]) {
      auto io = root["io"];
      if (io["use_camera"]) {
        cfg.io.use_camera = io["use_camera"].as<bool>();
      }
      if (io["camera_port"]) {
        cfg.io.camera_port = io["camera_port"].as<int>();
      }
      if (io["input_file_path"]) {
        cfg.io.input_file_path = io["input_file_path"].as<std::string>();
      }
      if (io["scale"]) {
        cfg.io.scale = io["scale"].as<int>();
      }
      if (io["save_results"]) {
        cfg.io.save_results = io["save_results"].as<bool>();
      }
    }
    // HOG parameters
    if (root["hog"]) {
      auto hog = root["hog"];
      if (hog["bin_count"]) {
        cfg.hog.bin_count = hog["bin_count"].as<int>();
      }
      if (hog["threshold_value"]) {
        cfg.hog.threshold_value = hog["threshold_value"].as<double>();
      }
      if (hog["blur_kernel_size"]) {
        cfg.hog.blur_kernel_size = hog["blur_kernel_size"].as<int>();
      }
      if (hog["blur_sigma"]) {
        cfg.hog.blur_sigma = hog["blur_sigma"].as<double>();
      }
      if (hog["erode_kernel_size"]) {
        cfg.hog.erode_kernel_size = hog["erode_kernel_size"].as<int>();
      }
    }
    // Visual Vertical estimator parameters
    if (root["vv_estimator"]) {
      auto vv = root["vv_estimator"];
      if (vv["min_angle"]) {
        cfg.vv.min_angle = vv["min_angle"].as<int>();
      }
      if (vv["max_angle"]) {
        cfg.vv.max_angle = vv["max_angle"].as<int>();
      }
      if (vv["smoothing_factor"]) {
        cfg.vv.smoothing_factor = vv["smoothing_factor"].as<double>();
      }
    }
  } catch (const std::exception& e) {
    return absl::InvalidArgumentError(absl::StrCat(
        "Failed to load config file '", filepath, "': ", e.what()));
  }
  return cfg;
}

}  // namespace utils
}  // namespace vv
