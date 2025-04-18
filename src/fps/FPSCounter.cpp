    #include "visual_vertical/fps/FPSCounter.hpp"
    #include <iostream> // 디버깅/정보 출력용 (선택 사항)

    namespace vv {

    FPSCounter::FPSCounter()
        : m_frameCount(0),
          m_totalProcessingTimeSec(0.0),
          m_currentFPS(0.0) {
        m_overallStartTime = std::chrono::high_resolution_clock::now();
        m_frameStartTime = m_overallStartTime; // 초기화
    }

    void FPSCounter::tickStart() {
        m_frameStartTime = std::chrono::high_resolution_clock::now();
    }

    void FPSCounter::tickEnd() {
        auto frameEndTime = std::chrono::high_resolution_clock::now();
        auto frameDuration = std::chrono::duration_cast<std::chrono::microseconds>(frameEndTime - m_frameStartTime);
        double frameTimeSec = frameDuration.count() / 1'000'000.0;

        if (frameTimeSec > 1e-9) { // 0으로 나누는 것 방지
            m_currentFPS = 1.0 / frameTimeSec;
        } else {
            m_currentFPS = 0.0; // 또는 매우 큰 값으로 설정
        }

        m_totalProcessingTimeSec += frameTimeSec;
        m_frameCount++;
    }

    double FPSCounter::getFPS() const {
        return m_currentFPS;
    }

    double FPSCounter::getAverageFPS() const {
        if (m_frameCount > 0 && m_totalProcessingTimeSec > 1e-9) {
            return m_frameCount / m_totalProcessingTimeSec;
        }
        return 0.0;
    }
    
    long long FPSCounter::getFrameCount() const {
        return m_frameCount;
    }
    
    double FPSCounter::getTotalProcessingTimeSec() const {
        return m_totalProcessingTimeSec;
    }


    } // namespace vv