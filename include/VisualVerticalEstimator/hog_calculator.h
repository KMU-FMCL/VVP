#pragma once

#include <opencv2/core.hpp>
#include <vector>

namespace vve {

/**
 * @brief HOG(Histogram of Oriented Gradients) 특징 계산 클래스
 */
class HOGCalculator {
public:
    /**
     * @brief HOG 특징 구조체
     */
    struct HOGFeatures {
        cv::Mat gradientY;           // Y 방향 그래디언트
        std::vector<float> histogram; // 방향성 히스토그램
        cv::Mat magnitude;           // 그래디언트 크기
        cv::Mat filteredMagnitude;   // 임계값 필터링된 크기
    };

    /**
     * @brief 생성자
     * @param numBins 방향성 히스토그램의 빈 개수
     * @param gaussianKernelSizeX 가우시안 커널 X 크기
     * @param gaussianKernelSizeY 가우시안 커널 Y 크기
     * @param gaussianSigma 가우시안 시그마 값
     * @param magnitudeThreshold 크기 임계값
     */
    HOGCalculator(
        int numBins = 180,
        int gaussianKernelSizeX = 11,
        int gaussianKernelSizeY = 11,
        double gaussianSigma = 3.0,
        double magnitudeThreshold = 0.25
    );

    /**
     * @brief 소멸자
     */
    virtual ~HOGCalculator() = default;

    /**
     * @brief 입력 이미지에 대해 HOG 특징 계산
     * @param image 입력 BGR 이미지
     * @return 계산된 HOG 특징 (gradient_y, histogram, magnitude, filtered_magnitude)
     */
    HOGFeatures calculate(const cv::Mat& image);

    /**
     * @brief 히스토그램 빈 개수 설정
     * @param bins 빈 개수
     */
    void setNumBins(int bins);

    /**
     * @brief 가우시안 커널 크기 설정
     * @param sizeX X 방향 크기
     * @param sizeY Y 방향 크기
     */
    void setGaussianKernelSize(int sizeX, int sizeY);

    /**
     * @brief 가우시안 시그마 값 설정
     * @param sigma 시그마 값
     */
    void setGaussianSigma(double sigma);

    /**
     * @brief 크기 임계값 설정
     * @param threshold 임계값
     */
    void setMagnitudeThreshold(double threshold);

private:
    int numBins;
    cv::Size gaussianKernelSize;
    double gaussianSigma;
    double magnitudeThreshold;

    /**
     * @brief 그래디언트 계산
     * @param image 입력 그레이스케일 이미지
     * @return (gradientX, gradientY) 쌍
     */
    std::pair<cv::Mat, cv::Mat> calculateGradients(const cv::Mat& image);

    /**
     * @brief 그래디언트 크기와 방향 계산
     * @param gradientX X 방향 그래디언트
     * @param gradientY Y 방향 그래디언트
     * @return (magnitude, angle) 쌍
     */
    std::pair<cv::Mat, cv::Mat> calculateMagnitudeAndAngle(
        const cv::Mat& gradientX, const cv::Mat& gradientY);

    /**
     * @brief 방향성 히스토그램 계산
     * @param magnitude 그래디언트 크기
     * @param angle 그래디언트 방향
     * @return 방향성 히스토그램
     */
    std::vector<float> calculateHistogram(const cv::Mat& magnitude, const cv::Mat& angle);

    /**
     * @brief 크기 임계값 필터링
     * @param magnitude 그래디언트 크기
     * @return 필터링된 크기
     */
    cv::Mat thresholdMagnitude(const cv::Mat& magnitude);
};

} // namespace vve