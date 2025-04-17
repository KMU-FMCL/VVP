#include "VisualVerticalEstimator/hog_calculator.h"
#include "VisualVerticalEstimator/constants.h"
#include "VisualVerticalEstimator/utils.h"
#include <opencv2/imgproc.hpp>

namespace vve {

HOGCalculator::HOGCalculator(
    int numBins,
    int gaussianKernelSizeX,
    int gaussianKernelSizeY,
    double gaussianSigma,
    double magnitudeThreshold
) : numBins(numBins),
    gaussianKernelSize(gaussianKernelSizeX, gaussianKernelSizeY),
    gaussianSigma(gaussianSigma),
    magnitudeThreshold(magnitudeThreshold) {
}

HOGCalculator::HOGFeatures HOGCalculator::calculate(const cv::Mat& image) {
    // 그레이스케일 변환
    cv::Mat gray;
    if (image.channels() == 3) {
        cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    } else {
        gray = image.clone();
    }

    // 그래디언트 계산
    auto [gradientX, gradientY] = calculateGradients(gray);

    // 크기와 방향 계산
    auto [magnitude, angle] = calculateMagnitudeAndAngle(gradientX, gradientY);

    // 방향성 히스토그램 계산
    std::vector<float> histogram = calculateHistogram(magnitude, angle);

    // 크기 임계값 필터링
    cv::Mat filteredMagnitude = thresholdMagnitude(magnitude);

    // 결과 반환
    HOGFeatures features;
    features.gradientY = gradientY;
    features.histogram = histogram;
    features.magnitude = magnitude;
    features.filteredMagnitude = filteredMagnitude;

    return features;
}

std::pair<cv::Mat, cv::Mat> HOGCalculator::calculateGradients(const cv::Mat& image) {
    cv::Mat blurred;
    cv::GaussianBlur(image, blurred, gaussianKernelSize, gaussianSigma);

    cv::Mat gradientX, gradientY;
    cv::Sobel(blurred, gradientX, CV_32F, 1, 0, 3);
    cv::Sobel(blurred, gradientY, CV_32F, 0, 1, 3);

    return {gradientX, gradientY};
}

std::pair<cv::Mat, cv::Mat> HOGCalculator::calculateMagnitudeAndAngle(
    const cv::Mat& gradientX, const cv::Mat& gradientY) {
    
    cv::Mat magnitude, angle;
    cv::cartToPolar(gradientX, gradientY, magnitude, angle, true);  // true: 각도를 도(degree) 단위로 계산

    return {magnitude, angle};
}

std::vector<float> HOGCalculator::calculateHistogram(const cv::Mat& magnitude, const cv::Mat& angle) {
    // 히스토그램 초기화
    std::vector<float> histogram(numBins, 0.0f);

    // 히스토그램 계산
    for (int y = 0; y < magnitude.rows; ++y) {
        for (int x = 0; x < magnitude.cols; ++x) {
            float mag = magnitude.at<float>(y, x);
            float ang = angle.at<float>(y, x);

            // 각도가 0-180도 범위에 있도록 조정
            if (ang > 180.0f) {
                ang -= 180.0f;
            }

            // 각도를 히스토그램 빈 인덱스로 변환
            int bin = static_cast<int>(ang * numBins / 180.0f) % numBins;
            
            // 크기를 가중치로 사용하여 히스토그램 빈 값 증가
            histogram[bin] += mag;
        }
    }

    // 히스토그램 정규화
    float sum = 0.0f;
    for (float value : histogram) {
        sum += value;
    }

    if (sum > 0.0f) {
        for (float& value : histogram) {
            value /= sum;
        }
    }

    return histogram;
}

cv::Mat HOGCalculator::thresholdMagnitude(const cv::Mat& magnitude) {
    // 크기 정규화
    cv::Mat normalizedMagnitude;
    cv::normalize(magnitude, normalizedMagnitude, 0, 1, cv::NORM_MINMAX);

    // 임계값 적용
    cv::Mat thresholded;
    cv::threshold(normalizedMagnitude, thresholded, magnitudeThreshold, 1.0, cv::THRESH_BINARY);

    // 침식 연산 적용 (노이즈 제거)
    cv::Mat eroded;
    cv::Mat kernel = cv::getStructuringElement(
        cv::MORPH_RECT, 
        cv::Size(EROSION_KERNEL_SIZE_X, EROSION_KERNEL_SIZE_Y)
    );
    cv::erode(thresholded, eroded, kernel, cv::Point(-1, -1), EROSION_ITERATIONS);

    return eroded;
}

} // namespace vve