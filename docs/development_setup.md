# Development Environment Setup

This document describes how to set up your development environment to work with this project, including the necessary tools for code formatting and linting according to our project's Google C++ Style Guide.

## Required Tools

1. **CMake**: Build system generator
2. **C++17 Compatible Compiler**: gcc 7+, clang 5+, or MSVC 19.14+ (Visual Studio 2017)
3. **clang-format**: Code formatter
4. **clang-tidy**: Code linter/static analyzer

## Installation Instructions

### macOS

```bash
# Using Homebrew
brew install cmake
brew install llvm    # Includes clang, clang-format, and clang-tidy
```

### Linux (Ubuntu/Debian)

```bash
# Install build tools
sudo apt update
sudo apt install cmake build-essential

# Install clang tools
sudo apt install clang clang-format clang-tidy
```

### Windows

1. Install [Visual Studio](https://visualstudio.microsoft.com/) with C++ workload
2. Install [CMake](https://cmake.org/download/)
3. Install [LLVM](https://releases.llvm.org/download.html) which includes clang-format and clang-tidy
4. Add the LLVM bin directory to your PATH

## Project Setup

After cloning the repository, set up the git hooks to automate code formatting:

```bash
# Make the pre-commit hook executable (skip on Windows)
chmod +x .git/hooks/pre-commit
```

## Building the Project

```bash
# Create a build directory & Configure the project
cmake -S . -B build 

# Build
cmake --build build
```

## Code Formatting

The project uses Google's C++ style guide via clang-format. The configuration is stored in the `.clang-format` file at the root of the repository.

To manually format a file:

```bash
clang-format -i path/to/file.cc
```

To format all C++ files in the project:

```bash
find src include tests -name "*.cc" -o -name "*.h" | xargs clang-format -i
```

## Static Analysis with clang-tidy

The project uses clang-tidy for static analysis. The configuration is stored in the `.clang-tidy` file at the root of the repository.

To manually run clang-tidy on a file:

```bash
clang-tidy path/to/file.cc -- -std=c++17
```

For more comprehensive analysis, you can use [run-clang-tidy](https://clang.llvm.org/extra/clang-tidy/):

```bash
run-clang-tidy -p build
```

## Git Pre-commit Hook

A pre-commit hook is installed that automatically runs clang-format on changed files before each commit. If clang-format is not installed or not in your PATH, the commit will be blocked with an error message.

If you need to bypass this check for any reason, you can use:

```bash
git commit --no-verify
```

But please use this sparingly and make sure to follow the style guide manually.

## Abseil Library

This project uses Google's [Abseil library](https://abseil.io/). The library is included as a dependency in the project's CMake configuration.

## Configuration Files

All configurable parameters should be stored in YAML files under the `/config` directory:

- `config.yaml`: Main configuration file
- `test.yaml`: Configuration for tests

## Additional Resources

- [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)
- [Abseil C++ Tips](https://abseil.io/tips/)
- [Effective Modern C++](http://shop.oreilly.com/product/0636920033707.do) by Scott Meyers 
