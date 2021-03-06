#pragma once
#include "prog/expr/node.hpp"
#include "prog/sym/const_decl_table.hpp"
#include "prog/sym/const_id.hpp"

namespace prog::expr {

// Assign the result of an expression to a constant.
class AssignExprNode final : public Node {
  friend auto
  assignExprNode(const sym::ConstDeclTable& constTable, sym::ConstId constId, NodePtr expr)
      -> NodePtr;

public:
  AssignExprNode() = delete;

  auto operator==(const Node& rhs) const noexcept -> bool override;
  auto operator!=(const Node& rhs) const noexcept -> bool override;

  [[nodiscard]] constexpr static auto getKind() { return NodeKind::Assign; }

  [[nodiscard]] auto operator[](unsigned int i) const -> const Node& override;
  [[nodiscard]] auto getChildCount() const -> unsigned int override;
  [[nodiscard]] auto getType() const noexcept -> sym::TypeId override;
  [[nodiscard]] auto toString() const -> std::string override;

  [[nodiscard]] auto clone(Rewriter* rewriter) const -> std::unique_ptr<Node> override;

  [[nodiscard]] auto getConst() const noexcept -> sym::ConstId;

  auto accept(NodeVisitor* visitor) const -> void override;

private:
  sym::ConstId m_constId;
  NodePtr m_expr;

  AssignExprNode(sym::ConstId constId, NodePtr expr);
};

// Factories.
auto assignExprNode(const sym::ConstDeclTable& constTable, sym::ConstId constId, NodePtr expr)
    -> NodePtr;

} // namespace prog::expr
