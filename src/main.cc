#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_format.h"
#include "absl/strings/string_view.h"
#include "absl/types/span.h"
#include "vvp/estimation/vv_estimator.h"
#include "vvp/estimation/vv_histogram_visualizer.h"  // Added for VVHistogramVisualizer
#include "vvp/fps/fps_counter.h"
#include "vvp/io/input_handler.h"      // Changed from io_handler.h
#include "vvp/io/output_handler.h"     // Added
#include "vvp/processing/image_processor.h"
#include "vvp/utils/display_utils.h"   // Added
#include "vvp/utils/config_loader.h"  // YAML config loader
#include "vvp/utils/helpers.h"        // print_opencv_info
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

// 결과 비디오 높이 계산을 위한 상수
constexpr double kResultHeightFactor =
    2.6;  // 원본 높이의 2.6배 (히스토그램 공간 포함)
// 수직 기준 각도 (도)
constexpr int kVerticalAngleDegrees = 90;  // 이미지 회전 기준점(수직 방향)
// 히스토그램 높이 비율 (원본 이미지 높이 대비)
constexpr double kHistogramHeightFactor = 0.6;  // 원본 높이의 60%
// 키보드 키 코드
constexpr int kEscKeyCode = 27;  // ESC 키의 ASCII 코드

namespace {
auto print_help() -> void {
  std::cout << "Visual Vertical Estimator - Help\n"
            << "================================\n"
            << "Usage: vv_estimator [OPTIONS] [CONFIG_PATH]\n\n"
            << "Options:\n"
            << "  --help    Display this help message and exit\n\n"
            << "Arguments:\n"
            << "  CONFIG_PATH    Path to YAML configuration file (default: "
               "./config/config.yaml)\n"
            << '\n';
}
}  // namespace

auto main(int argc, char* argv[]) -> int {
  // 명령줄 인자를 안전하게 접근하기 위해 absl::Span 사용
  absl::Span<char*> args(argv, argc);

  // 도움말 옵션 확인
  for (int i = 1; i < argc; i++) {
    absl::string_view arg(args[i]);
    if (arg == "--help" || arg == "-h") {
      print_help();
      return 0;
    }
  }

  // OpenCV 정보 출력
  vv::utils::print_opencv_info();

  // 설정 파일 경로(기본값: 프로젝트 루트의 config/config.yaml)
  std::string config_path = absl::StrCat(PROJECT_ROOT, "/config/config.yaml");
  if (argc >= 2 && args[1][0] != '-') {
    config_path = args[1];
  }
  std::cout << "Loading config from: " << config_path << '\n';

  absl::StatusOr<vv::utils::ConfigAll> cfg_result =
      vv::utils::ConfigLoader::load(config_path);
  if (!cfg_result.ok()) {
    std::cerr << cfg_result.status().message() << '\n';
    return -1;
  }
  vv::utils::ConfigAll const& cfg_all = *cfg_result;
  vv::Config const& config = cfg_all.io;

  // 입출력 핸들러 초기화
  vv::io::InputHandler input_handler(config); // Changed
  vv::io::OutputHandler output_handler(config); // Added
  absl::Status status = input_handler.open_video_source(); // Changed
  if (!status.ok()) {
    std::cerr << "Error: " << status.message() << '\n';
    return 1;
  }

  // 이미지 처리기 및 VV 추정기 초기화 (YAML 파라미터 사용)
  vv::ImageProcessor image_processor(cfg_all.hog);
  vv::VVEstimator vv_estimator(cfg_all.vv);
  vv::VVHistogramVisualizer vv_visualizer(
      cfg_all.vv);  // Added VVHistogramVisualizer instance

  // FPS 카운터 초기화
  vv::FPSCounter fps_counter;

  // 첫 프레임 읽기 및 비디오 출력 설정
  cv::Mat frame;
  status = input_handler.read_next_frame(frame); // Changed
  if (!status.ok()) {
    std::cerr << "Error: " << status.message() << '\n';
    return 1;
  }

  // 이미지 크기 조정
  frame = vv::ImageProcessor::resize_image(frame, config.scale);

  // 비디오 작성기 설정
  int original_width = frame.cols;
  int original_height = frame.rows;

  // 결과 비디오 크기 계산 (원본 이미지 * 2 + 히스토그램)
  int result_width = original_width * 2;
  int result_height = static_cast<int>(original_height * kResultHeightFactor);

  // FPS 가져오기 (InputHandler에서)
  double fps_for_output = input_handler.get_fps();
  if (fps_for_output <= 0) {
      fps_for_output = 30.0; // 기본 FPS 설정 (예: 30)
      std::cout << "Warning: Video source FPS not available or invalid. Using default FPS for output: "
                << fps_for_output << std::endl;
  }
  status = output_handler.setup_video_writer(result_width, result_height, fps_for_output); // Changed
  if (!status.ok()) {
    std::cerr << "Warning: " << status.message() << '\n';
  }

  // 이전 VV 결과 초기화
  vv::VVResult previous_result;

  // 메인 처리 루프
  while (true) {
    // FPS 측정 시작
    fps_counter.tick_start();

    // 프레임 읽기
    status = input_handler.read_next_frame(frame); // Changed
    if (!status.ok()) {
      break;
    }

    // 이미지 크기 조정
    frame = vv::ImageProcessor::resize_image(frame, config.scale);

    // HOG 계산
    vv::HOGResult hog_result = image_processor.compute_hog(frame);

    // VV 추정 (직전 프레임의 FPS 사용)
    // 주의: current_fps는 루프 후반에 계산되므로, 이 시점에서는 이전 루프의
    // 값이 사용됨 또는 첫 프레임에서는 기본값(0.0)이 사용됨
    double fps_to_record =
        fps_counter.get_fps();  // CSV 기록 및 VV 추정 전달용 FPS
    vv::VVResult vv_result = vv_estimator.estimate_vv(
        hog_result.histogram, previous_result, fps_to_record);
    previous_result = vv_result;  // 이전 결과 업데이트는 여기 있어야 함

    // 이미지 회전 (보정)
    cv::Mat calibrated_image = vv::ImageProcessor::rotate_image(
        frame, kVerticalAngleDegrees - vv_result.angle);

    // 히스토그램 시각화 생성
    cv::Mat histogram_image = vv_visualizer.create_histogram_visualization(
        hog_result.histogram, vv_result, result_width,
        static_cast<int>(original_height * kHistogramHeightFactor));

    // 시각화 이미지 생성 (여기도 같은 FPS 사용)
    cv::Mat visualization_result = vv::ImageProcessor::create_visualization(
        frame, calibrated_image, hog_result, vv_result, histogram_image,
        static_cast<float>(fps_to_record)  // 시각화에도 기록용 FPS 사용
    );

    // 결과 표시 및 저장
    int key = vv::utils::DisplayUtils::display_frame(visualization_result); // Changed
    output_handler.write_frame(visualization_result); // Changed

    // FPS 측정 종료 (전체 루프 처리 시간 측정)
    fps_counter.tick_end();

    // ESC 키가 눌리면 종료
    if (key == kEscKeyCode) {
      break;
    }
  }

  // 결과 CSV 저장
  if (config.save_csv_results) { // Changed from config.save_results
    status = output_handler.save_results_to_csv(vv_estimator.get_all_results()); // Changed
    if (!status.ok()) {
      std::cerr << "Error saving results: " << status.message() << '\n';
    }
  }

  // 평균 FPS 출력
  std::cout << "Average FPS: " << fps_counter.get_average_fps() << '\n';
  std::cout << "Total frames processed: " << fps_counter.get_frame_count()
            << '\n';
  std::cout << "Total processing time: "
            << fps_counter.get_total_processing_time_sec() << " seconds"
            << '\n';

  std::cout << "Processing complete." << '\n';
  return 0;
}
