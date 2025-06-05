# Makefile for Visual Vertical Estimation project
# Basic settings
BUILD_DIR := build
CMAKE_FLAGS := -D CMAKE_BUILD_TYPE=RelWithDebInfo -D CMAKE_EXPORT_COMPILE_COMMANDS=ON -D BUILD_TESTS=OFF

# .PHONY declaration - prevents conflicts with directory/file names
.PHONY: all build clean test lint format help rebuild build-no-checks

# Default target
all: build

# Display help
help:
	@echo "Available commands:"
	@echo "  make            - Build project with clang-tidy and format checks (same as build)"
	@echo "  make build      - Build project with clang-tidy and format checks"
	@echo "  make build-no-checks - Build project without code checks"
	@echo "  make clean      - Clean build directory"
	@echo "  make test       - Run tests"
	@echo "  make lint       - Run clang-tidy checks separately"
	@echo "  make format     - Format code with clang-format separately"
	@echo "  make rebuild    - Full rebuild (clean + build)"
	@echo "  make help       - Display this help message"

# Create build directory and build project (with checks)
build:
	@mkdir -p $(BUILD_DIR)
	@echo "Configuring with CMake..."
	@cmake -S . -B $(BUILD_DIR) $(CMAKE_FLAGS)
	@echo "Building the project with clang-tidy and format checks..."
	@cmake --build $(BUILD_DIR)
	@echo "Build completed successfully!"

# Build without code checks
build-no-checks:
	@mkdir -p $(BUILD_DIR)
	@echo "Configuring with CMake (without code checks)..."
	@cmake -S . -B $(BUILD_DIR) $(CMAKE_FLAGS) -D ENABLE_CLANG_TIDY=OFF -D ENABLE_CLANG_FORMAT=OFF
	@echo "Building the project without code checks..."
	@cmake --build $(BUILD_DIR)
	@echo "Build completed successfully!"

# Clean build directory
clean:
	@echo "Cleaning build directory..."
	@rm -rf $(BUILD_DIR)
	@echo "Clean completed!"

# Run tests
test: build
	@echo "Running tests..."
	@cd $(BUILD_DIR) && ctest --output-on-failure
	@echo "All tests completed!"

# Code inspection - using clang-tidy (separate from build)
lint:
	@echo "Running clang-tidy..."
	@find src include -name "*.cpp" -o -name "*.h" -o -name "*.hpp" | xargs clang-tidy -p $(BUILD_DIR)
	@echo "Code inspection completed!"

# Format code - using clang-format (separate from build)
format:
	@echo "Formatting code with clang-format..."
	@find src include -name "*.cpp" -o -name "*.h" -o -name "*.hpp" | xargs clang-format -i
	@echo "Code formatting completed!"

# Full rebuild
rebuild: clean build
 