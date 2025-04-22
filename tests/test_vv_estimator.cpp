#include "vvp/estimation/VVEstimator.h"
#include "vvp/utils/ConfigLoader.h"

#include <gtest/gtest.h>

// VVEstimator 클래스 테스트
class VVEstimatorTest : public ::testing::Test {
 protected:
  void SetUp() override {
    // ConfigReader::parse_config_file 대신 ConfigLoader::load 사용
    estimator = std::make_unique<vv::VVEstimator>(vv::VVParams());
    testHistogram.resize(180);
    initTestData();
  }

  void initTestData() {
    // 인공적인 HOG 결과 데이터 생성
    // 예: 90도(수직)에 피크가 있는 히스토그램
    for (int i = 0; i < 180; i++) {
      double gaussian = std::exp(-0.5 * std::pow((i - 90) / 10.0, 2));
      testHistogram[i] = gaussian * 100.0;
    }

    // VVResult 객체 초기화
    previousResult.angle = 90.0;
    previousResult.update_acceleration();
  }

  std::unique_ptr<vv::VVEstimator> estimator;
  std::vector<float> testHistogram;
  vv::VVResult previousResult;
};

// VV 각도 추정 테스트
TEST_F(VVEstimatorTest, EstimateVV) {
  // estimate_vv 메서드 사용하여 테스트
  vv::VVResult result = estimator->estimate_vv(testHistogram, previousResult);

  // 예상되는 값 범위 안에 있는지 확인
  EXPECT_NEAR(result.angle, 90.0, 5.0);  // 주요 방향이 90도에 가까워야 함
}

// 결과 히스토리 테스트
TEST_F(VVEstimatorTest, GetAllResults) {
  // 먼저 여러 개의 결과 생성
  for (int i = 0; i < 3; i++) {
    estimator->estimate_vv(testHistogram, previousResult);
  }

  // get_all_results 메서드로 모든 결과 얻기
  const auto& allResults = estimator->get_all_results();

  // 추정한 결과 수와 일치하는지 확인
  EXPECT_EQ(allResults.size(), 3);
}

// 히스토그램 시각화 테스트
TEST_F(VVEstimatorTest, CreateHistogramVisualization) {
  vv::VVResult result = estimator->estimate_vv(testHistogram, previousResult);

  // 히스토그램 시각화 생성
  cv::Mat histImage = estimator->create_histogram_visualization(
      testHistogram, result, 300, 200);

  // 이미지가 생성되었는지 확인
  EXPECT_FALSE(histImage.empty());
  EXPECT_EQ(histImage.cols, 300);
  EXPECT_EQ(histImage.rows, 200);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
