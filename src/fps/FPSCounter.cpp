#include "vvp/fps/FPSCounter.h"

#include <iostream>  // 디버깅/정보 출력용 (선택 사항)

#include "absl/time/clock.h"
#include "absl/time/time.h"

namespace vv {

FPSCounter::FPSCounter()
    : frame_count_(0), total_processing_time_sec_(0.0), current_fps_(0.0) {
  overall_start_time_ = absl::Now();
  frame_start_time_ = overall_start_time_;  // 초기화
}

void FPSCounter::tick_start() {
  frame_start_time_ = absl::Now();
}

void FPSCounter::tick_end() {
  absl::Time frame_end_time = absl::Now();
  absl::Duration frame_duration = frame_end_time - frame_start_time_;
  double frame_time_sec = absl::ToDoubleSeconds(frame_duration);

  if (frame_time_sec > 1e-9) {  // 0으로 나누는 것 방지
    current_fps_ = 1.0 / frame_time_sec;
  } else {
    current_fps_ = 0.0;  // 또는 매우 큰 값으로 설정
  }

  total_processing_time_sec_ += frame_time_sec;
  frame_count_++;
}

double FPSCounter::get_fps() const {
  return current_fps_;
}

double FPSCounter::get_average_fps() const {
  if (frame_count_ > 0 && total_processing_time_sec_ > 1e-9) {
    return frame_count_ / total_processing_time_sec_;
  }
  return 0.0;
}

long long FPSCounter::get_frame_count() const {
  return frame_count_;
}

double FPSCounter::get_total_processing_time_sec() const {
  return total_processing_time_sec_;
}

}  // namespace vv