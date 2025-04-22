#include "absl/strings/str_cat.h"

#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "vvp/estimation/VVEstimator.hpp"
#include "vvp/fps/FPSCounter.hpp"
#include "vvp/io/IOHandler.hpp"
#include "vvp/processing/ImageProcessor.hpp"
#include "vvp/utils/ConfigLoader.hpp"  // YAML config loader
#include "vvp/utils/Helpers.hpp"       // print_opencv_info

int main(int argc, char* argv[]) {
  // OpenCV 정보 출력
  vv::utils::print_opencv_info();

  // 설정 파일 경로(기본값: 프로젝트 루트의 config/config.yaml)
  std::string config_path = absl::StrCat(PROJECT_ROOT, "/config/config.yaml");
  if (argc >= 2) {
    config_path = argv[1];
  }
  std::cout << "Loading config from: " << config_path << std::endl;
  vv::utils::ConfigAll cfg_all;
  try {
    cfg_all = vv::utils::ConfigLoader::load(config_path);
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return -1;
  }
  const vv::Config& config = cfg_all.io;

  // 입출력 핸들러 초기화
  vv::IOHandler io_handler(config);
  if (!io_handler.open_video_source()) {
    std::cerr << "Error: Could not open video source." << std::endl;
    return 1;
  }

  // 이미지 처리기 및 VV 추정기 초기화 (YAML 파라미터 사용)
  vv::ImageProcessor image_processor(cfg_all.hog);
  vv::VVEstimator vv_estimator(cfg_all.vv);

  // FPS 카운터 초기화
  vv::FPSCounter fps_counter;

  // 첫 프레임 읽기 및 비디오 출력 설정
  cv::Mat frame;
  if (!io_handler.read_next_frame(frame)) {
    std::cerr << "Error: Could not read first frame." << std::endl;
    return 1;
  }

  // 이미지 크기 조정
  frame = image_processor.resize_image(frame, config.scale);

  // 비디오 작성기 설정
  int original_width = frame.cols;
  int original_height = frame.rows;

  // 결과 비디오 크기 계산 (원본 이미지 * 2 + 히스토그램)
  int result_width = original_width * 2;
  int result_height = static_cast<int>(original_height * 2.6);

  if (!io_handler.setup_video_writer(result_width, result_height)) {
    std::cerr << "Warning: Could not setup video writer." << std::endl;
  }

  // 이전 VV 결과 초기화
  vv::VVResult previous_result;

  // 메인 처리 루프
  while (true) {
    // FPS 측정 시작
    fps_counter.tick_start();

    // 프레임 읽기
    if (!io_handler.read_next_frame(frame)) {
      break;
    }

    // 이미지 크기 조정
    frame = image_processor.resize_image(frame, config.scale);

    // HOG 계산
    vv::HOGResult hog_result = image_processor.compute_hog(frame);

    // VV 추정
    vv::VVResult vv_result =
        vv_estimator.estimate_vv(hog_result.histogram, previous_result);
    previous_result = vv_result;

    // 이미지 회전 (보정)
    cv::Mat calibrated_image =
        image_processor.rotate_image(frame, 90 - vv_result.angle);

    // 히스토그램 시각화 생성
    cv::Mat histogram_image = vv_estimator.create_histogram_visualization(
        hog_result.histogram, vv_result, result_width, original_height * 0.6);

    // 시각화 이미지 생성
    cv::Mat visualization_result = image_processor.create_visualization(
        frame, calibrated_image, hog_result, vv_result, histogram_image,
        fps_counter.get_fps()  // FPS 정보 전달
    );

    // 결과 표시 및 저장
    int key = io_handler.display_frame(visualization_result);
    io_handler.write_frame(visualization_result);

    // FPS 측정 종료
    fps_counter.tick_end();

    // ESC 키가 눌리면 종료
    if (key == 27) {
      break;
    }
  }

  // 결과 CSV 저장
  if (config.save_results) {
    io_handler.save_results_to_csv(vv_estimator.get_all_results());
  }

  // 평균 FPS 출력
  std::cout << "Average FPS: " << fps_counter.get_average_fps() << std::endl;
  std::cout << "Total frames processed: " << fps_counter.get_frame_count()
            << std::endl;
  std::cout << "Total processing time: "
            << fps_counter.get_total_processing_time_sec() << " seconds"
            << std::endl;

  std::cout << "Processing complete." << std::endl;
  return 0;
}
