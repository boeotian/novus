#include "vm/platform_interface.hpp"

namespace vm {

static int outputBufferSize = 512;

static auto setupOutputBuffering(std::FILE* file) -> void {
  // Enable full buffering.
  setvbuf(file, nullptr, _IOFBF, outputBufferSize);
}

PlatformInterface::PlatformInterface(
    int envArgsCount,
    char* const* envArgs,
    std::FILE* stdIn,
    std::FILE* stdOut,
    std::FILE* stdErr) noexcept :
    m_envArgsCount{envArgsCount},
    m_envArgs{envArgs},
    m_stdIn{stdIn},
    m_stdOut{stdOut},
    m_stdErr{stdErr} {

  if (stdOut != nullptr) {
    setupOutputBuffering(stdOut);
  }
  if (stdErr != nullptr) {
    setupOutputBuffering(stdErr);
  }
}

auto PlatformInterface::getEnvArg(int idx) noexcept -> const char* {
  return idx < 0 || idx >= m_envArgsCount ? nullptr : m_envArgs[idx];
}

auto PlatformInterface::getEnvArgCount() noexcept -> int { return m_envArgsCount; }

} // namespace vm
