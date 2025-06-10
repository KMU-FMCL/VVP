#ifndef VVP_UTILS_CONFIGLOADER_H_
#define VVP_UTILS_CONFIGLOADER_H_

#include "absl/status/statusor.h"
#include "vvp/estimation/types.h"
#include <string>

namespace vv::utils {

/**
 * @brief Combined configuration structures for application modules.
 */
struct ConfigAll {
  Config io;      ///< I/O related settings
  HOGParams hog;  ///< HOG computation parameters
  VVParams vv;    ///< Visual Vertical estimator parameters
};

/**
 * @brief YAML configuration loader.
 *
 * Uses yaml-cpp to parse a YAML file and populate ConfigAll.
 */
class ConfigLoader {
 public:
  /**
   * @brief Load configuration from a YAML file.
   * @param filepath Path to the YAML config file.
   * @return Populated ConfigAll structure with status.
   */
  static auto load(std::string const& filepath) -> absl::StatusOr<ConfigAll>;
};

}  // namespace vv::utils

#endif  // VVP_UTILS_CONFIGLOADER_H_
