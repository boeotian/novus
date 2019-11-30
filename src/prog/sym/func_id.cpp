#include "prog/sym/func_id.hpp"

namespace prog::sym {

FuncId::FuncId(unsigned int id) : m_id{id} {}

auto FuncId::operator==(const FuncId& rhs) const noexcept -> bool { return m_id == rhs.m_id; }

auto FuncId::operator!=(const FuncId& rhs) const noexcept -> bool {
  return !FuncId::operator==(rhs);
}

auto operator<<(std::ostream& out, const FuncId& rhs) -> std::ostream& {
  return out << "func-" << rhs.m_id;
}

} // namespace prog::sym
