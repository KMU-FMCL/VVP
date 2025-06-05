#include "vvp/utils/ConfigLoader.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_cat.h"
#include <yaml-cpp/yaml.h>

namespace vv::utils {

namespace {

// 설정 항목을 안전하게 파싱하는 템플릿 함수
template <typename T, typename ConfigType>
void parse_config_item(const YAML::Node& node, std::string const& key,
                       ConfigType& config, T ConfigType::* member) {
  if (node[key]) {
    config.*member = node[key].as<T>();
  }
}

// IO 관련 설정 파싱
void parse_io_config(const YAML::Node& io_node, Config& io_config) {
  parse_config_item<bool>(io_node, "use_camera", io_config,
                          &Config::use_camera);
  parse_config_item<int>(io_node, "camera_port", io_config,
                         &Config::camera_port);
  parse_config_item<std::string>(io_node, "input_file_path", io_config,
                                 &Config::input_file_path);
  parse_config_item<int>(io_node, "scale", io_config, &Config::scale);
  parse_config_item<bool>(io_node, "save_results", io_config,
                          &Config::save_results);
}

// HOG 관련 설정 파싱
void parse_hog_config(const YAML::Node& hog_node, HOGParams& hog_config) {
  parse_config_item<int>(hog_node, "bin_count", hog_config,
                         &HOGParams::bin_count);
  parse_config_item<double>(hog_node, "threshold_value", hog_config,
                            &HOGParams::threshold_value);
  parse_config_item<int>(hog_node, "blur_kernel_size", hog_config,
                         &HOGParams::blur_kernel_size);
  parse_config_item<double>(hog_node, "blur_sigma", hog_config,
                            &HOGParams::blur_sigma);
  parse_config_item<int>(hog_node, "erode_kernel_size", hog_config,
                         &HOGParams::erode_kernel_size);
}

// VV 추정기 관련 설정 파싱
void parse_vv_config(const YAML::Node& vv_node, VVParams& vv_config) {
  parse_config_item<int>(vv_node, "min_angle", vv_config, &VVParams::min_angle);
  parse_config_item<int>(vv_node, "max_angle", vv_config, &VVParams::max_angle);
  parse_config_item<double>(vv_node, "smoothing_factor", vv_config,
                            &VVParams::smoothing_factor);
}

}  // namespace

auto ConfigLoader::load(std::string const& filepath)
    -> absl::StatusOr<ConfigAll> {
  ConfigAll cfg;
  try {
    YAML::Node root = YAML::LoadFile(filepath);

    // 각 설정 섹션 파싱
    if (root["io"]) {
      parse_io_config(root["io"], cfg.io);
    }

    if (root["hog"]) {
      parse_hog_config(root["hog"], cfg.hog);
    }

    if (root["vv_estimator"]) {
      parse_vv_config(root["vv_estimator"], cfg.vv);
    }
  } catch (std::exception const& e) {
    return absl::InvalidArgumentError(absl::StrCat(
        "Failed to load config file '", filepath, "': ", e.what()));
  }
  return cfg;
}

}  // namespace vv::utils
