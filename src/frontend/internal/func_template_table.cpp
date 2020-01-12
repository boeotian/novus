#include "func_template_table.hpp"
#include <limits>

namespace frontend::internal {

auto FuncTemplateTable::hasFunc(const std::string& name) -> bool {
  return m_templates.find(name) != m_templates.end();
}

auto FuncTemplateTable::declare(
    Context* context,
    const std::string& name,
    std::vector<std::string> typeSubs,
    const parse::FuncDeclStmtNode* n) -> void {

  auto itr = m_templates.find(name);
  if (itr == m_templates.end()) {
    itr = m_templates.insert({name, std::vector<FuncTemplate>{}}).first;
  }
  itr->second.push_back(FuncTemplate{context, name, std::move(typeSubs), n});
}

auto FuncTemplateTable::instantiate(const std::string& name, const prog::sym::TypeSet& typeParams)
    -> std::vector<const FuncTemplateInst*> {

  auto itr = m_templates.find(name);
  if (itr == m_templates.end()) {
    return {};
  }

  auto result = std::vector<const FuncTemplateInst*>{};
  for (auto& funcTemplate : itr->second) {
    if (funcTemplate.getTypeParamCount() == typeParams.getCount()) {
      result.push_back(funcTemplate.instantiate(typeParams));
    }
  }
  return result;
}

auto FuncTemplateTable::inferParamsAndInstantiate(
    const std::string& name, const prog::sym::TypeSet& argTypes)
    -> std::vector<const FuncTemplateInst*> {

  auto result = std::vector<const FuncTemplateInst*>{};
  for (const auto& funcTemplAndParams : inferParams(name, argTypes)) {
    result.push_back(funcTemplAndParams.first->instantiate(funcTemplAndParams.second));
  }
  return result;
}

auto FuncTemplateTable::getRetType(const std::string& name, const prog::sym::TypeSet& typeParams)
    -> std::optional<prog::sym::TypeId> {
  auto itr = m_templates.find(name);
  if (itr == m_templates.end()) {
    return std::nullopt;
  }

  // Only return a value if all overloads agree on the result-type.
  std::optional<prog::sym::TypeId> result = std::nullopt;
  for (auto& funcTemplate : itr->second) {
    if (funcTemplate.getTypeParamCount() == typeParams.getCount()) {
      const auto retType = funcTemplate.getRetType(typeParams);
      if (!retType) {
        continue;
      }
      if (!result) {
        result = retType;
      } else if (*result != *retType) {
        return std::nullopt;
      }
    }
  }
  return result;
}

auto FuncTemplateTable::inferParamsAndGetRetType(
    const std::string& name, const prog::sym::TypeSet& argTypes)
    -> std::optional<prog::sym::TypeId> {

  // Only return a value if all templates agree on the result-type.
  std::optional<prog::sym::TypeId> result = std::nullopt;
  for (const auto& funcTemplAndParams : inferParams(name, argTypes)) {
    const auto retType = funcTemplAndParams.first->getRetType(funcTemplAndParams.second);
    if (!retType || retType->isInfer()) {
      continue;
    }
    if (!result) {
      result = retType;
    } else if (*result != *retType) {
      return std::nullopt;
    }
  }
  return result;
}

auto FuncTemplateTable::inferParams(const std::string& name, const prog::sym::TypeSet& argTypes)
    -> std::vector<std::pair<FuncTemplate*, prog::sym::TypeSet>> {

  auto itr = m_templates.find(name);
  if (itr == m_templates.end()) {
    return {};
  }

  /* Find the templates where we can successfully infer the type-parameters, prefer templates with
  // the least amount of type-parameters. Can return multiple templates if they have the same amount
  of type-parameters. */

  auto typeParamCount = std::numeric_limits<unsigned int>::max();
  auto result         = std::vector<std::pair<FuncTemplate*, prog::sym::TypeSet>>{};

  for (auto& funcTemplate : itr->second) {
    if (funcTemplate.getArgumentCount() == argTypes.getCount()) {
      const auto inferredTypeParams = funcTemplate.inferTypeParams(argTypes);
      if (inferredTypeParams) {
        if (funcTemplate.getTypeParamCount() < typeParamCount) {
          result.clear();
          result.emplace_back(&funcTemplate, *inferredTypeParams);
          typeParamCount = funcTemplate.getTypeParamCount();
        } else if (funcTemplate.getTypeParamCount() == typeParamCount) {
          result.emplace_back(&funcTemplate, *inferredTypeParams);
        }
      }
    }
  }

  return result;
}

} // namespace frontend::internal
