#include "vvp/app/runner.h"
#include <iostream>

namespace vv {

// Runner::Impl 클래스 정의
class Runner::Impl {
 public:
  Impl(int argc, char* argv[]) : argc_(argc), argv_(argv) {}

  auto Run() const -> int {
    // 현재는 기본 구현만 (추후 main.cc에서 로직 이전 예정)
    std::cout << "Runner::Impl::Run() called with " << argc_ << " arguments\n";
    return 0;
  }

 private:
  int argc_;
  char** argv_;
};

// Runner 클래스 구현
Runner::Runner(int argc, char* argv[])
    : pimpl_(std::make_unique<Impl>(argc, argv)) {}

Runner::~Runner() = default;

auto Runner::Run() const -> int { return pimpl_->Run(); }

}  // namespace vv
