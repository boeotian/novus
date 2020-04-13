#pragma once
#include <cstdint>

#if !defined(_WIN32)

#include <termios.h>
#include <unistd.h>

#endif

namespace vm::internal {

enum class TermOpts : int32_t { NoEcho = 1 << 0, NoBuffer = 1 << 1 };

inline auto hasTerminal() -> bool {
#if defined(_WIN32)
  // TODO(bastian): Implement checking if terminal is active on windows.
  return false;
#else
  return isatty(0) == 1;
#endif
}

inline auto setTermOpts(TermOpts opts) -> bool {
  if (!hasTerminal()) {
    return false;
  }

#if defined(_WIN32)
  // TODO(bastian): Implement terminal control options for Windows.
#else
  /* Update the termios structure for file descriptor 0 (main terminal). */

  termios t;
  if (tcgetattr(0, &t) != 0) {
    return false;
  }

  if (static_cast<int32_t>(opts) & static_cast<int32_t>(TermOpts::NoEcho)) {
    t.c_lflag &= ~ECHO;
  }
  if (static_cast<int32_t>(opts) & static_cast<int32_t>(TermOpts::NoBuffer)) {
    t.c_lflag &= ~ICANON;
  }

  if (tcsetattr(0, TCSANOW, &t) != 0) {
    return false;
  }
  return true;
#endif
}

inline auto unsetTermOpts(TermOpts opts) -> bool {
  if (!hasTerminal()) {
    return false;
  }

#if defined(_WIN32)
  // TODO(bastian): Implement terminal control options for Windows.
#else
  /* Update the termios structure for file descriptor 0 (main terminal). */

  termios t;
  if (tcgetattr(0, &t)) {
    return false;
  }

  if (static_cast<int32_t>(opts) & static_cast<int32_t>(TermOpts::NoEcho)) {
    t.c_lflag |= ECHO;
  }
  if (static_cast<int32_t>(opts) & static_cast<int32_t>(TermOpts::NoBuffer)) {
    t.c_lflag |= ICANON;
  }

  if (tcsetattr(0, TCSANOW, &t)) {
    return false;
  }
  return true;
#endif
}

} // namespace vm::internal
