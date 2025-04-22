#pragma once
#include <string>

#include "vvp/estimation/Types.hpp"

namespace vv {
namespace utils {

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
   * @return Populated ConfigAll structure.
   */
  static ConfigAll load(const std::string& filepath);
};

}  // namespace utils
}  // namespace vv