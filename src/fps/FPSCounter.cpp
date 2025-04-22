#include "vvp/fps/FPSCounter.hpp"

#include <iostream>  // 디버깅/정보 출력용 (선택 사항)

namespace vv {

FPSCounter::FPSCounter()
    : frame_count_(0), total_processing_time_sec_(0.0), current_fps_(0.0) {
  overall_start_time_ = std::chrono::high_resolution_clock::now();
  frame_start_time_ = overall_start_time_;  // 초기화
}

void FPSCounter::tickStart() {
  frame_start_time_ = std::chrono::high_resolution_clock::now();
}

void FPSCounter::tickEnd() {
  auto frameEndTime = std::chrono::high_resolution_clock::now();
  auto frameDuration = std::chrono::duration_cast<std::chrono::microseconds>(
      frameEndTime - frame_start_time_);
  double frameTimeSec = frameDuration.count() / 1'000'000.0;

  if (frameTimeSec > 1e-9) {  // 0으로 나누는 것 방지
    current_fps_ = 1.0 / frameTimeSec;
  } else {
    current_fps_ = 0.0;  // 또는 매우 큰 값으로 설정
  }

  total_processing_time_sec_ += frameTimeSec;
  frame_count_++;
}

double FPSCounter::getFPS() const {
  return current_fps_;
}

double FPSCounter::getAverageFPS() const {
  if (frame_count_ > 0 && total_processing_time_sec_ > 1e-9) {
    return frame_count_ / total_processing_time_sec_;
  }
  return 0.0;
}

long long FPSCounter::getFrameCount() const {
  return frame_count_;
}

double FPSCounter::getTotalProcessingTimeSec() const {
  return total_processing_time_sec_;
}

}  // namespace vv