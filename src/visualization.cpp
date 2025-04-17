#include "VisualVerticalEstimator/visualization.h"
#include "VisualVerticalEstimator/constants.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <sstream>
#include <iomanip>

namespace vve {

Visualizer::Visualizer(
    int lineThickness,
    int arrowThickness,
    double fontScale,
    int fontThickness,
    double histYlim,
    int histBarWidth
) : lineThickness(lineThickness),
    arrowThickness(arrowThickness),
    fontScale(fontScale),
    fontThickness(fontThickness),
    histYlim(histYlim),
    histBarWidth(histBarWidth) {
}

cv::Mat Visualizer::visualizeResults(
    const cv::Mat& frame,
    double orientation,
    const std::vector<float>& histogram,
    const cv::Mat& filteredMagnitude
) {
    // 결과 이미지 준비
    cv::Mat result = frame.clone();
    
    // 필터링된 그래디언트 크기 오버레이
    overlayFilteredMagnitude(result, filteredMagnitude);
    
    // 방향 화살표 그리기
    drawOrientationArrow(result, orientation, cv::Scalar(0, 255, 0));
    
    // 방향 텍스트 그리기
    drawOrientationText(result, orientation, cv::Scalar(255, 255, 255));
    
    // 히스토그램 그리기
    drawHistogram(result, histogram, cv::Scalar(0, 255, 255));
    
    // 정보 텍스트 그리기 (FPS는 외부에서 계산하여 전달할 수 있음)
    // 여기서는 기본값 0.0 사용
    drawInfoText(result, 0.0);
    
    return result;
}

void Visualizer::drawOrientationArrow(cv::Mat& image, double orientation, const cv::Scalar& color) {
    int centerX = image.cols / 2;
    int centerY = image.rows / 2;
    int arrowLength = std::min(image.rows, image.cols) / 3;
    
    // 각도를 라디안으로 변환 (OpenCV 좌표계: 0도는 오른쪽, 시계 방향)
    double angleRad = orientation * CV_PI / 180.0;
    
    // 화살표 끝점 계산
    int endX = centerX + static_cast<int>(arrowLength * std::cos(angleRad));
    int endY = centerY - static_cast<int>(arrowLength * std::sin(angleRad));
    
    // 화살표 그리기
    cv::arrowedLine(image, cv::Point(centerX, centerY), cv::Point(endX, endY), 
                    color, arrowThickness, cv::LINE_AA, 0, 0.3);
}

void Visualizer::drawOrientationText(cv::Mat& image, double orientation, const cv::Scalar& color) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(1) << "방향: " << orientation << "°";
    
    int textX = 20;
    int textY = 50;
    
    // 텍스트 배경 그리기
    cv::Size textSize = cv::getTextSize(ss.str(), cv::FONT_HERSHEY_SIMPLEX, 
                                        fontScale, fontThickness, nullptr);
    cv::Rect textRect(textX - 5, textY - textSize.height - 5, 
                      textSize.width + 10, textSize.height + 10);
    cv::rectangle(image, textRect, cv::Scalar(0, 0, 0), -1);
    
    // 텍스트 그리기
    cv::putText(image, ss.str(), cv::Point(textX, textY), 
                cv::FONT_HERSHEY_SIMPLEX, fontScale, color, fontThickness, cv::LINE_AA);
}

void Visualizer::drawHistogram(cv::Mat& image, const std::vector<float>& histogram, const cv::Scalar& color) {
    if (histogram.empty()) {
        return;
    }
    
    // 히스토그램 영역 설정
    int histHeight = image.rows / 4;
    int histWidth = histogram.size() * histBarWidth;
    int histX = (image.cols - histWidth) / 2;
    int histY = image.rows - histHeight - 20;
    
    // 히스토그램 배경 그리기
    cv::Rect histRect(histX, histY, histWidth, histHeight);
    cv::rectangle(image, histRect, cv::Scalar(0, 0, 0), -1);
    
    // 히스토그램 막대 그리기
    for (size_t i = 0; i < histogram.size(); ++i) {
        float value = std::min(static_cast<float>(histogram[i]), static_cast<float>(histYlim));
        int barHeight = static_cast<int>(value * histHeight / histYlim);
        
        cv::Rect barRect(histX + i * histBarWidth, histY + histHeight - barHeight, 
                         histBarWidth, barHeight);
        cv::rectangle(image, barRect, color, -1);
    }
    
    // 히스토그램 테두리 그리기
    cv::rectangle(image, histRect, cv::Scalar(255, 255, 255), 1);
    
    // X축 레이블 그리기 (0°, 90°, 180°)
    cv::putText(image, "0°", cv::Point(histX, histY + histHeight + 15), 
                cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1, cv::LINE_AA);
    cv::putText(image, "90°", cv::Point(histX + histWidth / 2 - 15, histY + histHeight + 15), 
                cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1, cv::LINE_AA);
    cv::putText(image, "180°", cv::Point(histX + histWidth - 30, histY + histHeight + 15), 
                cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1, cv::LINE_AA);
}

void Visualizer::overlayFilteredMagnitude(cv::Mat& image, const cv::Mat& filteredMagnitude) {
    if (filteredMagnitude.empty() || filteredMagnitude.size() != image.size()) {
        return;
    }
    
    for (int y = 0; y < image.rows; ++y) {
        for (int x = 0; x < image.cols; ++x) {
            if (filteredMagnitude.at<float>(y, x) > 0.0f) {
                // 빨간색으로 오버레이
                cv::Vec3b& pixel = image.at<cv::Vec3b>(y, x);
                pixel[0] = 0;      // B
                pixel[1] = 0;      // G
                pixel[2] = 255;    // R
            }
        }
    }
}

void Visualizer::drawInfoText(cv::Mat& image, double fps) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(1) << "FPS: " << fps;
    
    int textX = 20;
    int textY = image.rows - 20;
    
    // 텍스트 배경 그리기
    cv::Size textSize = cv::getTextSize(ss.str(), cv::FONT_HERSHEY_SIMPLEX, 
                                        0.6, 1, nullptr);
    cv::Rect textRect(textX - 5, textY - textSize.height - 5, 
                      textSize.width + 10, textSize.height + 10);
    cv::rectangle(image, textRect, cv::Scalar(0, 0, 0), -1);
    
    // 텍스트 그리기
    cv::putText(image, ss.str(), cv::Point(textX, textY), 
                cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(255, 255, 255), 1, cv::LINE_AA);
}

} // namespace vve