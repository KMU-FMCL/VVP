#ifndef VVP_FPS_FPSCOUNTER_H_
#define VVP_FPS_FPSCOUNTER_H_

#include "absl/time/time.h"

namespace vv {

/**
 * @brief FPS 측정을 관리하는 클래스
 */
class FPSCounter {
 public:
  /**
   * @brief 생성자
   */
  FPSCounter();

  /**
   * @brief 프레임 처리 시작 시 호출
   */
  void tick_start();

  /**
   * @brief 프레임 처리 종료 시 호출하여 FPS 계산
   */
  void tick_end();

  /**
   * @brief 가장 최근에 계산된 FPS 반환
   * @return 현재 프레임의 FPS
   */
  double get_fps() const;

  /**
   * @brief 전체 실행 시간 동안의 평균 FPS 계산 및 반환
   * @return 평균 FPS
   */
  double get_average_fps() const;

  /**
   * @brief 처리된 총 프레임 수 반환
   * @return 총 프레임 수
   */
  long long get_frame_count() const;

  /**
   * @brief 총 처리 시간(초) 반환
   * @return 총 처리 시간
   */
  double get_total_processing_time_sec() const;

 private:
  absl::Time frame_start_time_;
  absl::Time overall_start_time_;
  long long frame_count_;
  double total_processing_time_sec_;
  double current_fps_;
};

}  // namespace vv

#endif  // VVP_FPS_FPSCOUNTER_H_