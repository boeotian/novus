#include "vm/executor.hpp"
#include "internal/allocator.hpp"
#include "internal/call_stack.hpp"
#include "internal/const_stack.hpp"
#include "internal/eval_stack.hpp"
#include "internal/ref_string.hpp"
#include "vm/exceptions/div_by_zero.hpp"
#include "vm/exceptions/eval_stack_not_empty.hpp"
#include "vm/exceptions/invalid_assembly.hpp"
#include "vm/opcode.hpp"
#include <charconv>
#include <cstdio>
#include <stdexcept>

namespace vm {

static const int EvalStackSize   = 4096;
static const int ConstsStackSize = 4096;

static auto execute(const Assembly& assembly, io::Interface* interface, uint32_t entryPoint) {
  auto evalStack  = internal::EvalStack{EvalStackSize};
  auto constStack = internal::ConstStack{ConstsStackSize};
  auto callStack  = internal::CallStack{};
  auto allocator  = internal::Allocator{};
  callStack.push(assembly, entryPoint);

  while (true) {
    auto scope = callStack.getTop();
    switch (scope->readOpCode()) {
    case OpCode::LoadLitInt: {
      auto litInt = scope->readInt32();
      evalStack.push(internal::intValue(litInt));
    } break;
    case OpCode::LoadLitFloat: {
      auto litFloat = scope->readFloat();
      evalStack.push(internal::floatValue(litFloat));
    } break;
    case OpCode::LoadLitString: {
      const auto litStrId = scope->readInt32();
      const auto& litStr  = assembly.getLitString(litStrId);
      auto strRef         = allocator.allocStrLit(litStr);
      evalStack.push(internal::refValue(strRef));
    } break;

    case OpCode::ReserveConsts: {
      auto amount = scope->readUInt8();
      scope->reserveConsts(&constStack, amount);
      break;
    }
    case OpCode::StoreConst: {
      auto constId = scope->readUInt8();
      auto a       = evalStack.pop();
      scope->setConst(constId, a);
    } break;
    case OpCode::LoadConst: {
      auto constId = scope->readUInt8();
      evalStack.push(scope->getConst(constId));
    } break;

    case OpCode::AddInt: {
      auto b = evalStack.pop().getInt();
      auto a = evalStack.pop().getInt();
      evalStack.push(internal::intValue(a + b));
    } break;
    case OpCode::AddFloat: {
      auto b = evalStack.pop().getFloat();
      auto a = evalStack.pop().getFloat();
      evalStack.push(internal::floatValue(a + b));
    } break;
    case OpCode::AddString: {
      auto b = getStringRef(evalStack.pop());
      auto a = getStringRef(evalStack.pop());

      // Make a new string big enough to fit both and copy both there.
      auto result = allocator.allocStr(a->getSize() + b->getSize());
      std::memcpy(result.second, a->getDataPtr(), a->getSize());
      std::memcpy(result.second + a->getSize(), b->getDataPtr(), b->getSize());
      evalStack.push(internal::refValue(result.first));
    } break;
    case OpCode::SubInt: {
      auto b = evalStack.pop().getInt();
      auto a = evalStack.pop().getInt();
      evalStack.push(internal::intValue(a - b));
    } break;
    case OpCode::SubFloat: {
      auto b = evalStack.pop().getFloat();
      auto a = evalStack.pop().getFloat();
      evalStack.push(internal::floatValue(a - b));
    } break;
    case OpCode::MulInt: {
      auto b = evalStack.pop().getInt();
      auto a = evalStack.pop().getInt();
      evalStack.push(internal::intValue(a * b));
    } break;
    case OpCode::MulFloat: {
      auto b = evalStack.pop().getFloat();
      auto a = evalStack.pop().getFloat();
      evalStack.push(internal::floatValue(a * b));
    } break;
    case OpCode::DivInt: {
      auto b = evalStack.pop().getInt();
      auto a = evalStack.pop().getInt();
      if (b == 0) {
        throw exceptions::DivByZero{};
      }
      evalStack.push(internal::intValue(a / b));
    } break;
    case OpCode::DivFloat: {
      auto b = evalStack.pop().getFloat();
      auto a = evalStack.pop().getFloat();
      if (b == 0) {
        throw exceptions::DivByZero{};
      }
      evalStack.push(internal::floatValue(a / b));
    } break;
    case OpCode::RemInt: {
      auto b = evalStack.pop().getInt();
      auto a = evalStack.pop().getInt();
      if (b == 0) {
        throw exceptions::DivByZero{};
      }
      evalStack.push(internal::intValue(a % b));
    } break;
    case OpCode::NegInt: {
      auto a = evalStack.pop().getInt();
      evalStack.push(internal::intValue(-a));
    } break;
    case OpCode::NegFloat: {
      auto a = evalStack.pop().getFloat();
      evalStack.push(internal::floatValue(-a));
    } break;
    case OpCode::LogicInvInt: {
      auto a = evalStack.pop().getInt();
      evalStack.push(internal::intValue(a == 0 ? 1 : 0));
    } break;

    case OpCode::CheckEqInt: {
      auto b = evalStack.pop().getInt();
      auto a = evalStack.pop().getInt();
      evalStack.push(internal::intValue(a == b ? 1 : 0));
    } break;
    case OpCode::CheckEqFloat: {
      auto b = evalStack.pop().getFloat();
      auto a = evalStack.pop().getFloat();
      evalStack.push(internal::intValue(a == b ? 1 : 0));
    } break;
    case OpCode::CheckEqString: {
      auto b  = getStringRef(evalStack.pop());
      auto a  = getStringRef(evalStack.pop());
      auto eq = (a->getSize() == b->getSize()) &&
          std::memcmp(a->getDataPtr(), b->getDataPtr(), a->getSize()) == 0;
      evalStack.push(internal::intValue(eq ? 1 : 0));
    } break;
    case OpCode::CheckGtInt: {
      auto b = evalStack.pop().getInt();
      auto a = evalStack.pop().getInt();
      evalStack.push(internal::intValue(a > b ? 1 : 0));
    } break;
    case OpCode::CheckGtFloat: {
      auto b = evalStack.pop().getFloat();
      auto a = evalStack.pop().getFloat();
      evalStack.push(internal::intValue(a > b ? 1 : 0));
    } break;
    case OpCode::CheckLeInt: {
      auto b = evalStack.pop().getInt();
      auto a = evalStack.pop().getInt();
      evalStack.push(internal::intValue(a < b ? 1 : 0));
    } break;
    case OpCode::CheckLeFloat: {
      auto b = evalStack.pop().getFloat();
      auto a = evalStack.pop().getFloat();
      evalStack.push(internal::intValue(a < b ? 1 : 0));
    } break;

    case OpCode::ConvIntFloat: {
      auto val = evalStack.pop().getInt();
      evalStack.push(internal::floatValue(static_cast<float>(val)));
    } break;
    case OpCode::ConvFloatInt: {
      auto val = evalStack.pop().getFloat();
      evalStack.push(internal::intValue(static_cast<int>(val)));
    } break;
    case OpCode::ConvIntString: {
      static const auto maxCharSize = 11;

      const auto val         = evalStack.pop().getInt();
      const auto strRefAlloc = allocator.allocStr(maxCharSize);
      const auto convRes = std::to_chars(strRefAlloc.second, strRefAlloc.second + maxCharSize, val);
      if (convRes.ec != std::errc()) {
        throw std::logic_error{"Failed to convert integer to string"};
      }
      strRefAlloc.first->updateSize(convRes.ptr - strRefAlloc.second);
      evalStack.push(internal::refValue(strRefAlloc.first));
    } break;
    case OpCode::ConvFloatString: {
      const auto val = evalStack.pop().getFloat();

      // NOLINTNEXTLINE: C-style var-arg func, needed because clang is missing std::to_chars(float).
      const auto charSize    = std::snprintf(nullptr, 0, "%.6g", val) + 1; // +1: null-terminator.
      const auto strRefAlloc = allocator.allocStr(charSize);

      // NOLINTNEXTLINE: C-style var-arg func, needed because clang is missing std::to_chars(float).
      std::snprintf(strRefAlloc.second, charSize, "%.6g", val);

      // Remove the null-terminator from the size. Our strings don't use a null-terminator but
      // snprintf always outputs one.
      strRefAlloc.first->updateSize(charSize - 1);
      evalStack.push(internal::refValue(strRefAlloc.first));
    } break;

    case OpCode::MakeStruct: {
      const auto fieldCount = scope->readUInt8();
      auto structRefAlloc   = allocator.allocStruct(fieldCount);
      // Important to iterate in reverse, as the fields are in reverse order on the stack.
      for (auto fieldIndex = fieldCount; fieldIndex-- > 0;) {
        *(structRefAlloc.second + fieldIndex) = evalStack.pop();
      }
      evalStack.push(internal::refValue(structRefAlloc.first));
    } break;
    case OpCode::LoadStructField: {
      const auto fieldIndex = scope->readUInt8();
      auto s                = getStructRef(evalStack.pop());
      evalStack.push(s->getField(fieldIndex));
    } break;

    case OpCode::PrintString: {
      auto* strRef = getStringRef(evalStack.pop());
      interface->print(strRef->getDataPtr(), strRef->getSize());
    } break;

    case OpCode::Jump: {
      auto ipOffset = scope->readUInt32();
      scope->jump(ipOffset);
    } break;
    case OpCode::JumpIf: {
      auto ipOffset = scope->readUInt32();
      auto a        = evalStack.pop().getInt();
      if (a != 0) {
        scope->jump(ipOffset);
      }
    } break;

    case OpCode::Call: {
      auto ipOffset = scope->readUInt32();
      callStack.push(assembly, ipOffset);
    } break;
    case OpCode::Ret: {
      scope->releaseConsts(&constStack);
      if (!callStack.pop()) {
        if (evalStack.getSize() != 0) {
          throw exceptions::EvalStackNotEmpty{};
        }
        assert(constStack.getSize() == 0);
        return; // Execution finishes after we returned from the last scope.
      }
    } break;

    case OpCode::Dup:
      evalStack.push(evalStack.peek());
      break;
    case OpCode::Pop:
      evalStack.pop();
      break;

    case OpCode::Fail:
    default:
      throw exceptions::InvalidAssembly{};
    }
  }
}

auto execute(const Assembly& assembly, io::Interface* interface) -> void {
  if (!interface) {
    throw std::invalid_argument{"Interface cannot be null"};
  }
  for (auto itr = assembly.beginEntryPoints(); itr != assembly.endEntryPoints(); ++itr) {
    execute(assembly, interface, *itr);
  }
}

} // namespace vm