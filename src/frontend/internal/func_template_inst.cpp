#include "func_template_inst.hpp"

namespace frontend::internal {

FuncTemplateInst::FuncTemplateInst(prog::sym::TypeSet typeParams) :
    m_typeParams{std::move(typeParams)}, m_func{std::nullopt}, m_retType{std::nullopt} {}

auto FuncTemplateInst::getTypeParams() const noexcept -> const prog::sym::TypeSet& {
  return m_typeParams;
}

auto FuncTemplateInst::getFunc() const noexcept -> std::optional<prog::sym::FuncId> {
  return m_func;
}

auto FuncTemplateInst::getRetType() const noexcept -> std::optional<prog::sym::TypeId> {
  return m_retType;
}

} // namespace frontend::internal