#ifndef VVP_APP_RUNNER_H_
#define VVP_APP_RUNNER_H_

#include <memory>  // For std::unique_ptr

namespace vv {

class Runner {
 public:
  Runner(int argc, char* argv[]);
  ~Runner();  // Required for pImpl with std::unique_ptr

  Runner(Runner const&) = delete;
  Runner& operator=(Runner const&) = delete;
  Runner(Runner&&) noexcept = default;
  Runner& operator=(Runner&&) noexcept = default;

  auto Run() const -> int;

 private:
  class Impl;  // Forward declaration for pImpl
  std::unique_ptr<Impl> pimpl_;
};

}  // namespace vv

#endif  // VVP_APP_RUNNER_H_
