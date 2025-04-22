#include "vvp/estimation/VVEstimator.hpp"

#include <algorithm>
#include <numeric>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

namespace vv {

VVEstimator::VVEstimator() : m_params() {
  // 기본 파라미터 사용
}
//------------------------------------------------------------------------------
VVEstimator::VVEstimator(const VVParams& params) : m_params(params) {
  // 사용자 정의 파라미터 사용
}

VVResult VVEstimator::estimateVV(const std::vector<float>& hogHistogram,
                                 const VVResult& previousResult) {
  VVResult result;

  // 최대 3개의 피크 찾기
  std::vector<int> indices(hogHistogram.size());
  std::iota(indices.begin(), indices.end(), 0);

  // MIN_ANGLE ~ MAX_ANGLE 범위에서만 고려
  auto compareFunc = [&hogHistogram](int i1, int i2) {
    return hogHistogram[i1] > hogHistogram[i2];
  };

  // 범위 내의 상위 3개 인덱스 찾기 (minAngle ~ maxAngle)
  std::vector<int> bestIndices;
  for (size_t i = 0; i < indices.size(); ++i) {
    int angle = indices[i];
    if (angle >= m_params.minAngle && angle <= m_params.maxAngle) {
      bestIndices.push_back(angle);
    }
  }

  std::partial_sort(
      bestIndices.begin(),
      bestIndices.begin() + std::min(3, static_cast<int>(bestIndices.size())),
      bestIndices.end(), compareFunc);

  // 상위 3개 인덱스만 사용
  bestIndices.resize(std::min(3, static_cast<int>(bestIndices.size())));

  if (bestIndices.empty()) {
    // 유효한 인덱스가 없을 경우 이전 결과 반환
    return previousResult;
  }

  // 가중 평균으로 VV 계산
  double sumWeights = 0.0;
  double sumWeightedAngles = 0.0;

  for (int idx : bestIndices) {
    double weight = hogHistogram[idx];
    sumWeights += weight;
    sumWeightedAngles += idx * weight;
  }

  if (sumWeights > 0) {
    result.angle = sumWeightedAngles / sumWeights;
  } else {
    // 가중치 합이 0이면 이전 결과 사용
    result.angle = previousResult.angle;
  }

  // NaN 체크
  if (std::isnan(result.angle)) {
    result.angle = previousResult.angle;
  }

  // 시간적 스무딩 적용
  result.angle = m_params.smoothingFactor * result.angle +
                 (1.0 - m_params.smoothingFactor) * previousResult.angle;

  // 가속도 계산
  result.updateAcceleration();

  // 결과 저장
  m_results.push_back(result);

  return result;
}

const std::vector<VVResult>& VVEstimator::getAllResults() const {
  return m_results;
}

cv::Mat VVEstimator::createHistogramVisualization(
    const std::vector<float>& hogHistogram, const VVResult& vvResult, int width,
    int height) const {
  // 히스토그램이 비어있는 경우 검사
  if (hogHistogram.empty()) {
    return cv::Mat(height, width, CV_8UC3, cv::Scalar(255, 255, 255));
  }

  // 정규화를 위해 합계 계산
  float histSum =
      std::accumulate(hogHistogram.begin(), hogHistogram.end(), 0.0f);
  if (histSum <= 0) {
    return cv::Mat(height, width, CV_8UC3, cv::Scalar(255, 255, 255));
  }

  // 시각화를 위한 이미지 생성
  cv::Mat histImage(height, width, CV_8UC3, cv::Scalar(255, 255, 255));

  // 히스토그램 막대 너비 계산
  int barWidth = std::max(1, width / static_cast<int>(hogHistogram.size()));

  // 히스토그램 최대값 찾기 (정규화용)
  float maxVal =
      *std::max_element(hogHistogram.begin(), hogHistogram.end()) / histSum;
  float scale =
      0.8f * height / std::max(maxVal, 0.001f);  // 높이의 80%까지 사용

  // 히스토그램 그리기
  for (size_t i = 0; i < hogHistogram.size(); i++) {
    float normVal = hogHistogram[i] / histSum;
    int barHeight = cvRound(normVal * scale);

    // X축 반전 (180 -> 0)
    int x = width - static_cast<int>(i) * barWidth - barWidth;

    cv::rectangle(histImage, cv::Point(x, height - barHeight),
                  cv::Point(x + barWidth, height), cv::Scalar(100, 100, 100),
                  cv::FILLED);
  }

  // VV 각도 표시
  int vvX = width - static_cast<int>(vvResult.angle) * barWidth - barWidth / 2;
  cv::line(histImage, cv::Point(vvX, 0), cv::Point(vvX, height),
           cv::Scalar(0, 255, 0), 2, cv::LINE_AA);

  // 경계선 표시 (minAngle, maxAngle)
  int xMin = width - m_params.minAngle * barWidth - barWidth / 2;
  int xMax = width - m_params.maxAngle * barWidth - barWidth / 2;

  cv::line(histImage, cv::Point(xMin, 0), cv::Point(xMin, height),
           cv::Scalar(0, 0, 0), 1, cv::LINE_AA);
  cv::line(histImage, cv::Point(xMax, 0), cv::Point(xMax, height),
           cv::Scalar(0, 0, 0), 1, cv::LINE_AA);

  // X축 눈금 추가
  int tickStep = 30;
  for (int angle = 0; angle <= 180; angle += tickStep) {
    int x = width - angle * barWidth - barWidth / 2;

    // 눈금 선
    cv::line(histImage, cv::Point(x, height - 5), cv::Point(x, height),
             cv::Scalar(0, 0, 0), 1, cv::LINE_AA);

    // 눈금 레이블
    cv::putText(histImage, std::to_string(angle),
                cv::Point(x - 10, height - 10), cv::FONT_HERSHEY_SIMPLEX, 0.4,
                cv::Scalar(0, 0, 0), 1, cv::LINE_AA);
  }

  return histImage;
}

}  // namespace vv