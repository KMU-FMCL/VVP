#include "vvp/estimation/types.h"
#include <cmath>

namespace vv {

// 상수 정의
absl::string_view const TimeConstants::kIsoFormat = "%Y%m%d_%H%M%S";

// Implementation is only used as needed
// Currently mostly empty as most functionality is implemented inline in the
// header

void VVResult::update_acceleration() {
  angle_rad = angle * PhysicsConstants::kDegreesToRadians;
  acc_x = PhysicsConstants::kGravityAcceleration * std::cos(angle_rad);
  acc_y = PhysicsConstants::kGravityAcceleration * std::sin(angle_rad);
}

}  // namespace vv