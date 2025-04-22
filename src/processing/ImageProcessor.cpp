#include "vvp/processing/ImageProcessor.hpp"

#include <iomanip>
#include <iostream>
#include <sstream>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

namespace vv {

ImageProcessor::ImageProcessor(const HOGParams& params) : params_(params) {
  // 침식 연산을 위한 커널 초기화
  erode_kernel_ = cv::getStructuringElement(
      cv::MORPH_RECT,
      cv::Size(params_.erodeKernelSize, params_.erodeKernelSize));
}

HOGResult ImageProcessor::compute_hog(const cv::Mat& image) {
  HOGResult result;

  // 그레이스케일 변환
  cv::Mat gray;
  cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);

  // 가우시안 블러 적용
  cv::GaussianBlur(gray, gray,
                   cv::Size(params_.blurKernelSize, params_.blurKernelSize),
                   params_.blurSigma);

  // 0-1 범위로 정규화
  gray.convertTo(gray, CV_32F);
  cv::normalize(gray, gray, 0, 1, cv::NORM_MINMAX);

  // Sobel 그래디언트 계산
  cv::Mat gx, gy;
  cv::Sobel(gray, gx, CV_32F, 1, 0);
  cv::Sobel(gray, gy, CV_32F, 0, 1);
  gy = -gy;  // y 방향 반전 (Python 코드와 일치)

  // 그래디언트 크기(magnitude)와 방향(angle) 계산
  cv::Mat mag, ang;
  cv::cartToPolar(gx, gy, mag, ang);

  // 각도를 도(degree) 단위로 변환
  ang = ang * 180 / CV_PI;

  // 그래디언트 크기 정규화 및 임계값 처리
  cv::normalize(mag, mag, 0, 1, cv::NORM_MINMAX);

  cv::Mat magFilter;
  cv::threshold(mag, magFilter, params_.thresholdValue, 1, cv::THRESH_BINARY);

  // 침식 연산 적용
  cv::erode(magFilter, magFilter, erode_kernel_);
  cv::normalize(magFilter, magFilter, 0, 1, cv::NORM_MINMAX);

  // 각도 조정 (0-179도 범위로)
  cv::Mat angMod;
  cv::Mat mask = (ang == 360);
  ang.copyTo(angMod);
  angMod.setTo(0, mask);

  mask = (ang >= 180);
  // 수정: mask 영역의 값만 변경
  cv::Mat temp = ang - 180;
  temp.copyTo(angMod, mask);

  // 히스토그램 계산
  std::vector<float> hist(params_.binCount, 0.0f);

  // 수동 히스토그램 계산: 각 픽셀의 각도 인덱스에 필터된 매그니튜드 누적
  const int rows = angMod.rows;
  const int cols = angMod.cols;
  for (int i = 0; i < rows; ++i) {
    const float* angPtr = angMod.ptr<float>(i);
    const float* magPtr = magFilter.ptr<float>(i);
    for (int j = 0; j < cols; ++j) {
      int bin = static_cast<int>(angPtr[j]);
      if (static_cast<unsigned>(bin) <
          static_cast<unsigned>(params_.binCount)) {
        hist[bin] += magPtr[j];
      }
    }
  }

  // 결과 설정
  result.gradientX = gx;
  result.gradientY = gy;
  result.histogram = hist;
  result.magnitude = mag;
  result.magnitudeFiltered = magFilter;

  return result;
}

cv::Mat ImageProcessor::resize_image(const cv::Mat& image, int scale) const {
  if (scale <= 0 || scale == 1) {
    return image.clone();
  }

  cv::Mat resized;
  cv::resize(image, resized, cv::Size(image.cols / scale, image.rows / scale),
             0, 0, cv::INTER_LINEAR);

  return resized;
}

cv::Mat ImageProcessor::rotate_image(const cv::Mat& image, double angle) const {
  cv::Point2f center(image.cols / 2.0f, image.rows / 2.0f);
  cv::Mat rotMat = cv::getRotationMatrix2D(center, angle, 1.0);
  cv::Mat rotated;

  cv::warpAffine(image, rotated, rotMat, image.size(), cv::INTER_LINEAR);

  return rotated;
}

cv::Mat ImageProcessor::create_visualization(const cv::Mat& inputImage,
                                             const cv::Mat& calibratedImage,
                                             const HOGResult& hogResult,
                                             const VVResult& vvResult,
                                             const cv::Mat& histogramImage,
                                             float fps) const {
  // 원본 이미지에 VV 표시 추가
  cv::Mat inputWithVV = draw_vv_indicators(inputImage.clone(), vvResult);

  // 보정된 이미지에 수평선 추가
  cv::Mat calibratedWithLine = calibratedImage.clone();
  cv::line(calibratedWithLine, cv::Point(0, calibratedWithLine.rows / 2),
           cv::Point(calibratedWithLine.cols, calibratedWithLine.rows / 2),
           cv::Scalar(0, 0, 0), 2, cv::LINE_AA);

  // 상단 이미지 가로로 합치기 (원본 + 보정)
  cv::Mat topRow;
  cv::hconcat(inputWithVV, calibratedWithLine, topRow);

  // HOG 결과 이미지 생성
  cv::Mat hogMag, hogMagFilter;
  hogResult.magnitude.convertTo(hogMag, CV_8U, 255);
  hogResult.magnitudeFiltered.convertTo(hogMagFilter, CV_8U, 255);

  // 단일 채널을 3채널로 변환 (그레이스케일 -> 컬러)
  cv::Mat hogMagColor, hogMagFilterColor;
  cv::cvtColor(hogMag, hogMagColor, cv::COLOR_GRAY2BGR);
  cv::cvtColor(hogMagFilter, hogMagFilterColor, cv::COLOR_GRAY2BGR);

  // 중간 이미지 가로로 합치기 (HOG 매그니튜드 + 필터링된 매그니튜드)
  cv::Mat middleRow;
  cv::hconcat(hogMagColor, hogMagFilterColor, middleRow);

  // 모든 행 세로로 합치기 전에 크기 조정 확인
  cv::resize(middleRow, middleRow, topRow.size());

  // 히스토그램 이미지가 없거나 너비가 다른 경우 수정
  cv::Mat histImage = histogramImage;
  if (histImage.empty()) {
    // 빈 히스토그램 이미지 생성
    histImage = cv::Mat(topRow.rows / 2, topRow.cols, CV_8UC3,
                        cv::Scalar(255, 255, 255));
  } else if (histImage.cols != topRow.cols) {
    // 너비 맞추기
    cv::resize(histImage, histImage, cv::Size(topRow.cols, histImage.rows));
  }

  // 수직 합치기
  cv::Mat result;
  std::vector<cv::Mat> rows;
  rows.push_back(topRow);
  rows.push_back(middleRow);
  rows.push_back(histImage);

  cv::vconcat(rows, result);

  // FPS 정보 추가
  if (fps > 0.0f) {
    std::stringstream ss;
    ss << "FPS: " << std::fixed << std::setprecision(1) << fps;
    cv::putText(result, ss.str(), cv::Point(1100, 30), cv::FONT_HERSHEY_SIMPLEX,
                1.0, cv::Scalar(0, 255, 0), 2);
  }

  return result;
}

cv::Mat ImageProcessor::draw_vv_indicators(cv::Mat image,
                                           const VVResult& vvResult) const {
  try {
    // VV 각도 텍스트 추가
    cv::putText(image,
                " VV_dig=" + std::to_string(static_cast<int>(vvResult.angle)),
                cv::Point(10, 30), cv::FONT_HERSHEY_PLAIN, 2,
                cv::Scalar(0, 255, 0), 2, cv::LINE_AA);

    // 수평선과 수직선 추가
    cv::line(image, cv::Point(0, image.rows / 2),
             cv::Point(image.cols, image.rows / 2), cv::Scalar(0, 0, 0), 2,
             cv::LINE_4);

    cv::line(image, cv::Point(image.cols / 2, image.rows / 2),
             cv::Point(image.cols / 2, image.rows), cv::Scalar(0, 0, 0), 2,
             cv::LINE_4);

    // VV 선 그리기
    double radians = (90 - vvResult.angle) * CV_PI / 180.0;
    double length = static_cast<double>(image.rows) / 2.0;
    double dx = length * std::cos(radians);
    double dy = length * std::sin(radians);

    cv::Point center(image.cols / 2, image.rows / 2);
    cv::Point end(static_cast<int>(center.x + dx),
                  static_cast<int>(center.y - dy));

    cv::line(image, center, end, cv::Scalar(0, 255, 0), 2, cv::LINE_AA);

    // 가속도 벡터 그리기 (accX, accY)
    double accScaleFactor = length / 9.8;  // 9.8 m/s^2를 length 픽셀로 스케일링
    cv::Point accVec(
        static_cast<int>(center.x + vvResult.accX * accScaleFactor),
        static_cast<int>(center.y - vvResult.accY * accScaleFactor));
    cv::arrowedLine(image, center, accVec, cv::Scalar(0, 0, 255), 2,
                    cv::LINE_AA);

    return image;
  } catch (const std::exception& e) {
    std::cerr << "Error drawing VV indicators: " << e.what() << std::endl;
    return image;
  }
}

}  // namespace vv
