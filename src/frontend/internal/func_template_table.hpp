#pragma once
#include "internal/func_template.hpp"
#include <unordered_map>
#include <utility>

namespace frontend::internal {

class FuncTemplateTable final {
public:
  FuncTemplateTable()                                 = default;
  FuncTemplateTable(const FuncTemplateTable& rhs)     = delete;
  FuncTemplateTable(FuncTemplateTable&& rhs) noexcept = default;
  ~FuncTemplateTable()                                = default;

  auto operator=(const FuncTemplateTable& rhs) -> FuncTemplateTable& = delete;
  auto operator=(FuncTemplateTable&& rhs) noexcept -> FuncTemplateTable& = delete;

  auto hasFunc(const std::string& name) -> bool;

  auto declare(
      Context* context,
      const std::string& name,
      std::vector<std::string> typeSubs,
      const parse::FuncDeclStmtNode* n) -> void;

  [[nodiscard]] auto instantiate(const std::string& name, const prog::sym::TypeSet& typeParams)
      -> std::vector<const FuncTemplateInst*>;

  [[nodiscard]] auto
  inferParamsAndInstantiate(const std::string& name, const prog::sym::TypeSet& argTypes)
      -> std::vector<const FuncTemplateInst*>;

  [[nodiscard]] auto getRetType(const std::string& name, const prog::sym::TypeSet& typeParams)
      -> std::optional<prog::sym::TypeId>;

  [[nodiscard]] auto
  inferParamsAndGetRetType(const std::string& name, const prog::sym::TypeSet& argTypes)
      -> std::optional<prog::sym::TypeId>;

private:
  std::unordered_map<std::string, std::vector<FuncTemplate>> m_templates;

  auto inferParams(const std::string& name, const prog::sym::TypeSet& argTypes)
      -> std::vector<std::pair<FuncTemplate*, prog::sym::TypeSet>>;
};

} // namespace frontend::internal
