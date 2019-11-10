#pragma once
#include "prog/sym/type_id.hpp"
#include <initializer_list>
#include <iostream>
#include <vector>

namespace prog::sym {

class Input final {
  friend auto operator<<(std::ostream& out, const Input& rhs) -> std::ostream&;

public:
  using iterator = typename std::vector<TypeId>::const_iterator;

  Input(std::initializer_list<TypeId> list);
  explicit Input(std::vector<TypeId> val);

  [[nodiscard]] auto operator[](unsigned int) const -> TypeId;

  auto operator==(const Input& rhs) const noexcept -> bool;
  auto operator!=(const Input& rhs) const noexcept -> bool;

  [[nodiscard]] auto getCount() const -> unsigned int;

  [[nodiscard]] auto begin() const -> iterator;
  [[nodiscard]] auto end() const -> iterator;

private:
  std::vector<TypeId> m_val;
};

auto operator<<(std::ostream& out, const Input& rhs) -> std::ostream&;

} // namespace prog::sym
