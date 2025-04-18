# Visual Vertical Estimator

6-DoF SVC 모델을 위한 비주얼 버티컬(Visual Vertical) 추정 프로그램입니다.

## 개요

이 프로젝트는 비디오 또는 카메라 입력에서 시각적 수직(Visual Vertical)을 추정하는 C++ 라이브러리 및 애플리케이션입니다. HOG(Histogram of Oriented Gradients) 기반 알고리즘을 사용하여 화면 속 수직 방향을 추정하고, 이를 기반으로 중력 방향을 계산합니다.

## 의존성

- C++17 호환 컴파일러
- CMake 3.10 이상
- OpenCV 4.x

## 빌드 방법

```bash
# 프로젝트 클론
git clone <repository-url>
cd VisualVerticalEstimator

# 빌드 디렉토리 생성 및 이동
mkdir -p build
cd build

# CMake 구성 및 빌드
cmake ..
make -j4

# 설치(선택 사항)
make install
```

## 사용 방법

### 비디오 파일 처리
```bash
./vv_estimator -i /path/to/video.mp4 -s 2
```

### 카메라에서 직접 처리
```bash
./vv_estimator -c true -cp 0 -s 1
```

### 명령줄 옵션
- `-i`, `--inputfile`: 입력 비디오 파일 경로
- `-c`, `--camera`: 카메라 사용 여부 (true/false)
- `-cp`, `--camera_port`: 카메라 포트 번호 (기본값: 0)
- `-s`, `--scale`: 이미지 크기 조정 비율 (기본값: 2)
- `-h`, `--help`: 도움말 표시

## 결과

프로그램 실행 결과로 다음 파일들이 생성됩니다:
- `VV_*.csv`: 추정된 Visual Vertical 각도 및 계산된 가속도 값 (CSV 형식)
- `VV_Video_*.mp4`: 처리 과정과 결과가 시각화된 비디오

## 라이센스

본 프로젝트는 [라이센스 정보]에 따라 배포됩니다. 