#pragma once

#include <vector>
#include <opencv2/core.hpp>

namespace vve {

/**
 * @brief 방향성 분석 및 계산 클래스
 */
class OrientationAnalyzer {
public:
    /**
     * @brief 생성자
     * @param orientationRangeStart 방향 범위 시작 각도
     * @param orientationRangeEnd 방향 범위 끝 각도
     * @param smoothingFactor 현재 값 가중치
     * @param topNPeaks 사용할 상위 N개 피크 수
     */
    OrientationAnalyzer(
        int orientationRangeStart = 30,
        int orientationRangeEnd = 150,
        double smoothingFactor = 0.7,
        int topNPeaks = 3
    );

    /**
     * @brief 소멸자
     */
    virtual ~OrientationAnalyzer() = default;

    /**
     * @brief 방향성 히스토그램에서 주요 방향 계산
     * @param histogram 방향성 히스토그램
     * @return 주요 방향 각도 (도 단위)
     */
    double calculateDominantOrientation(const std::vector<float>& histogram);

    /**
     * @brief 이전 프레임의 방향을 고려한 스무딩 적용
     * @param currentOrientation 현재 프레임의 방향
     * @return 스무딩된 방향 각도
     */
    double applySmoothing(double currentOrientation);

    /**
     * @brief 히스토그램에서 피크 검출
     * @param histogram 방향성 히스토그램
     * @return 피크 인덱스와 값의 쌍 벡터 (내림차순 정렬)
     */
    std::vector<std::pair<int, float>> findPeaks(const std::vector<float>& histogram);

    /**
     * @brief 히스토그램 슬라이싱 (특정 범위만 선택)
     * @param histogram 전체 방향성 히스토그램
     * @return 슬라이싱된 히스토그램
     */
    std::vector<float> sliceHistogram(const std::vector<float>& histogram);

    /**
     * @brief 방향 각도를 화면 좌표계로 변환
     * @param angle 방향 각도 (도 단위)
     * @return 화면 좌표계 각도 (도 단위)
     */
    double convertToScreenCoordinates(double angle);

    /**
     * @brief 방향 범위 시작 각도 설정
     * @param start 시작 각도
     */
    void setOrientationRangeStart(int start);

    /**
     * @brief 방향 범위 끝 각도 설정
     * @param end 끝 각도
     */
    void setOrientationRangeEnd(int end);

    /**
     * @brief 스무딩 계수 설정
     * @param factor 스무딩 계수 (0.0 ~ 1.0)
     */
    void setSmoothingFactor(double factor);

    /**
     * @brief 상위 피크 수 설정
     * @param n 피크 수
     */
    void setTopNPeaks(int n);

    /**
     * @brief 이전 방향 재설정
     */
    void resetPreviousOrientation();

private:
    int orientationRangeStart;
    int orientationRangeEnd;
    double smoothingFactor;
    double previousWeight;
    int topNPeaks;
    double previousOrientation;
    bool hasPreviousOrientation;

    /**
     * @brief 히스토그램 스무딩 (이동 평균)
     * @param histogram 원본 히스토그램
     * @param windowSize 윈도우 크기
     * @return 스무딩된 히스토그램
     */
    std::vector<float> smoothHistogram(const std::vector<float>& histogram, int windowSize = 5);
};

} // namespace vve