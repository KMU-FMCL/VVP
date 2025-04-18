#pragma once

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include "visual_vertical/Types.hpp"

namespace vv {

/**
 * @brief 이미지 처리 클래스
 * 
 * HOG 계산 및 이미지 변환, 처리를 담당하는 클래스입니다.
 */
class ImageProcessor {
public:
    /**
     * @brief 생성자
     * @param params HOG 계산 파라미터
     */
    explicit ImageProcessor(const HOGParams& params = HOGParams());

    /**
     * @brief HOG(Histogram of Oriented Gradients) 계산
     * @param image 입력 이미지
     * @return HOG 계산 결과 (그래디언트, 히스토그램, 매그니튜드 등)
     */
    HOGResult computeHOG(const cv::Mat& image);

    /**
     * @brief 이미지 크기 조정
     * @param image 입력 이미지
     * @param scale 크기 조정 비율
     * @return 크기가 조정된 이미지
     */
    cv::Mat resizeImage(const cv::Mat& image, int scale) const;

    /**
     * @brief 이미지 회전
     * @param image 입력 이미지
     * @param angle 회전 각도 (도)
     * @return 회전된 이미지
     */
    cv::Mat rotateImage(const cv::Mat& image, double angle) const;

    /**
     * @brief 결과 시각화 이미지 생성
     * @param inputImage 원본 입력 이미지
     * @param calibratedImage 보정된 이미지
     * @param hogResult HOG 계산 결과
     * @param vvResult VV 추정 결과
     * @param histogramImage 히스토그램 이미지
     * @param fps 프레임 속도
     * @return 모든 결과가 결합된 시각화 이미지
     */
    cv::Mat createVisualization(
        const cv::Mat& inputImage, 
        const cv::Mat& calibratedImage,
        const HOGResult& hogResult,
        const VVResult& vvResult,
        const cv::Mat& histogramImage,
        float fps = 0.0f
    ) const;

private:
    HOGParams m_params;
    cv::Mat m_erodeKernel;

    /**
     * @brief 이미지에 VV 각도 선 그리기
     * @param image 대상 이미지
     * @param vvResult VV 추정 결과
     * @return VV 표시가 추가된 이미지
     */
    cv::Mat drawVVIndicators(cv::Mat image, const VVResult& vvResult) const;
};

} // namespace vv 