#pragma once
#include "prog/sym/func_decl.hpp"
#include "prog/sym/func_id.hpp"
#include "prog/sym/input.hpp"
#include "prog/sym/type_decl.hpp"
#include <string>
#include <unordered_map>
#include <vector>

namespace prog {

class Program;

namespace sym {

class FuncDeclTable final {
public:
  using iterator = typename std::vector<FuncDecl>::const_iterator;
  using id       = typename prog::sym::FuncId;

  FuncDeclTable()                             = default;
  FuncDeclTable(const FuncDeclTable& rhs)     = delete;
  FuncDeclTable(FuncDeclTable&& rhs) noexcept = default;
  ~FuncDeclTable()                            = default;

  auto operator=(const FuncDeclTable& rhs) -> FuncDeclTable& = delete;
  auto operator=(FuncDeclTable&& rhs) noexcept -> FuncDeclTable& = delete;

  [[nodiscard]] auto operator[](FuncId id) const -> const FuncDecl&;

  [[nodiscard]] auto begin() const -> iterator;
  [[nodiscard]] auto end() const -> iterator;

  [[nodiscard]] auto lookup(const std::string& name) const -> std::vector<FuncId>;
  [[nodiscard]] auto
  lookup(const Program& prog, const std::string& name, const Input& input, int maxConversions) const
      -> std::optional<FuncId>;

  auto
  registerFunc(const Program& prog, FuncKind kind, std::string name, Input input, TypeId output)
      -> FuncId;

  auto updateFuncOutput(FuncId id, TypeId newOutput) -> void;

private:
  std::vector<FuncDecl> m_funcs;
  std::unordered_map<std::string, std::vector<FuncId>> m_lookup;
};

} // namespace sym

} // namespace prog