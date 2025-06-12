#include "vvp/processing/image_processor.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_format.h"
#include "vvp/processing/constants.h"  // Explicitly include for clarity
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace vv {

ImageProcessor::ImageProcessor(HOGParams const& params) : params_(params) {
  // 침식 연산을 위한 커널 초기화
  erode_kernel_ = cv::getStructuringElement(
      cv::MORPH_RECT,
      cv::Size(params_.erode_kernel_size, params_.erode_kernel_size));
}

auto ImageProcessor::compute_hog(cv::Mat const& image) -> HOGResult {
  HOGResult result;

  // 그레이스케일 변환
  cv::Mat gray;
  cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);

  // 가우시안 블러 적용
  cv::GaussianBlur(gray, gray,
                   cv::Size(params_.blur_kernel_size, params_.blur_kernel_size),
                   params_.blur_sigma);

  // 0-1 범위로 정규화
  gray.convertTo(gray, CV_32F);
  cv::normalize(gray, gray, 0, 1, cv::NORM_MINMAX);

  // Sobel 그래디언트 계산
  cv::Mat gradient_x;
  cv::Mat gradient_y;
  cv::Sobel(gray, gradient_x, CV_32F, 1, 0);
  cv::Sobel(gray, gradient_y, CV_32F, 0, 1);
  gradient_y = -gradient_y;  // y 방향 반전 (Python 코드와 일치)

  // 그래디언트 크기(magnitude)와 방향(angle) 계산
  cv::Mat magnitude;
  cv::Mat angle;
  cv::cartToPolar(gradient_x, gradient_y, magnitude, angle);

  // 각도를 도(degree) 단위로 변환
  angle = angle * AngleConstants::kRadToDeg / CV_PI;

  // 그래디언트 크기 정규화 및 임계값 처리
  cv::normalize(magnitude, magnitude, 0, 1, cv::NORM_MINMAX);

  cv::Mat magnitude_filtered;
  cv::threshold(magnitude, magnitude_filtered, params_.threshold_value, 1,
                cv::THRESH_BINARY);

  // 침식 연산 적용
  cv::erode(magnitude_filtered, magnitude_filtered, erode_kernel_);
  cv::normalize(magnitude_filtered, magnitude_filtered, 0, 1, cv::NORM_MINMAX);

  // 각도 조정 (0-179도 범위로)
  cv::Mat angle_modified;
  cv::Mat mask = (angle == AngleConstants::kFullCircle);
  angle.copyTo(angle_modified);
  angle_modified.setTo(0, mask);

  mask = (angle >= AngleConstants::kRadToDeg);
  // 수정: mask 영역의 값만 변경
  cv::Mat temp = angle - AngleConstants::kRadToDeg;
  temp.copyTo(angle_modified, mask);

  // 히스토그램 계산
  std::vector<float> histogram(params_.bin_count, 0.0F);

  // 수동 히스토그램 계산: 각 픽셀의 각도 인덱스에 필터된 매그니튜드 누적
  int const rows = angle_modified.rows;
  int const cols = angle_modified.cols;
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < cols; ++j) {
      int bin = static_cast<int>(angle_modified.at<float>(i, j));
      if (static_cast<unsigned>(bin) <
          static_cast<unsigned>(params_.bin_count)) {
        histogram[bin] += magnitude_filtered.at<float>(i, j);
      }
    }
  }

  // 결과 설정
  result.gradient_x = gradient_x;
  result.gradient_y = gradient_y;
  result.histogram = histogram;
  result.magnitude = magnitude;
  result.magnitude_filtered = magnitude_filtered;

  return result;
}

}  // namespace vv
