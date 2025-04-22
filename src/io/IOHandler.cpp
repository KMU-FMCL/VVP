#include "vvp/io/IOHandler.hpp"

#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

#include <filesystem>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

#include "vvp/utils/Helpers.hpp"  // 추가 (get_current_date_string 사용 위해)

namespace vv {

IOHandler::IOHandler(const Config& config) : config_(config) {
  // Resolve relative input path against project root
  if (!config_.use_camera) {
    std::filesystem::path in_path(config_.input_file_path);
    if (!in_path.is_absolute()) {
      in_path = std::filesystem::path(PROJECT_ROOT) / in_path;
      config_.input_file_path = in_path.string();
    }
  }
  // 현재 날짜 및 전체 타임스탬프 가져오기
  std::string current_date = utils::get_current_date_string();  // "YYYYMMDD"
  std::string current_timestamp =
      get_current_time_stamp();  // "YYYYMMDD_HHMMSS"
  // 타임스탬프에서 시간 부분(HHMMSS)만 추출
  std::string time_part = extract_time_part(current_timestamp);

  // 날짜 기반 결과 디렉토리 경로 설정 (project root/results)
  std::filesystem::path base_result_dir =
      std::filesystem::path(PROJECT_ROOT) / "results";
  std::filesystem::path date_result_dir =
      base_result_dir / current_date;  // 예: ../results/20230417

  // 날짜 기반 결과 하위 디렉토리 생성 (존재하지 않을 경우)
  // 날짜 기반 결과 디렉토리 생성
  ensure_directory_exists(date_result_dir);
  std::cout << "Results will be saved to directory: "
            << std::filesystem::absolute(date_result_dir).string() << std::endl;

  // 비디오, CSV 파일 경로 초기화 (파일 이름에 시간 포함)
  if (config_.use_camera) {
    // 카메라 입력의 경우, 파일 이름에 'camera'와 시간만 포함
    std::string csv_file_name = "camera_" + time_part + ".csv";
    std::string video_file_name = "camera_" + time_part + ".mp4";
    csv_file_path_ = (date_result_dir / csv_file_name).string();
    video_file_path_ = (date_result_dir / video_file_name).string();
  } else {
    // 파일 경로 처리에 std::filesystem 사용
    std::filesystem::path input_path(config_.input_file_path);

    // stem()은 확장자를 제외한 파일 이름만 가져옵니다
    std::string filename_without_ext = input_path.stem().string();

    // 최종 파일 이름 구성: VV_ + 원본이름 + _ + 시간 + 확장자
    std::string csv_file_name =
        "VV_" + filename_without_ext + "_" + time_part + ".csv";
    std::string video_file_name =
        "VV_Video_" + filename_without_ext + "_" + time_part + ".mp4";
    csv_file_path_ = (date_result_dir / csv_file_name).string();
    video_file_path_ = (date_result_dir / video_file_name).string();
  }
}

IOHandler::~IOHandler() {
  // 리소스 정리
  if (video_capture_.isOpened()) {
    video_capture_.release();
  }

  if (video_writer_.isOpened()) {
    video_writer_.release();
  }

  cv::destroyAllWindows();
}

bool IOHandler::open_video_source() {
  if (config_.use_camera) {
    video_capture_.open(config_.camera_port, cv::CAP_DSHOW);
  } else {
    video_capture_.open(config_.input_file_path);
  }

  if (!video_capture_.isOpened()) {
    std::cerr << "Error: Could not open video source: "
              << (config_.use_camera
                      ? "Camera #" + std::to_string(config_.camera_port)
                      : config_.input_file_path)
              << std::endl;
    return false;
  }

  return true;
}

bool IOHandler::read_next_frame(cv::Mat& frame) {
  if (!video_capture_.isOpened()) {
    return false;
  }

  return video_capture_.read(frame);
}

bool IOHandler::setup_video_writer(int width, int height) {
  if (!video_capture_.isOpened()) {
    return false;
  }

  // 비디오 코덱 및 프레임 레이트 설정
  int fourcc = cv::VideoWriter::fourcc('m', 'p', '4', 'v');
  double fps = video_capture_.get(cv::CAP_PROP_FPS);

  if (fps <= 0) {
    fps = 30.0;  // 기본값 설정
  }

  // VideoWriter 열기 전에 디렉토리 존재 여부 재확인
  // 비디오 저장 디렉토리 생성
  std::filesystem::path video_path(video_file_path_);
  ensure_directory_exists(video_path.parent_path());

  video_writer_.open(video_file_path_, fourcc, fps, cv::Size(width, height));

  if (!video_writer_.isOpened()) {
    std::cerr << "Error: Could not create video writer for: "
              << video_file_path_ << std::endl;
    return false;
  }

  std::cout << "Video will be saved to: " << video_file_path_ << std::endl;
  return true;
}

void IOHandler::write_frame(const cv::Mat& frame) {
  if (video_writer_.isOpened()) {
    video_writer_.write(frame);
  }
}

int IOHandler::display_frame(const cv::Mat& frame, int wait_key) {
  cv::imshow("Visual Vertical Estimation", frame);
  return cv::waitKey(wait_key);
}

bool IOHandler::save_results_to_csv(const std::vector<VVResult>& results) {
  if (results.empty()) {
    std::cerr << "Error: No results to save." << std::endl;
    return false;
  }

  // CSV 파일 열기 전에 디렉토리 존재 여부 재확인
  // CSV 저장 디렉토리 생성
  std::filesystem::path csv_path(csv_file_path_);
  ensure_directory_exists(csv_path.parent_path());

  std::ofstream out_file(csv_file_path_);
  if (!out_file.is_open()) {
    std::cerr << "Error: Could not open file for writing: " << csv_file_path_
              << std::endl;
    return false;
  }

  // CSV 헤더 작성
  out_file << "VV_acc_x[m/s^2],VV_acc_y[m/s^2],VV_acc_rad,VV_acc_dig"
           << std::endl;

  // 데이터 작성
  for (const auto& result : results) {
    out_file << result.acc_x << "," << result.acc_y << "," << result.angle_rad
             << "," << result.angle << std::endl;
  }

  out_file.close();
  std::cout << "Results saved to: " << csv_file_path_ << std::endl;

  return true;
}

cv::VideoCapture& IOHandler::get_video_capture() {
  return video_capture_;
}

std::string IOHandler::generate_output_file_path(
    const std::string& prefix, const std::string& extension) const {
  // 현재 날짜 기준 디렉토리와 시간을 포함한 파일 이름 생성
  std::string current_date = utils::get_current_date_string();
  std::filesystem::path base_result_dir = "../results";
  std::filesystem::path date_result_dir =
      base_result_dir / current_date;  // base_result_dir 객체에 / 연산자 사용
  std::string timestamp = get_current_time_stamp();

  // 타임스탬프에서 시간 부분 추출
  std::string time_part = "000000";  // 기본값
  size_t underscore_pos = timestamp.find('_');
  if (underscore_pos != std::string::npos &&
      underscore_pos + 1 < timestamp.length()) {
    time_part = timestamp.substr(underscore_pos + 1);  // "HHMMSS" 추출
  }

  return (date_result_dir / (prefix + "_" + time_part + extension)).string();
}

std::string IOHandler::get_current_time_stamp() const {
  auto now = std::chrono::system_clock::now();
  std::time_t time = std::chrono::system_clock::to_time_t(now);
  std::stringstream ss;
  ss << std::put_time(std::localtime(&time), kIsoTimeFormat.c_str());
  return ss.str();
}

}  // namespace vv

//------------------------------------------------------------------------------
// Helper implementations
//------------------------------------------------------------------------------
namespace vv {

std::string IOHandler::extract_time_part(const std::string& timestamp) {
  size_t pos = timestamp.find('_');
  if (pos != std::string::npos && pos + 1 < timestamp.size()) {
    return timestamp.substr(pos + 1);
  }
  return "000000";
}

void IOHandler::ensure_directory_exists(const std::filesystem::path& dir) {
  try {
    std::filesystem::create_directories(dir);
  } catch (const std::exception& e) {
    std::cerr << "Warning: Could not create directory: " << dir << " ("
              << e.what() << ")" << std::endl;
  }
}
}  // namespace vv
