#pragma once

#include <string>

namespace vve {

// HOG 관련 상수
constexpr int NUM_ORIENTATION_BINS = 180;
constexpr int GAUSSIAN_KERNEL_SIZE_X = 11;
constexpr int GAUSSIAN_KERNEL_SIZE_Y = 11;
constexpr double GAUSSIAN_SIGMA = 3.0;
constexpr double MAGNITUDE_THRESHOLD = 0.25;
constexpr int EROSION_KERNEL_SIZE_X = 3;
constexpr int EROSION_KERNEL_SIZE_Y = 3;
constexpr int EROSION_ITERATIONS = 1;

// 방향성 계산 관련 상수
constexpr int ORIENTATION_RANGE_START = 30;
constexpr int ORIENTATION_RANGE_END = 150;  // 슬라이싱 시 exclusive 이므로 150까지 포함하려면 151
constexpr double SMOOTHING_FACTOR = 0.7;    // 현재 값 가중치
constexpr double PREVIOUS_WEIGHT = 1.0 - SMOOTHING_FACTOR;  // 이전 값 가중치
constexpr int TOP_N_PEAKS = 3;              // 방향 계산에 사용할 상위 N개 피크

// 시각화 관련 상수
constexpr int DEFAULT_SCALE_FACTOR = 2;
constexpr int LINE_THICKNESS = 2;
constexpr int ARROW_THICKNESS = 4;
constexpr double FONT_SCALE = 1.5;          // putText 폰트 크기 조절 쉽게
constexpr int FONT_THICKNESS = 2;
constexpr double HIST_YLIM = 0.2;           // 히스토그램 Y축 최대값
constexpr int HIST_BAR_WIDTH = 2;

// 물리 상수
constexpr double GRAVITY_ACCELERATION = 9.8;

// 파일명 포맷
const std::string ISOTIMEFORMAT = "%Y%m%d_%H%M%S";

} // namespace vve