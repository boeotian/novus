#pragma once
#include "prog/expr/node.hpp"
#include "prog/program.hpp"

namespace prog::expr {

// Execute a recursive call to the current function.
// Note: Can only be used in a function body.
class CallSelfExprNode final : public Node {
  friend auto callSelfExprNode(sym::TypeId resultType, std::vector<NodePtr> args) -> NodePtr;

public:
  CallSelfExprNode() = delete;

  auto operator==(const Node& rhs) const noexcept -> bool override;
  auto operator!=(const Node& rhs) const noexcept -> bool override;

  [[nodiscard]] constexpr static auto getKind() { return NodeKind::CallSelf; }

  [[nodiscard]] auto operator[](unsigned int i) const -> const Node& override;
  [[nodiscard]] auto getChildCount() const -> unsigned int override;
  [[nodiscard]] auto getType() const noexcept -> sym::TypeId override;
  [[nodiscard]] auto toString() const -> std::string override;

  [[nodiscard]] auto clone(Rewriter* rewriter) const -> std::unique_ptr<Node> override;

  auto accept(NodeVisitor* visitor) const -> void override;

private:
  sym::TypeId m_resultType;
  std::vector<NodePtr> m_args;

  CallSelfExprNode(sym::TypeId resultType, std::vector<NodePtr> args);
};

// Factories.
auto callSelfExprNode(sym::TypeId resultType, std::vector<NodePtr> args) -> NodePtr;

} // namespace prog::expr
