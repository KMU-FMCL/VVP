#include "vvp/utils/ConfigLoader.hpp"

#include <yaml-cpp/yaml.h>

namespace vv {
namespace utils {

ConfigAll ConfigLoader::load(const std::string& filepath) {
  ConfigAll cfg;
  try {
    YAML::Node root = YAML::LoadFile(filepath);
    // I/O settings
    if (root["io"]) {
      auto io = root["io"];
      if (io["useCamera"]) {
        cfg.io.useCamera = io["useCamera"].as<bool>();
      }
      if (io["cameraPort"]) {
        cfg.io.cameraPort = io["cameraPort"].as<int>();
      }
      if (io["inputFilePath"]) {
        cfg.io.inputFilePath = io["inputFilePath"].as<std::string>();
      }
      if (io["scale"]) {
        cfg.io.scale = io["scale"].as<int>();
      }
      if (io["saveResults"]) {
        cfg.io.saveResults = io["saveResults"].as<bool>();
      }
    }
    // HOG parameters
    if (root["hog"]) {
      auto hog = root["hog"];
      if (hog["binCount"]) {
        cfg.hog.binCount = hog["binCount"].as<int>();
      }
      if (hog["thresholdValue"]) {
        cfg.hog.thresholdValue = hog["thresholdValue"].as<double>();
      }
      if (hog["blurKernelSize"]) {
        cfg.hog.blurKernelSize = hog["blurKernelSize"].as<int>();
      }
      if (hog["blurSigma"]) {
        cfg.hog.blurSigma = hog["blurSigma"].as<double>();
      }
      if (hog["erodeKernelSize"]) {
        cfg.hog.erodeKernelSize = hog["erodeKernelSize"].as<int>();
      }
    }
    // Visual Vertical estimator parameters
    if (root["vvEstimator"]) {
      auto vv = root["vvEstimator"];
      if (vv["minAngle"]) {
        cfg.vv.minAngle = vv["minAngle"].as<int>();
      }
      if (vv["maxAngle"]) {
        cfg.vv.maxAngle = vv["maxAngle"].as<int>();
      }
      if (vv["smoothingFactor"]) {
        cfg.vv.smoothingFactor = vv["smoothingFactor"].as<double>();
      }
    }
  } catch (const std::exception& e) {
    throw std::runtime_error(std::string("Failed to load config file '") +
                             filepath + "': " + e.what());
  }
  return cfg;
}

}  // namespace utils
}  // namespace vv