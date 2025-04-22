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
  std::string configPath = std::string(PROJECT_ROOT) + "/config/config.yaml";
  if (argc >= 2) {
    configPath = argv[1];
  }
  std::cout << "Loading config from: " << configPath << std::endl;
  vv::utils::ConfigAll cfgAll;
  try {
    cfgAll = vv::utils::ConfigLoader::load(configPath);
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return -1;
  }
  const vv::Config& config = cfgAll.io;

  // 입출력 핸들러 초기화
  vv::IOHandler ioHandler(config);
  if (!ioHandler.open_video_source()) {
    std::cerr << "Error: Could not open video source." << std::endl;
    return 1;
  }

  // 이미지 처리기 및 VV 추정기 초기화 (YAML 파라미터 사용)
  vv::ImageProcessor imageProcessor(cfgAll.hog);
  vv::VVEstimator vvEstimator(cfgAll.vv);

  // FPS 카운터 초기화
  vv::FPSCounter fpsCounter;

  // 첫 프레임 읽기 및 비디오 출력 설정
  cv::Mat frame;
  if (!ioHandler.read_next_frame(frame)) {
    std::cerr << "Error: Could not read first frame." << std::endl;
    return 1;
  }

  // 이미지 크기 조정
  frame = imageProcessor.resize_image(frame, config.scale);

  // 비디오 작성기 설정
  int originalWidth = frame.cols;
  int originalHeight = frame.rows;

  // 결과 비디오 크기 계산 (원본 이미지 * 2 + 히스토그램)
  int resultWidth = originalWidth * 2;
  int resultHeight = static_cast<int>(originalHeight * 2.6);

  if (!ioHandler.setup_video_writer(resultWidth, resultHeight)) {
    std::cerr << "Warning: Could not setup video writer." << std::endl;
  }

  // 이전 VV 결과 초기화
  vv::VVResult previousResult;

  // 메인 처리 루프
  while (true) {
    // FPS 측정 시작
    fpsCounter.tick_start();

    // 프레임 읽기
    if (!ioHandler.read_next_frame(frame)) {
      break;
    }

    // 이미지 크기 조정
    frame = imageProcessor.resize_image(frame, config.scale);

    // HOG 계산
    vv::HOGResult hogResult = imageProcessor.compute_hog(frame);

    // VV 추정
    vv::VVResult vvResult =
        vvEstimator.estimate_vv(hogResult.histogram, previousResult);
    previousResult = vvResult;

    // 이미지 회전 (보정)
    cv::Mat calibratedImage =
        imageProcessor.rotate_image(frame, 90 - vvResult.angle);

    // 히스토그램 시각화 생성
    cv::Mat histogramImage = vvEstimator.create_histogram_visualization(
        hogResult.histogram, vvResult, resultWidth, originalHeight * 0.6);

    // 시각화 이미지 생성
    cv::Mat visualizationResult = imageProcessor.create_visualization(
        frame, calibratedImage, hogResult, vvResult, histogramImage,
        fpsCounter.get_fps()  // FPS 정보 전달
    );

    // 결과 표시 및 저장
    int key = ioHandler.display_frame(visualizationResult);
    ioHandler.write_frame(visualizationResult);

    // FPS 측정 종료
    fpsCounter.tick_end();

    // ESC 키가 눌리면 종료
    if (key == 27) {
      break;
    }
  }

  // 결과 CSV 저장
  if (config.saveResults) {
    ioHandler.save_results_to_csv(vvEstimator.get_all_results());
  }

  // 평균 FPS 출력
  std::cout << "Average FPS: " << fpsCounter.get_average_fps() << std::endl;
  std::cout << "Total frames processed: " << fpsCounter.get_frame_count()
            << std::endl;
  std::cout << "Total processing time: "
            << fpsCounter.get_total_processing_time_sec() << " seconds"
            << std::endl;

  std::cout << "Processing complete." << std::endl;
  return 0;
}