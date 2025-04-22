#ifndef VVP_UTILS_HELPERS_H_
#define VVP_UTILS_HELPERS_H_

#include "absl/strings/string_view.h"
#include "absl/time/time.h"

#include <algorithm>
#include <chrono>
#include <numeric>
#include <string>

#include <ctime>

#include "vvp/estimation/Types.hpp"

namespace vv {
namespace utils {

/**
 * @brief 명령줄 인자에서 설정 파싱
 * @param argc 인자 개수
 * @param argv 인자 배열
 * @return 파싱된 설정
 */
Config parse_command_line_args(int argc, char* argv[]);

/**
 * @brief 현재 시간 포매팅
 * @param format 시간 포맷 문자열
 * @return 포매팅된 시간 문자열
 */
std::string format_current_time(absl::string_view format = kIsoTimeFormat);

/**
 * @brief 현재 날짜를 YYYYMMDD 형식의 문자열로 반환
 * @return "YYYYMMDD" 형식의 날짜 문자열
 */
std::string get_current_date_string();

/**
 * @brief 도움말 메시지 출력
 */
void print_usage();

/**
 * @brief OpenCV 정보 및 OpenCL 지원 여부 출력
 */
void print_opencv_info();

/**
 * @brief 벡터의 최대값 인덱스 찾기
 * @param vec 벡터
 * @param startIdx 시작 인덱스
 * @param endIdx 종료 인덱스
 * @return 최대값의 인덱스
 */
template <typename T>
int argmax(const std::vector<T>& vec, int startIdx, int endIdx);

/**
 * @brief 벡터의 상위 N개 인덱스 찾기
 * @param vec 벡터
 * @param n 찾을 상위 개수
 * @param startIdx 시작 인덱스
 * @param endIdx 종료 인덱스
 * @return 상위 N개 인덱스 배열
 */
template <typename T>
std::vector<int> argmax_n(const std::vector<T>& vec, int n, int startIdx,
                          int endIdx);

}  // namespace utils
}  // namespace vv

// 템플릿 함수 구현
template <typename T>
int vv::utils::argmax(const std::vector<T>& vec, int startIdx, int endIdx) {
  if (vec.empty() || startIdx < 0 || endIdx > static_cast<int>(vec.size()) ||
      startIdx >= endIdx) {
    return -1;
  }

  return std::distance(vec.begin(), std::max_element(vec.begin() + startIdx,
                                                     vec.begin() + endIdx));
}

template <typename T>
std::vector<int> vv::utils::argmax_n(const std::vector<T>& vec, int n,
                                     int startIdx, int endIdx) {
  if (vec.empty() || startIdx < 0 || endIdx > static_cast<int>(vec.size()) ||
      startIdx >= endIdx || n <= 0) {
    return {};
  }

  std::vector<int> indices(endIdx - startIdx);
  std::iota(indices.begin(), indices.end(), startIdx);

  std::partial_sort(
      indices.begin(),
      indices.begin() + std::min(n, static_cast<int>(indices.size())),
      indices.end(), [&vec](int i1, int i2) { return vec[i1] > vec[i2]; });

  indices.resize(std::min(n, static_cast<int>(indices.size())));
  return indices;
}

#endif  // VVP_UTILS_HELPERS_H_
