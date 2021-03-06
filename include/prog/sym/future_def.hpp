#pragma once
#include "prog/sym/type_id.hpp"

namespace prog::sym {

// Definition of a future.
// Future is a handle to a value that is being computed by another excecutor.
class FutureDef final {
  friend class TypeDefTable;

public:
  FutureDef()                         = delete;
  FutureDef(const FutureDef& rhs)     = default;
  FutureDef(FutureDef&& rhs) noexcept = default;
  ~FutureDef()                        = default;

  auto operator=(const FutureDef& rhs) -> FutureDef& = delete;
  auto operator=(FutureDef&& rhs) noexcept -> FutureDef& = delete;

  [[nodiscard]] auto getId() const noexcept -> const TypeId&;
  [[nodiscard]] auto getResult() const -> const TypeId&;

private:
  sym::TypeId m_id;
  TypeId m_result;

  FutureDef(sym::TypeId id, TypeId result);
};

} // namespace prog::sym
