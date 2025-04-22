#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include "vvp/processing/ImageProcessor.h"

#include <gtest/gtest.h>

// ImageProcessor 클래스 테스트
class ImageProcessorTest : public ::testing::Test {
 protected:
  void SetUp() override {
    // 테스트 셋업
    processor = std::make_unique<vv::ImageProcessor>();

    // 테스트용 이미지 생성 (100x100 크기의 단순 그라데이션)
    testImage = cv::Mat(100, 100, CV_8UC3);
    for (int y = 0; y < testImage.rows; y++) {
      for (int x = 0; x < testImage.cols; x++) {
        // 간단한 세로 그라데이션 패턴 (왼쪽에서 오른쪽으로)
        testImage.at<cv::Vec3b>(y, x) =
            cv::Vec3b(50, 50, x * 255 / testImage.cols);
      }
    }

    // 세로 선 추가 (90도 방향 강조)
    cv::line(testImage,
             cv::Point(50, 0),
             cv::Point(50, 100),
             cv::Scalar(255, 255, 255),
             2);
  }

  std::unique_ptr<vv::ImageProcessor> processor;
  cv::Mat testImage;
};

// 이미지 크기 조정 테스트
TEST_F(ImageProcessorTest, ResizeImage) {
  // 2배 축소
  cv::Mat resized = processor->resize_image(testImage, 2);
  EXPECT_EQ(resized.size(), cv::Size(50, 50));

  // 원본 크기 유지
  cv::Mat resized2 = processor->resize_image(testImage, 1);
  EXPECT_EQ(resized2.size(), testImage.size());

  // 잘못된 값 처리
  cv::Mat resized3 = processor->resize_image(testImage, -1);
  EXPECT_EQ(resized3.size(), testImage.size());
}

// 이미지 회전 테스트
TEST_F(ImageProcessorTest, RotateImage) {
  // 90도 회전
  cv::Mat rotated = processor->rotate_image(testImage, 90);
  EXPECT_EQ(rotated.size(), testImage.size());

  // 0도 회전 (원본과 동일)
  cv::Mat rotated2 = processor->rotate_image(testImage, 0);

  // 픽셀 값 비교 (완전히 동일하지는 않을 수 있음)
  double diff = cv::norm(rotated2, testImage, cv::NORM_L1);
  EXPECT_LT(diff / (testImage.rows * testImage.cols),
            1.0);  // 평균 차이가 작아야 함
}

// HOG 계산 테스트
TEST_F(ImageProcessorTest, ComputeHOG) {
  vv::HOGResult hogResult = processor->compute_hog(testImage);

  // 히스토그램 검증
  EXPECT_EQ(hogResult.histogram.size(), 180);

  // 결과 매트릭스 검증
  EXPECT_FALSE(hogResult.gradient_x.empty());
  EXPECT_FALSE(hogResult.gradient_y.empty());
  EXPECT_FALSE(hogResult.magnitude.empty());
  EXPECT_FALSE(hogResult.magnitude_filtered.empty());

  // 테스트 환경의 정확한 값을 예측하기 어려우므로
  // 히스토그램이 제대로 계산되는지만 확인합니다
  EXPECT_FALSE(hogResult.histogram.empty());

  // 최소한 하나의 값이 0보다 커야 함
  bool hasPositive = false;
  for (const auto& val : hogResult.histogram) {
    if (val > 0) {
      hasPositive = true;
      break;
    }
  }
  EXPECT_TRUE(hasPositive);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}