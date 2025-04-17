#include "VisualVerticalEstimator/orientation.h"
#include "VisualVerticalEstimator/utils.h"
#include <algorithm>
#include <numeric>
#include <cmath>

namespace vve {

OrientationAnalyzer::OrientationAnalyzer(
    int orientationRangeStart,
    int orientationRangeEnd,
    double smoothingFactor,
    int topNPeaks
) : orientationRangeStart(orientationRangeStart),
    orientationRangeEnd(orientationRangeEnd),
    smoothingFactor(smoothingFactor),
    previousWeight(1.0 - smoothingFactor),
    topNPeaks(topNPeaks),
    previousOrientation(0.0),
    hasPreviousOrientation(false) {
}

double OrientationAnalyzer::calculateDominantOrientation(const std::vector<float>& histogram) {
    // 히스토그램 슬라이싱 (관심 범위만 선택)
    std::vector<float> sliced = sliceHistogram(histogram);
    
    // 히스토그램 스무딩
    std::vector<float> smoothed = smoothHistogram(sliced);
    
    // 피크 검출
    std::vector<std::pair<int, float>> peaks = findPeaks(smoothed);
    
    // 상위 N개 피크 선택
    int numPeaks = std::min(static_cast<int>(peaks.size()), topNPeaks);
    
    if (numPeaks == 0) {
        Logger::warning("피크를 찾을 수 없습니다.");
        return hasPreviousOrientation ? previousOrientation : 90.0;
    }
    
    // 가중 평균 계산
    double weightedSum = 0.0;
    double weightSum = 0.0;
    
    for (int i = 0; i < numPeaks; ++i) {
        int peakIndex = peaks[i].first;
        float peakValue = peaks[i].second;
        
        // 원래 히스토그램 인덱스로 변환
        int originalIndex = peakIndex + orientationRangeStart;
        
        // 각도 계산 (빈 인덱스를 각도로 변환)
        double angle = static_cast<double>(originalIndex) * 180.0 / histogram.size();
        
        // 가중치 적용 (피크 값을 가중치로 사용)
        weightedSum += angle * peakValue;
        weightSum += peakValue;
    }
    
    double dominantOrientation = weightSum > 0.0 ? weightedSum / weightSum : 90.0;
    
    // 시간적 스무딩 적용
    return applySmoothing(dominantOrientation);
}

double OrientationAnalyzer::applySmoothing(double currentOrientation) {
    if (!hasPreviousOrientation) {
        previousOrientation = currentOrientation;
        hasPreviousOrientation = true;
        return currentOrientation;
    }
    
    // 각도 차이가 큰 경우 스무딩 적용하지 않음 (급격한 변화 방지)
    double diff = std::abs(currentOrientation - previousOrientation);
    if (diff > 45.0) {
        previousOrientation = currentOrientation;
        return currentOrientation;
    }
    
    // 지수 이동 평균 적용
    double smoothedOrientation = smoothingFactor * currentOrientation + 
                                 previousWeight * previousOrientation;
    
    previousOrientation = smoothedOrientation;
    return smoothedOrientation;
}

std::vector<std::pair<int, float>> OrientationAnalyzer::findPeaks(const std::vector<float>& histogram) {
    std::vector<std::pair<int, float>> peaks;
    
    if (histogram.empty()) {
        return peaks;
    }
    
    // 첫 번째와 마지막 요소는 별도로 처리
    if (histogram.size() > 1) {
        if (histogram[0] > histogram[1]) {
            peaks.emplace_back(0, histogram[0]);
        }
        
        if (histogram.back() > histogram[histogram.size() - 2]) {
            peaks.emplace_back(histogram.size() - 1, histogram.back());
        }
    }
    
    // 중간 요소들 처리
    for (size_t i = 1; i < histogram.size() - 1; ++i) {
        if (histogram[i] > histogram[i - 1] && histogram[i] > histogram[i + 1]) {
            peaks.emplace_back(i, histogram[i]);
        }
    }
    
    // 피크 값 기준으로 내림차순 정렬
    std::sort(peaks.begin(), peaks.end(), 
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    return peaks;
}

std::vector<float> OrientationAnalyzer::sliceHistogram(const std::vector<float>& histogram) {
    if (histogram.empty()) {
        return {};
    }
    
    int start = std::max(0, orientationRangeStart);
    int end = std::min(static_cast<int>(histogram.size()), orientationRangeEnd);
    
    if (start >= end || start >= static_cast<int>(histogram.size())) {
        return {};
    }
    
    return std::vector<float>(histogram.begin() + start, histogram.begin() + end);
}

std::vector<float> OrientationAnalyzer::smoothHistogram(const std::vector<float>& histogram, int windowSize) {
    if (histogram.empty() || windowSize <= 1) {
        return histogram;
    }
    
    int halfWindow = windowSize / 2;
    std::vector<float> smoothed(histogram.size());
    
    for (size_t i = 0; i < histogram.size(); ++i) {
        float sum = 0.0f;
        int count = 0;
        
        for (int j = -halfWindow; j <= halfWindow; ++j) {
            int index = static_cast<int>(i) + j;
            
            // 경계 처리 (순환)
            if (index < 0) {
                index += histogram.size();
            } else if (index >= static_cast<int>(histogram.size())) {
                index -= histogram.size();
            }
            
            sum += histogram[index];
            ++count;
        }
        
        smoothed[i] = sum / count;
    }
    
    return smoothed;
}

double OrientationAnalyzer::convertToScreenCoordinates(double angle) {
    // 화면 좌표계에서는 위쪽이 0도, 시계 방향으로 각도가 증가
    // HOG 각도는 오른쪽이 0도, 반시계 방향으로 각도가 증가
    return (450.0 - angle) * 180.0 / 180.0;
}

} // namespace vve