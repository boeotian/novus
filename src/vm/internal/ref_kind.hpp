#pragma once

namespace vm::internal {

enum class RefKind : unsigned int {
  String = 1,
  Struct = 2,
  Future = 3,
};

} // namespace vm::internal
