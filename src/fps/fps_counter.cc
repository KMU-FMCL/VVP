#include "vvp/fps/fps_counter.h"
#include "absl/time/clock.h"
#include "absl/time/time.h"
#include <cstdint>   // for int64_t
#include <iostream>  // 디버깅/정보 출력용 (선택 사항)

namespace vv {

static double const kEpsilon = 1e-9;

FPSCounter::FPSCounter()
    : frame_count_(0), total_processing_time_sec_(0.0), current_fps_(0.0F) {
  overall_start_time_ = absl::Now();
  frame_start_time_ = overall_start_time_;  // 초기화
}

void FPSCounter::tick_start() { frame_start_time_ = absl::Now(); }

void FPSCounter::tick_end() {
  absl::Time frame_end_time = absl::Now();
  absl::Duration frame_duration = frame_end_time - frame_start_time_;
  double frame_time_sec = absl::ToDoubleSeconds(frame_duration);

  if (frame_time_sec > kEpsilon) {  // 0으로 나누는 것 방지
    current_fps_ = static_cast<float>(1.0 / frame_time_sec);
  } else {
    current_fps_ = 0.0F;  // 또는 매우 큰 값으로 설정
  }

  total_processing_time_sec_ += frame_time_sec;
  frame_count_++;
}

auto FPSCounter::get_fps() const -> float { return current_fps_; }

auto FPSCounter::get_average_fps() const -> double {
  if (frame_count_ > 0 && total_processing_time_sec_ > kEpsilon) {
    return static_cast<double>(frame_count_) / total_processing_time_sec_;
  }
  return 0.0;
}

auto FPSCounter::get_frame_count() const -> int64_t { return frame_count_; }

auto FPSCounter::get_total_processing_time_sec() const -> double {
  return total_processing_time_sec_;
}

}  // namespace vv