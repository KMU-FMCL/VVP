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

auto ImageProcessor::create_visualization(cv::Mat const& input_image,
                                          cv::Mat const& calibrated_image,
                                          HOGResult const& hog_result,
                                          VVResult const& vv_result,
                                          cv::Mat const& histogram_image,
                                          float fps) -> cv::Mat {
  // 원본 이미지에 VV 표시 추가
  cv::Mat input_with_vv =
      ImageProcessor::draw_vv_indicators(input_image.clone(), vv_result);

  // 보정된 이미지에 수평선 추가
  cv::Mat calibrated_with_line = calibrated_image.clone();
  cv::line(calibrated_with_line,
           cv::Point(0, calibrated_with_line.rows /
                            static_cast<int>(ImageConstants::kDivideByTwo)),
           cv::Point(calibrated_with_line.cols,
                     calibrated_with_line.rows /
                         static_cast<int>(ImageConstants::kDivideByTwo)),
           ImageConstants::Colors::kBlack,
           VisualizationConstants::kLineThickness, cv::LINE_AA);

  // 상단 이미지 가로로 합치기 (원본 + 보정)
  cv::Mat top_row;
  cv::hconcat(input_with_vv, calibrated_with_line, top_row);

  // HOG 결과 이미지 생성
  cv::Mat hog_magnitude;
  cv::Mat hog_magnitude_filtered;
  hog_result.magnitude.convertTo(hog_magnitude, CV_8U,
                                 ImageConstants::kMaxPixelValue);
  hog_result.magnitude_filtered.convertTo(hog_magnitude_filtered, CV_8U,
                                          ImageConstants::kMaxPixelValue);

  // 단일 채널을 3채널로 변환 (그레이스케일 -> 컬러)
  cv::Mat hog_magnitude_color;
  cv::Mat hog_magnitude_filtered_color;
  cv::cvtColor(hog_magnitude, hog_magnitude_color, cv::COLOR_GRAY2BGR);
  cv::cvtColor(hog_magnitude_filtered, hog_magnitude_filtered_color,
               cv::COLOR_GRAY2BGR);

  // 중간 이미지 가로로 합치기 (HOG 매그니튜드 + 필터링된 매그니튜드)
  cv::Mat middle_row;
  cv::hconcat(hog_magnitude_color, hog_magnitude_filtered_color, middle_row);

  // 모든 행 세로로 합치기 전에 크기 조정 확인
  cv::resize(middle_row, middle_row, top_row.size());

  // 히스토그램 이미지가 없거나 너비가 다른 경우 수정
  cv::Mat histogram_img = histogram_image;
  if (histogram_img.empty()) {
    // 빈 히스토그램 이미지 생성
    histogram_img =
        cv::Mat(top_row.rows / static_cast<int>(ImageConstants::kDivideByTwo),
                top_row.cols, CV_8UC3, ImageConstants::Colors::kWhite);
  } else if (histogram_img.cols != top_row.cols) {
    // 너비 맞추기
    cv::resize(histogram_img, histogram_img,
               cv::Size(top_row.cols, histogram_img.rows));
  }

  // 수직 합치기
  cv::Mat result;
  std::vector<cv::Mat> rows;
  rows.push_back(top_row);
  rows.push_back(middle_row);
  rows.push_back(histogram_img);

  cv::vconcat(rows, result);

  // FPS 정보 추가
  if (fps > 0.0F) {
    cv::putText(result, absl::StrFormat("FPS: %.1f", fps),
                cv::Point(VisualizationConstants::kFpsTextPositionX,
                          VisualizationConstants::kFpsTextPositionY),
                cv::FONT_HERSHEY_SIMPLEX, VisualizationConstants::kFpsTextScale,
                ImageConstants::Colors::kGreen,
                VisualizationConstants::kLineThickness);
  }

  return result;
}

auto ImageProcessor::draw_vv_indicators(cv::Mat image,
                                        VVResult const& vv_result) -> cv::Mat {
  try {
    // VV 각도 텍스트 추가
    cv::putText(image,
                absl::StrCat(" VV_dig=", static_cast<int>(vv_result.angle)),
                cv::Point(VisualizationConstants::kVvTextPositionX,
                          VisualizationConstants::kVvTextPositionY),
                cv::FONT_HERSHEY_PLAIN, VisualizationConstants::kVvTextScale,
                ImageConstants::Colors::kGreen,
                VisualizationConstants::kLineThickness, cv::LINE_AA);

    // 수평선과 수직선 추가
    cv::line(
        image,
        cv::Point(0,
                  image.rows / static_cast<int>(ImageConstants::kDivideByTwo)),
        cv::Point(image.cols,
                  image.rows / static_cast<int>(ImageConstants::kDivideByTwo)),
        ImageConstants::Colors::kBlack, VisualizationConstants::kLineThickness,
        cv::LINE_4);

    cv::line(
        image,
        cv::Point(image.cols / static_cast<int>(ImageConstants::kDivideByTwo),
                  image.rows / static_cast<int>(ImageConstants::kDivideByTwo)),
        cv::Point(image.cols / static_cast<int>(ImageConstants::kDivideByTwo),
                  image.rows),
        ImageConstants::Colors::kBlack, VisualizationConstants::kLineThickness,
        cv::LINE_4);

    // VV 선 그리기
    double radians = (AngleConstants::kRightAngle - vv_result.angle) *
                     AngleConstants::kDegToRad;
    double length = static_cast<double>(image.rows) /
                    static_cast<double>(ImageConstants::kDivideByTwo);
    double delta_x = length * std::cos(radians);
    double delta_y = length * std::sin(radians);

    cv::Point center(
        image.cols / static_cast<int>(ImageConstants::kDivideByTwo),
        image.rows / static_cast<int>(ImageConstants::kDivideByTwo));
    cv::Point end(static_cast<int>(center.x + delta_x),
                  static_cast<int>(center.y - delta_y));

    cv::line(image, center, end, ImageConstants::Colors::kGreen,
             VisualizationConstants::kLineThickness, cv::LINE_AA);

    // 가속도 벡터 그리기 (acc_x, acc_y)
    double acc_scale_factor =
        length / AngleConstants::kGravityAcceleration;  // 9.8 m/s^2를 length
                                                        // 픽셀로 스케일링
    cv::Point acc_vec(
        static_cast<int>(center.x + (vv_result.acc_x * acc_scale_factor)),
        static_cast<int>(center.y - (vv_result.acc_y * acc_scale_factor)));
    cv::arrowedLine(image, center, acc_vec, ImageConstants::Colors::kRed,
                    VisualizationConstants::kLineThickness, cv::LINE_AA);

    return image;
  } catch (std::exception const& e) {
    std::cerr << "Error drawing VV indicators: " << e.what() << '\n';
    return image;
  }
}

}  // namespace vv
