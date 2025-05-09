    #pragma once

    #include <chrono> // 시간 관련 헤더

    namespace vv {

    /**
     * @brief FPS 측정을 관리하는 클래스
     */
    class FPSCounter {
    public:
        /**
         * @brief 생성자
         */
        FPSCounter();

        /**
         * @brief 프레임 처리 시작 시 호출
         */
        void tickStart();

        /**
         * @brief 프레임 처리 종료 시 호출하여 FPS 계산
         */
        void tickEnd();

        /**
         * @brief 가장 최근에 계산된 FPS 반환
         * @return 현재 프레임의 FPS
         */
        double getFPS() const;

        /**
         * @brief 전체 실행 시간 동안의 평균 FPS 계산 및 반환
         * @return 평균 FPS
         */
        double getAverageFPS() const;

        /**
         * @brief 처리된 총 프레임 수 반환
         * @return 총 프레임 수
         */
        long long getFrameCount() const;
        
        /**
         * @brief 총 처리 시간(초) 반환
         * @return 총 처리 시간
         */
        double getTotalProcessingTimeSec() const;


    private:
        std::chrono::high_resolution_clock::time_point m_frameStartTime;
        std::chrono::high_resolution_clock::time_point m_overallStartTime;
        long long m_frameCount;
        double m_totalProcessingTimeSec;
        double m_currentFPS;
    };

    } // namespace vv