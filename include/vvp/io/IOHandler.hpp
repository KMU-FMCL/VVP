#ifndef VVP_IO_IOHANDLER_H_
#define VVP_IO_IOHANDLER_H_

#include <string>
#include <vector>

#include <filesystem>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>

#include "vvp/estimation/Types.hpp"

namespace vv {

/**
 * @brief 입출력 처리 클래스
 *
 * 비디오 파일/카메라 읽기, 결과 저장, 화면 출력 등을 담당하는 클래스입니다.
 */
class IOHandler {
 public:
  /**
   * @brief 생성자
   * @param config 프로그램 설정
   */
  explicit IOHandler(const Config& config);

  /**
   * @brief 소멸자
   */
  ~IOHandler();

  /**
   * @brief 비디오 입력 준비
   * @return 성공 여부
   */
  bool open_video_source();

  /**
   * @brief 다음 프레임 읽기
   * @param[out] frame 읽은 프레임이 저장될 Mat
   * @return 프레임을 성공적으로 읽었는지 여부
   */
  bool read_next_frame(cv::Mat& frame);

  /**
   * @brief 결과 비디오 파일 준비
   * @param width 비디오 너비
   * @param height 비디오 높이
   * @return 성공 여부
   */
  bool setup_video_writer(int width, int height);

  /**
   * @brief 프레임을 결과 비디오에 쓰기
   * @param frame
   */
  void write_frame(const cv::Mat& frame);

  /**
   * @brief 처리 결과 표시
   * @param frame 표시할 프레임
   * @param waitKey 키 입력 대기 시간 (ms)
   * @return 입력된 키 (ESC: 27)
   */
  int display_frame(const cv::Mat& frame, int waitKey = 1);

  /**
   * @brief 처리 결과 CSV 파일로 저장
   * @param results VV 결과 벡터
   * @return 성공 여부
   */
  bool save_results_to_csv(const std::vector<VVResult>& results);

  /**
   * @brief VideoCapture 객체 얻기
   * @return VideoCapture 참조
   */
  cv::VideoCapture& get_video_capture();

  /**
   * @brief 결과 파일 경로 생성
   * @param prefix 파일 접두사
   * @param extension 파일 확장자
   * @return 생성된 파일 경로
   */
  std::string generate_output_file_path(const std::string& prefix,
                                        const std::string& extension) const;

 private:
  Config config_;
  cv::VideoCapture video_capture_;
  cv::VideoWriter video_writer_;
  std::string csv_file_path_;
  std::string video_file_path_;

  /**
   * @brief 현재 시간을 기반으로 타임스탬프 문자열 생성
   * @return 타임스탬프 문자열
   */
  std::string get_current_time_stamp() const;

  /**
   * @brief 타임스탬프에서 시간 부분(HHMMSS)만 추출
   * @param timestamp 전체 타임스탬프 문자열 (YYYYMMDD_HHMMSS)
   * @return 시간 부분 문자열 (HHMMSS), 추출 실패 시 "000000"
   */
  static std::string extract_time_part(const std::string& timestamp);

  /**
   * @brief 디렉토리가 존재하지 않으면 생성
   * @param dir 생성할 디렉토리 경로
   */
  static void ensure_directory_exists(const std::filesystem::path& dir);
};

}  // namespace vv

#endif  // VVP_IO_IOHANDLER_H_
