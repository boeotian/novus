#pragma once
#include "internal/executor_handle.hpp"
#include "internal/ref_long.hpp"
#include "internal/ref_stream_console.hpp"
#include "internal/ref_stream_file.hpp"
#include "internal/ref_stream_tcp.hpp"
#include "internal/settings.hpp"
#include "internal/stack.hpp"
#include "internal/stream_utilities.hpp"
#include "internal/string_utilities.hpp"
#include "internal/terminal.hpp"
#include "novasm/pcall_code.hpp"
#include "vm/exec_state.hpp"
#include "vm/platform_interface.hpp"
#include <chrono>
#include <cstdlib>
#include <thread>

namespace vm::internal {

// Execute a 'platform' call. Very similar to normal instructions but are interacting with the
// 'outside' world (for example file io).
auto inline pcall(
    const Settings& settings,
    PlatformInterface* iface,
    RefAllocator* refAlloc,
    BasicStack* stack,
    ExecutorHandle* execHandle,
    novasm::PCallCode code) noexcept -> void {
  assert(iface && refAlloc && stack && execHandle);

  using PCallCode = novasm::PCallCode;

#define CHECK_ALLOC(PTR)                                                                           \
  {                                                                                                \
    if (unlikely((PTR) == nullptr)) {                                                              \
      if (execHandle->getState() != ExecState::Aborted) {                                          \
        execHandle->setState(ExecState::AllocFailed);                                              \
      }                                                                                            \
      return;                                                                                      \
    }                                                                                              \
  }
#define PUSH(VAL)                                                                                  \
  if (unlikely(!stack->push(VAL))) {                                                               \
    execHandle->setState(ExecState::StackOverflow);                                                \
    return;                                                                                        \
  }
#define PUSH_INT(VAL) PUSH(intValue(VAL))
#define PUSH_BOOL(VAL) PUSH(intValue(VAL))
#define PUSH_LONG(VAL)                                                                             \
  {                                                                                                \
    int64_t v = VAL;                                                                               \
    if (v >= 0L) {                                                                                 \
      PUSH(posLongValue(v));                                                                       \
    } else {                                                                                       \
      PUSH_REF(refAlloc->allocPlain<LongRef>(v));                                                  \
    }                                                                                              \
  }
#define PUSH_REF(VAL)                                                                              \
  {                                                                                                \
    auto* refPtr = VAL;                                                                            \
    CHECK_ALLOC(refPtr);                                                                           \
    PUSH(refValue(refPtr));                                                                        \
  }
#define POP_AT(IDX) stack->popAt(IDX)
#define POP() stack->pop()
#define POP_INT() POP().getInt()
#define PEEK() stack->peek()
#define PEEK_BEHIND(BEHIND) stack->peek(BEHIND)
#define PEEK_INT() PEEK().getInt()

  switch (code) {
  case PCallCode::StreamCheckValid: {
    PUSH_BOOL(streamCheckValid(POP()));
  } break;
  case PCallCode::StreamReadString: {
    auto maxChars = POP_INT();

    // Note: Keep the stream on the stack, reason is gc could run while we are blocked.
    auto stream = PEEK();
    // Allocate a new string, and push it on the stack (so its already visible to the gc).
    auto str = refAlloc->allocStr(maxChars <= 0 ? 0U : static_cast<unsigned int>(maxChars));
    PUSH_REF(str);

    streamReadString(execHandle, stream, str);

    POP_AT(1); // Pop the stream off the stack, 1 because its behind the result string.
  } break;
  case PCallCode::StreamReadChar: {
    // Note: Keep the stream on the stack, reason is gc could run while we are blocked.
    auto stream = PEEK();

    auto readChar = streamReadChar(execHandle, stream);

    POP(); // Pop the stream off the stack.
    PUSH_INT(readChar);
  } break;
  case PCallCode::StreamWriteString: {
    // Note: Keep the string on the stack, reason is gc could run while we are blocked.
    auto* strRef = getStringRef(refAlloc, PEEK());
    CHECK_ALLOC(strRef);

    // Note: Keep the stream on the stack, reason is gc could run while we are blocked.
    auto stream = PEEK_BEHIND(1);
    auto result = streamWriteString(execHandle, stream, strRef);

    POP(); // Pop the string off the stack.
    POP(); // Pop the stream off the stack.
    PUSH_BOOL(result);
  } break;
  case PCallCode::StreamWriteChar: {
    uint8_t val = static_cast<uint8_t>(POP_INT());

    // Note: Keep the stream on the stack, reason is gc could run while we are blocked.
    auto stream = PEEK();
    auto result = streamWriteChar(execHandle, stream, val);

    POP(); // Pop the stream off the stack.
    PUSH_BOOL(result);
  } break;
  case PCallCode::StreamFlush: {
    PUSH_BOOL(streamFlush(POP()));
  } break;
  case PCallCode::StreamSetOptions: {
    auto options = POP_INT();
    auto stream  = POP();
    PUSH_BOOL(streamSetOpts(stream, static_cast<StreamOpts>(options)));
  } break;
  case PCallCode::StreamUnsetOptions: {
    auto options = POP_INT();
    auto stream  = POP();
    PUSH_BOOL(streamUnsetOpts(stream, static_cast<StreamOpts>(options)));
  } break;

  case PCallCode::FileOpenStream: {
    auto options = POP_INT();
    // Mode is stored in the least significant 8 bits.
    // Flags is stored in the 8 bits before (more significant) then mode.
    auto mode        = static_cast<FileStreamMode>(static_cast<uint8_t>(options));
    auto flags       = static_cast<FileStreamFlags>(static_cast<uint8_t>(options >> 8U));
    auto* pathStrRef = getStringRef(refAlloc, POP());
    CHECK_ALLOC(pathStrRef);

    PUSH_REF(openFileStream(refAlloc, pathStrRef, mode, flags));
  } break;
  case PCallCode::FileRemove: {
    auto* pathStrRef = getStringRef(refAlloc, POP());
    CHECK_ALLOC(pathStrRef);

    PUSH_BOOL(removeFile(pathStrRef));
  } break;

  case PCallCode::TcpOpenCon: {
    auto port = POP_INT();

    // Note: Keep the 'address' string on the stack, reason is gc could run while we are blocked.
    auto addr = PEEK();
    auto* result =
        tcpOpenConnection(settings, execHandle, refAlloc, addr.getDowncastRef<StringRef>(), port);

    POP(); // Pop the 'address' string off the stack.
    PUSH_REF(result);
  } break;
  case PCallCode::TcpStartServer: {
    auto backlog = POP_INT();
    auto port    = POP_INT();
    PUSH_REF(tcpStartServer(settings, refAlloc, port, backlog));
  } break;
  case PCallCode::TcpAcceptCon: {

    // Note: Keep the stream on the stack, reason is gc could run while we are blocked.
    auto stream  = PEEK();
    auto* result = tcpAcceptConnection(settings, execHandle, refAlloc, stream);

    POP(); // Pop the stream off the stack.
    PUSH_REF(result);
  } break;
  case PCallCode::IpLookupAddress: {

    // Note: Keep the 'hostname' string on the stack, reason is gc could run while we are blocked.
    auto hostname = PEEK();
    auto* result =
        ipLookupAddress(settings, execHandle, refAlloc, hostname.getDowncastRef<StringRef>());

    POP(); // Pop the hostname off the stack.
    PUSH_REF(result);
  } break;

  case PCallCode::ConsoleOpenStream: {
    auto kind = static_cast<ConsoleStreamKind>(POP_INT());
    PUSH_REF(openConsoleStream(iface, refAlloc, kind));
  } break;

  case PCallCode::TermSetOptions: {
    auto options = POP_INT();
    PUSH_BOOL(termSetOpts(static_cast<TermOpts>(options)));
  } break;
  case PCallCode::TermUnsetOptions: {
    auto options = POP_INT();
    PUSH_BOOL(termUnsetOpts(static_cast<TermOpts>(options)));
  } break;
  case PCallCode::TermGetWidth: {
    PUSH_INT(termGetWidth());
  } break;
  case PCallCode::TermGetHeight: {
    PUSH_INT(termGetHeight());
  } break;

  case PCallCode::GetEnvArg: {
    auto* res = iface->getEnvArg(POP_INT());
    PUSH_REF(res == nullptr ? refAlloc->allocStr(0) : toStringRef(refAlloc, res));
  } break;
  case PCallCode::GetEnvArgCount: {
    PUSH_INT(iface->getEnvArgCount());
  } break;
  case PCallCode::GetEnvVar: {
    auto* nameStrRef = getStringRef(refAlloc, POP());
    CHECK_ALLOC(nameStrRef);

    auto* res = std::getenv(nameStrRef->getCharDataPtr());
    PUSH_REF(res == nullptr ? refAlloc->allocStr(0) : toStringRef(refAlloc, res));
  } break;

  case PCallCode::ClockMicroSinceEpoch: {
    const auto now = std::chrono::system_clock::now().time_since_epoch();
    PUSH_LONG(std::chrono::duration_cast<std::chrono::microseconds>(now).count());
  } break;
  case PCallCode::ClockNanoSteady: {
    auto now = std::chrono::steady_clock::now().time_since_epoch();
    PUSH_LONG(std::chrono::duration_cast<std::chrono::nanoseconds>(now).count());
  } break;

  case PCallCode::SleepNano: {
    auto sleepTime = std::chrono::nanoseconds(getLong(PEEK()));
    execHandle->setState(ExecState::Paused);
    std::this_thread::sleep_for(sleepTime);
    execHandle->setState(ExecState::Running);
    if (execHandle->trap()) {
      return;
    }
  } break;
  case PCallCode::Assert: {
    auto* msg = getStringRef(refAlloc, POP());
    CHECK_ALLOC(msg);

    auto cond = PEEK_INT();
    if (unlikely(cond == 0)) {
      auto* stdErr = iface->getStdErr();
      if (stdErr != nullptr) {
        std::fwrite("Assertion failed: ", 18, 1, stdErr);
        std::fwrite(msg->getDataPtr(), msg->getSize(), 1, stdErr);
        std::fputc('\n', stdErr);
      }
      execHandle->setState(ExecState::AssertFailed);
    }
  } break;
  default:
    execHandle->setState(ExecState::InvalidAssembly);
  }

#undef CHECK_ALLOC
#undef PUSH
#undef PUSH_INT
#undef PUSH_BOOL
#undef PUSH_LONG
#undef PUSH_REF
#undef POP
#undef POP_INT
#undef PEEK
#undef PEEK_BEHIND
#undef PEEK_INT
}

} // namespace vm::internal
