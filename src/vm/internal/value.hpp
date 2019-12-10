#pragma once
#include "internal/ref.hpp"
#include <cassert>
#include <cstdint>

namespace vm::internal {

static const uint64_t refTag  = static_cast<uint64_t>(1U);
static const uint64_t valMask = ~static_cast<uint64_t>(1U);

class Value final {
  friend auto intValue(int32_t val) -> Value;
  friend auto floatValue(float val) -> Value;
  friend auto refValue(Ref* ref) -> Value;

  static_assert(
      sizeof(uintptr_t) <= sizeof(uint64_t)); // Assert we have enough space for a pointer.

  static_assert(
      alignof(Ref) > 1); // Assert that the alignment of 'Ref' leaves us a bit to store a tag.

  static_assert(sizeof(float) == sizeof(uint32_t)); // Assert that floats are 32 bit.

public:
  Value() = default;

  [[nodiscard]] inline auto isRef() const noexcept { return (m_raw & refTag) != 0U; }

  [[nodiscard]] inline auto getInt() const noexcept {
    assert(!isRef());
    auto upperRaw = static_cast<uint32_t>(m_raw >> 32U);
    return reinterpret_cast<int32_t&>(upperRaw); // NOLINT: Reinterpret cast
  }

  [[nodiscard]] inline auto getFloat() const noexcept {
    assert(!isRef());
    auto upperRaw = static_cast<uint32_t>(m_raw >> 32U);
    return reinterpret_cast<float&>(upperRaw); // NOLINT: Reinterpret cast
  }

  [[nodiscard]] inline auto getRef() const noexcept {
    assert(isRef());
    // Mask of the tag and interpret it as a pointer (works because due to alignment lowest bit is
    // always 0).
    return reinterpret_cast<Ref*>(m_raw & valMask); // NOLINT: Reinterpret cast
  }

private:
  uint64_t m_raw;

  inline explicit Value(uint64_t raw) : m_raw{raw} {}
};

[[nodiscard]] inline auto intValue(int32_t val) -> Value {
  // Int's are stored in the upper 32 bit of the raw value.
  auto upperRaw = reinterpret_cast<uint32_t&>(val); // NOLINT: Reinterpret cast
  return Value{static_cast<uint64_t>(upperRaw) << 32U};
}

[[nodiscard]] inline auto floatValue(float val) -> Value {
  // Float's are stored in the upper 32 bit of the raw value.
  auto upperRaw = reinterpret_cast<uint32_t&>(val); // NOLINT: Reinterpret cast
  return Value{static_cast<uint64_t>(upperRaw) << 32U};
}

[[nodiscard]] inline auto refValue(Ref* ref) -> Value {
  // First store the pointer in a variable of the native pointer size (might be 32 bit).
  uintptr_t rawRef = reinterpret_cast<uintptr_t>(ref); // NOLINT: Reinterpret cast

  // Then expand to 64 bit and tag it.
  return Value{static_cast<uint64_t>(rawRef) | refTag};
}

} // namespace vm::internal