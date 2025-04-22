cmake_minimum_required(VERSION 3.10)
project(
  VisualVerticalEstimator
  VERSION 0.0.1
  LANGUAGES CXX)

# Use C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set build type to Release if not specified
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE
      Release
      CACHE STRING "Build type" FORCE)
endif()

# Enable compile_commands.json for tooling support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find and include OpenCV
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# Include project header files
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Find YAML-CPP for configuration parsing
find_package(yaml-cpp REQUIRED)

# Add Abseil library
include(FetchContent)
FetchContent_Declare(
  absl
  GIT_REPOSITORY https://github.com/abseil/abseil-cpp.git
  GIT_TAG 20230125.3)
# Configure Abseil build options - build as a single library
set(ABSL_PROPAGATE_CXX_STD ON)
set(ABSL_ENABLE_INSTALL ON)
FetchContent_MakeAvailable(absl)

# Add subdirectories
add_subdirectory(src)

# Test build option (default: OFF)
option(BUILD_TESTS "Build tests" OFF)
if(BUILD_TESTS)
  # Enable testing and register with CTest
  enable_testing()
  add_subdirectory(tests)
endif()

# Set installation directory
set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/install)

# Install executable file
install(TARGETS vv_estimator DESTINATION bin)
