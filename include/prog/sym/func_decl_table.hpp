#pragma once
#include "prog/sym/func_decl.hpp"
#include "prog/sym/func_id.hpp"
#include "prog/sym/overload_options.hpp"
#include "prog/sym/type_decl.hpp"
#include "prog/sym/type_set.hpp"
#include <map>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace prog {

class Program;

namespace sym {

// Function declaration table.
// Table where all functions are declared. Multiple functions with the same name but different
// signatures are allowed.
//
// Note: action indicate the function is 'impure' and non-action indicates the function is pure.
// Implicit conversions are required to be pure.
//
class FuncDeclTable final {
public:
  using Iterator = std::map<FuncId, FuncDecl>::const_iterator;
  using Id       = prog::sym::FuncId;

  FuncDeclTable()                         = default;
  FuncDeclTable(const FuncDeclTable& rhs) = delete;
  FuncDeclTable(FuncDeclTable&& rhs)      = default;
  ~FuncDeclTable()                        = default;

  auto operator=(const FuncDeclTable& rhs) -> FuncDeclTable& = delete;
  auto operator=(FuncDeclTable&& rhs) noexcept -> FuncDeclTable& = default;

  [[nodiscard]] auto operator[](FuncId id) const -> const FuncDecl&;

  [[nodiscard]] auto getCount() const -> unsigned int;
  [[nodiscard]] auto begin() const -> Iterator;
  [[nodiscard]] auto end() const -> Iterator;

  [[nodiscard]] auto exists(const FuncId& id) const -> bool;
  [[nodiscard]] auto exists(const std::string& name) const -> bool;
  [[nodiscard]] auto lookup(const std::string& name, OverloadOptions options) const
      -> std::vector<FuncId>;
  [[nodiscard]] auto lookup(
      const Program& prog,
      const std::string& name,
      const TypeSet& input,
      OverloadOptions options) const -> std::optional<FuncId>;

  auto registerImplicitConv(const Program& prog, FuncKind kind, TypeId input, TypeId output)
      -> FuncId;

  auto
  registerFunc(const Program& prog, FuncKind kind, std::string name, TypeSet input, TypeId output)
      -> FuncId;

  auto
  registerAction(const Program& prog, FuncKind kind, std::string name, TypeSet input, TypeId output)
      -> FuncId;

  auto insertFunc(
      FuncId id,
      FuncKind kind,
      bool isAction,
      bool isImplicitConv,
      std::string name,
      TypeSet input,
      TypeId output) -> void;

  auto updateFuncOutput(FuncId id, TypeId newOutput) -> void;

private:
  std::map<FuncId, FuncDecl> m_funcs;
  std::unordered_map<std::string, std::vector<FuncId>> m_lookup;

  auto registerFunc(
      const Program& prog,
      FuncKind kind,
      bool isAction,
      bool isImplicitConv,
      std::string name,
      TypeSet input,
      TypeId output) -> FuncId;
};

} // namespace sym

} // namespace prog
