#include "vvp/processing/ImageProcessor.h"

#include <iomanip>
#include <iostream>
#include <sstream>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "absl/strings/str_cat.h"
#include "absl/strings/str_format.h"

namespace vv {

ImageProcessor::ImageProcessor(const HOGParams& params) : params_(params) {
  // 침식 연산을 위한 커널 초기화
  erode_kernel_ = cv::getStructuringElement(
      cv::MORPH_RECT,
      cv::Size(params_.erode_kernel_size, params_.erode_kernel_size));
}

HOGResult ImageProcessor::compute_hog(const cv::Mat& image) {
  HOGResult result;

  // 그레이스케일 변환
  cv::Mat gray;
  cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);

  // 가우시안 블러 적용
  cv::GaussianBlur(gray,
                   gray,
                   cv::Size(params_.blur_kernel_size, params_.blur_kernel_size),
                   params_.blur_sigma);

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

  cv::Mat mag_filter;
  cv::threshold(mag, mag_filter, params_.threshold_value, 1, cv::THRESH_BINARY);

  // 침식 연산 적용
  cv::erode(mag_filter, mag_filter, erode_kernel_);
  cv::normalize(mag_filter, mag_filter, 0, 1, cv::NORM_MINMAX);

  // 각도 조정 (0-179도 범위로)
  cv::Mat ang_mod;
  cv::Mat mask = (ang == 360);
  ang.copyTo(ang_mod);
  ang_mod.setTo(0, mask);

  mask = (ang >= 180);
  // 수정: mask 영역의 값만 변경
  cv::Mat temp = ang - 180;
  temp.copyTo(ang_mod, mask);

  // 히스토그램 계산
  std::vector<float> hist(params_.bin_count, 0.0f);

  // 수동 히스토그램 계산: 각 픽셀의 각도 인덱스에 필터된 매그니튜드 누적
  const int rows = ang_mod.rows;
  const int cols = ang_mod.cols;
  for (int i = 0; i < rows; ++i) {
    const float* ang_ptr = ang_mod.ptr<float>(i);
    const float* mag_ptr = mag_filter.ptr<float>(i);
    for (int j = 0; j < cols; ++j) {
      int bin = static_cast<int>(ang_ptr[j]);
      if (static_cast<unsigned>(bin) <
          static_cast<unsigned>(params_.bin_count)) {
        hist[bin] += mag_ptr[j];
      }
    }
  }

  // 결과 설정
  result.gradient_x = gx;
  result.gradient_y = gy;
  result.histogram = hist;
  result.magnitude = mag;
  result.magnitude_filtered = mag_filter;

  return result;
}

cv::Mat ImageProcessor::resize_image(const cv::Mat& image, int scale) const {
  if (scale <= 0 || scale == 1) {
    return image.clone();
  }

  cv::Mat resized;
  cv::resize(image,
             resized,
             cv::Size(image.cols / scale, image.rows / scale),
             0,
             0,
             cv::INTER_LINEAR);

  return resized;
}

cv::Mat ImageProcessor::rotate_image(const cv::Mat& image, double angle) const {
  cv::Point2f center(image.cols / 2.0f, image.rows / 2.0f);
  cv::Mat rot_mat = cv::getRotationMatrix2D(center, angle, 1.0);
  cv::Mat rotated;

  cv::warpAffine(image, rotated, rot_mat, image.size(), cv::INTER_LINEAR);

  return rotated;
}

cv::Mat ImageProcessor::create_visualization(const cv::Mat& input_image,
                                             const cv::Mat& calibrated_image,
                                             const HOGResult& hog_result,
                                             const VVResult& vv_result,
                                             const cv::Mat& histogram_image,
                                             float fps) const {
  // 원본 이미지에 VV 표시 추가
  cv::Mat input_with_vv = draw_vv_indicators(input_image.clone(), vv_result);

  // 보정된 이미지에 수평선 추가
  cv::Mat calibrated_with_line = calibrated_image.clone();
  cv::line(calibrated_with_line,
           cv::Point(0, calibrated_with_line.rows / 2),
           cv::Point(calibrated_with_line.cols, calibrated_with_line.rows / 2),
           cv::Scalar(0, 0, 0),
           kLineThickness,
           cv::LINE_AA);

  // 상단 이미지 가로로 합치기 (원본 + 보정)
  cv::Mat top_row;
  cv::hconcat(input_with_vv, calibrated_with_line, top_row);

  // HOG 결과 이미지 생성
  cv::Mat hog_mag, hog_mag_filter;
  hog_result.magnitude.convertTo(hog_mag, CV_8U, 255);
  hog_result.magnitude_filtered.convertTo(hog_mag_filter, CV_8U, 255);

  // 단일 채널을 3채널로 변환 (그레이스케일 -> 컬러)
  cv::Mat hog_mag_color, hog_mag_filter_color;
  cv::cvtColor(hog_mag, hog_mag_color, cv::COLOR_GRAY2BGR);
  cv::cvtColor(hog_mag_filter, hog_mag_filter_color, cv::COLOR_GRAY2BGR);

  // 중간 이미지 가로로 합치기 (HOG 매그니튜드 + 필터링된 매그니튜드)
  cv::Mat middle_row;
  cv::hconcat(hog_mag_color, hog_mag_filter_color, middle_row);

  // 모든 행 세로로 합치기 전에 크기 조정 확인
  cv::resize(middle_row, middle_row, top_row.size());

  // 히스토그램 이미지가 없거나 너비가 다른 경우 수정
  cv::Mat hist_image = histogram_image;
  if (hist_image.empty()) {
    // 빈 히스토그램 이미지 생성
    hist_image = cv::Mat(
        top_row.rows / 2, top_row.cols, CV_8UC3, cv::Scalar(255, 255, 255));
  } else if (hist_image.cols != top_row.cols) {
    // 너비 맞추기
    cv::resize(hist_image, hist_image, cv::Size(top_row.cols, hist_image.rows));
  }

  // 수직 합치기
  cv::Mat result;
  std::vector<cv::Mat> rows;
  rows.push_back(top_row);
  rows.push_back(middle_row);
  rows.push_back(hist_image);

  cv::vconcat(rows, result);

  // FPS 정보 추가
  if (fps > 0.0f) {
    cv::putText(result,
                absl::StrFormat("FPS: %.1f", fps),
                cv::Point(kFpsTextPositionX, kFpsTextPositionY),
                cv::FONT_HERSHEY_SIMPLEX,
                kFpsTextScale,
                cv::Scalar(0, 255, 0),
                kLineThickness);
  }

  return result;
}

cv::Mat ImageProcessor::draw_vv_indicators(cv::Mat image,
                                           const VVResult& vv_result) const {
  try {
    // VV 각도 텍스트 추가
    cv::putText(image,
                absl::StrCat(" VV_dig=", static_cast<int>(vv_result.angle)),
                cv::Point(kVvTextPositionX, kVvTextPositionY),
                cv::FONT_HERSHEY_PLAIN,
                kVvTextScale,
                cv::Scalar(0, 255, 0),
                kLineThickness,
                cv::LINE_AA);

    // 수평선과 수직선 추가
    cv::line(image,
             cv::Point(0, image.rows / 2),
             cv::Point(image.cols, image.rows / 2),
             cv::Scalar(0, 0, 0),
             kLineThickness,
             cv::LINE_4);

    cv::line(image,
             cv::Point(image.cols / 2, image.rows / 2),
             cv::Point(image.cols / 2, image.rows),
             cv::Scalar(0, 0, 0),
             kLineThickness,
             cv::LINE_4);

    // VV 선 그리기
    double radians = (90 - vv_result.angle) * CV_PI / 180.0;
    double length = static_cast<double>(image.rows) / 2.0;
    double dx = length * std::cos(radians);
    double dy = length * std::sin(radians);

    cv::Point center(image.cols / 2, image.rows / 2);
    cv::Point end(static_cast<int>(center.x + dx),
                  static_cast<int>(center.y - dy));

    cv::line(
        image, center, end, cv::Scalar(0, 255, 0), kLineThickness, cv::LINE_AA);

    // 가속도 벡터 그리기 (acc_x, acc_y)
    double acc_scale_factor =
        length / kGravityAcceleration;  // 9.8 m/s^2를 length 픽셀로 스케일링
    cv::Point acc_vec(
        static_cast<int>(center.x + vv_result.acc_x * acc_scale_factor),
        static_cast<int>(center.y - vv_result.acc_y * acc_scale_factor));
    cv::arrowedLine(image,
                    center,
                    acc_vec,
                    cv::Scalar(0, 0, 255),
                    kLineThickness,
                    cv::LINE_AA);

    return image;
  } catch (const std::exception& e) {
    std::cerr << "Error drawing VV indicators: " << e.what() << std::endl;
    return image;
  }
}

}  // namespace vv
