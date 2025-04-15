#pragma once

/**
 * @file vve.h
 * @brief Visual Vertical Estimator 라이브러리의 주요 헤더 파일
 * 
 * 이 헤더 파일은 Visual Vertical Estimator 라이브러리의 모든 주요 컴포넌트를
 * 포함합니다. 이 헤더 하나만 포함하면 라이브러리의 모든 기능을 사용할 수 있습니다.
 */

// 상수 정의
#include "VisualVerticalEstimator/constants.h"

// 유틸리티 함수
#include "VisualVerticalEstimator/utils.h"

// HOG 계산기
#include "VisualVerticalEstimator/hog_calculator.h"

// 방향성 분석
#include "VisualVerticalEstimator/orientation.h"

// 시각화
#include "VisualVerticalEstimator/visualization.h"

// 비디오 처리
#include "VisualVerticalEstimator/video_processor.h"

/**
 * @namespace vve
 * @brief Visual Vertical Estimator 라이브러리의 모든 클래스와 함수를 포함하는 네임스페이스
 */
namespace vve {

/**
 * @brief 라이브러리 버전 정보 구조체
 */
struct Version {
    static constexpr int major = 1;
    static constexpr int minor = 0;
    static constexpr int patch = 0;
    
    /**
     * @brief 버전 문자열 반환
     * @return 버전 문자열 (예: "1.0.0")
     */
    static const char* toString() {
        return "1.0.0";
    }
};

/**
 * @brief 라이브러리 초기화 함수
 * @param enableOpenCL OpenCL 사용 여부
 * @return 초기화 성공 여부
 */
bool initialize(bool enableOpenCL = true);

} // namespace vve