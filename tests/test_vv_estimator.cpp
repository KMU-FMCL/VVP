#include <gtest/gtest.h>
#include "visual_vertical/VVEstimator.hpp"

// VVEstimator 클래스 테스트
class VVEstimatorTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 테스트 셋업
        estimator = std::make_unique<vv::VVEstimator>();
    }
    
    std::unique_ptr<vv::VVEstimator> estimator;
};

// 빈 히스토그램으로 VV 추정 테스트
TEST_F(VVEstimatorTest, EstimateVVEmptyHistogram) {
    std::vector<float> emptyHistogram;
    vv::VVResult prevResult;
    prevResult.angle = 90.0;
    prevResult.updateAcceleration();
    
    vv::VVResult result = estimator->estimateVV(emptyHistogram, prevResult);
    
    // 빈 히스토그램에서는 이전 결과가 그대로 반환되어야 함
    EXPECT_DOUBLE_EQ(result.angle, prevResult.angle);
    EXPECT_DOUBLE_EQ(result.angleRad, prevResult.angleRad);
    EXPECT_NEAR(result.accX, prevResult.accX, 1e-10);
    EXPECT_NEAR(result.accY, prevResult.accY, 1e-10);
}

// 단일 피크가 있는 히스토그램으로 VV 추정 테스트
TEST_F(VVEstimatorTest, EstimateVVSinglePeak) {
    std::vector<float> histogram(180, 0.0f);
    
    // 90도에 강한 피크 생성
    histogram[90] = 1.0f;
    
    vv::VVResult prevResult;
    prevResult.angle = 85.0;
    prevResult.updateAcceleration();
    
    vv::VVResult result = estimator->estimateVV(histogram, prevResult);
    
    // 피크가 90도에 있으므로, 스무딩 적용 시 결과는 90도에 가까워야 함
    EXPECT_NEAR(result.angle, 88.5, 0.1); // 0.7 * 90 + 0.3 * 85 = 88.5
}

// 여러 피크가 있는 히스토그램으로 VV 추정 테스트
TEST_F(VVEstimatorTest, EstimateVVMultiplePeaks) {
    std::vector<float> histogram(180, 0.0f);
    
    // 여러 위치에 피크 생성
    histogram[60] = 0.3f;
    histogram[90] = 0.5f;
    histogram[120] = 0.2f;
    
    vv::VVResult prevResult;
    prevResult.angle = 90.0;
    prevResult.updateAcceleration();
    
    vv::VVResult result = estimator->estimateVV(histogram, prevResult);
    
    // 가중 평균으로 계산된 결과와 가까워야 함
    double expectedAngle = (60 * 0.3 + 90 * 0.5 + 120 * 0.2) / (0.3 + 0.5 + 0.2);
    expectedAngle = 0.7 * expectedAngle + 0.3 * 90.0; // 스무딩 적용
    
    EXPECT_NEAR(result.angle, expectedAngle, 0.1);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
} 