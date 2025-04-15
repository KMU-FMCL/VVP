#pragma once

#include <opencv2/core.hpp>
#include <vector>

namespace vve {

/**
 * @brief 시각화 관련 기능을 제공하는 클래스
 */
class Visualizer {
public:
    /**
     * @brief 생성자
     * @param lineThickness 선 두께
     * @param arrowThickness 화살표 두께
     * @param fontScale 폰트 크기 스케일
     * @param fontThickness 폰트 두께
     * @param histYlim 히스토그램 Y축 최대값
     * @param histBarWidth 히스토그램 막대 너비
     */
    Visualizer(
        int lineThickness = 2,
        int arrowThickness = 4,
        double fontScale = 1.5,
        int fontThickness = 2,
        double histYlim = 0.2,
        int histBarWidth = 2
    );

    /**
     * @brief 소멸자
     */
    virtual ~Visualizer() = default;

    /**
     * @brief 결과 시각화
     * @param frame 원본 프레임
     * @param orientation 계산된 방향 각도
     * @param histogram 방향성 히스토그램
     * @param filteredMagnitude 필터링된 그래디언트 크기
     * @return 시각화된 결과 이미지
     */
    cv::Mat visualizeResults(
        const cv::Mat& frame,
        double orientation,
        const std::vector<float>& histogram,
        const cv::Mat& filteredMagnitude
    );

    /**
     * @brief 선 두께 설정
     * @param thickness 선 두께
     */
    void setLineThickness(int thickness);

    /**
     * @brief 화살표 두께 설정
     * @param thickness 화살표 두께
     */
    void setArrowThickness(int thickness);

    /**
     * @brief 폰트 크기 스케일 설정
     * @param scale 폰트 크기 스케일
     */
    void setFontScale(double scale);

    /**
     * @brief 폰트 두께 설정
     * @param thickness 폰트 두께
     */
    void setFontThickness(int thickness);

    /**
     * @brief 히스토그램 Y축 최대값 설정
     * @param ylim Y축 최대값
     */
    void setHistYlim(double ylim);

    /**
     * @brief 히스토그램 막대 너비 설정
     * @param width 막대 너비
     */
    void setHistBarWidth(int width);

private:
    int lineThickness;
    int arrowThickness;
    double fontScale;
    int fontThickness;
    double histYlim;
    int histBarWidth;

    /**
     * @brief 방향 화살표 그리기
     * @param image 대상 이미지
     * @param orientation 방향 각도
     * @param color 화살표 색상
     */
    void drawOrientationArrow(cv::Mat& image, double orientation, const cv::Scalar& color);

    /**
     * @brief 방향 텍스트 그리기
     * @param image 대상 이미지
     * @param orientation 방향 각도
     * @param color 텍스트 색상
     */
    void drawOrientationText(cv::Mat& image, double orientation, const cv::Scalar& color);

    /**
     * @brief 방향성 히스토그램 그리기
     * @param image 대상 이미지
     * @param histogram 방향성 히스토그램
     * @param color 히스토그램 색상
     */
    void drawHistogram(cv::Mat& image, const std::vector<float>& histogram, const cv::Scalar& color);

    /**
     * @brief 필터링된 그래디언트 크기 오버레이
     * @param image 대상 이미지
     * @param filteredMagnitude 필터링된 그래디언트 크기
     */
    void overlayFilteredMagnitude(cv::Mat& image, const cv::Mat& filteredMagnitude);

    /**
     * @brief 정보 텍스트 그리기
     * @param image 대상 이미지
     * @param fps 초당 프레임 수
     */
    void drawInfoText(cv::Mat& image, double fps);
};

} // namespace vve