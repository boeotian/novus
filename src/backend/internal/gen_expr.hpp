#pragma once
#include "novasm/assembler.hpp"
#include "prog/expr/node_visitor.hpp"
#include "prog/program.hpp"
#include <optional>

namespace backend::internal {

class GenExpr final : public prog::expr::NodeVisitor {
public:
  GenExpr(
      const prog::Program& program,
      novasm::Assembler* asmb,
      const prog::sym::ConstDeclTable& constTable,
      std::optional<prog::sym::FuncId> curFunc,
      bool tail,
      unsigned int requestedValues);

  [[nodiscard]] auto getRequestedValues() -> unsigned int;
  [[nodiscard]] auto getValuesProduced() -> unsigned int;

  auto visit(const prog::expr::AssignExprNode& n) -> void override;
  auto visit(const prog::expr::SwitchExprNode& n) -> void override;
  auto visit(const prog::expr::CallExprNode& n) -> void override;
  auto visit(const prog::expr::CallDynExprNode& n) -> void override;
  auto visit(const prog::expr::CallSelfExprNode& n) -> void override;
  auto visit(const prog::expr::ClosureNode& n) -> void override;
  auto visit(const prog::expr::ConstExprNode& n) -> void override;
  auto visit(const prog::expr::FieldExprNode& n) -> void override;
  auto visit(const prog::expr::GroupExprNode& n) -> void override;
  auto visit(const prog::expr::UnionCheckExprNode& n) -> void override;
  auto visit(const prog::expr::UnionGetExprNode& n) -> void override;
  auto visit(const prog::expr::FailNode& n) -> void override;
  auto visit(const prog::expr::LitBoolNode& n) -> void override;
  auto visit(const prog::expr::LitFloatNode& n) -> void override;
  auto visit(const prog::expr::LitFuncNode& n) -> void override;
  auto visit(const prog::expr::LitIntNode& n) -> void override;
  auto visit(const prog::expr::LitLongNode& n) -> void override;
  auto visit(const prog::expr::LitStringNode& n) -> void override;
  auto visit(const prog::expr::LitCharNode& n) -> void override;
  auto visit(const prog::expr::LitEnumNode& n) -> void override;

private:
  const prog::Program& m_prog;
  novasm::Assembler* m_asmb;
  const prog::sym::ConstDeclTable& m_constTable;
  std::optional<prog::sym::FuncId> m_curFunc;
  bool m_tail;
  unsigned int m_requestedValues;
  unsigned int m_valuesProduced;

  template <typename CallTarget>
  auto makeLazy(const CallTarget& tgt, const std::vector<prog::expr::NodePtr>& args) -> void;

  auto makeClosure(const prog::expr::Node& lhs, const std::vector<prog::expr::NodePtr>& args)
      -> void;

  auto makeClosure(std::string targetLabel, const std::vector<prog::expr::NodePtr>& args) -> void;

  auto makeUnion(const prog::expr::CallExprNode& n) -> void;

  auto genSubExpr(const prog::expr::Node& n, bool tail, unsigned int requestedValues = 1)
      -> unsigned int;
};

} // namespace backend::internal
