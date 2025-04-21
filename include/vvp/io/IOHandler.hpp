#pragma once

#include <string>
#include <vector>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
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
    bool openVideoSource();

    /**
     * @brief 다음 프레임 읽기
     * @param[out] frame 읽은 프레임이 저장될 Mat
     * @return 프레임을 성공적으로 읽었는지 여부
     */
    bool readNextFrame(cv::Mat& frame);

    /**
     * @brief 결과 비디오 파일 준비
     * @param width 비디오 너비
     * @param height 비디오 높이
     * @return 성공 여부
     */
    bool setupVideoWriter(int width, int height);

    /**
     * @brief 프레임을 결과 비디오에 쓰기
     * @param frame 저장할 프레임
     */
    void writeFrame(const cv::Mat& frame);

    /**
     * @brief 처리 결과 표시
     * @param frame 표시할 프레임
     * @param waitKey 키 입력 대기 시간 (ms)
     * @return 입력된 키 (ESC: 27)
     */
    int displayFrame(const cv::Mat& frame, int waitKey = 1);

    /**
     * @brief 처리 결과 CSV 파일로 저장
     * @param results VV 결과 벡터
     * @return 성공 여부
     */
    bool saveResultsToCSV(const std::vector<VVResult>& results);

    /**
     * @brief VideoCapture 객체 얻기
     * @return VideoCapture 참조
     */
    cv::VideoCapture& getVideoCapture();

    /**
     * @brief 결과 파일 경로 생성
     * @param prefix 파일 접두사
     * @param extension 파일 확장자
     * @return 생성된 파일 경로
     */
    std::string generateOutputFilePath(const std::string& prefix, const std::string& extension) const;

private:
    Config m_config;
    cv::VideoCapture m_videoCapture;
    cv::VideoWriter m_videoWriter;
    std::string m_csvFilePath;
    std::string m_videoFilePath;
    
    /**
     * @brief 현재 시간을 기반으로 타임스탬프 문자열 생성
     * @return 타임스탬프 문자열
     */
    std::string getCurrentTimeStamp() const;
};

} // namespace vv 