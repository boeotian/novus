#pragma once
#include "prog/expr/node.hpp"
#include "prog/program.hpp"

namespace prog::expr {

class CallDynExprNode final : public Node {
  friend auto callDynExprNode(const Program& prog, NodePtr lhs, std::vector<NodePtr> args)
      -> NodePtr;

public:
  CallDynExprNode() = delete;

  auto operator==(const Node& rhs) const noexcept -> bool override;
  auto operator!=(const Node& rhs) const noexcept -> bool override;

  [[nodiscard]] auto operator[](unsigned int i) const -> const Node& override;
  [[nodiscard]] auto getChildCount() const -> unsigned int override;
  [[nodiscard]] auto getType() const noexcept -> sym::TypeId override;
  [[nodiscard]] auto toString() const -> std::string override;

  auto accept(NodeVisitor* visitor) const -> void override;

private:
  NodePtr m_lhs;
  sym::TypeId m_resultType;
  std::vector<NodePtr> m_args;

  CallDynExprNode(NodePtr lhs, sym::TypeId resultType, std::vector<NodePtr> args);
};

// Factories.
auto callDynExprNode(const Program& prog, NodePtr lhs, std::vector<NodePtr> args) -> NodePtr;

} // namespace prog::expr