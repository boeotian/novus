#pragma once
#include "lex/token.hpp"
#include "parse/node.hpp"

namespace parse {

// Expression that takes two expressions and an operation.
// Example in source: '2 + 2' or 'a + b' or 'func(43) / 4'.
class BinaryExprNode final : public Node {
  friend auto binaryExprNode(NodePtr lhs, lex::Token op, NodePtr rhs) -> NodePtr;

public:
  BinaryExprNode() = delete;

  auto operator==(const Node& rhs) const noexcept -> bool override;
  auto operator!=(const Node& rhs) const noexcept -> bool override;

  [[nodiscard]] auto operator[](unsigned int i) const -> const Node& override;
  [[nodiscard]] auto getChildCount() const -> unsigned int override;
  [[nodiscard]] auto getSpan() const -> input::Span override;

  [[nodiscard]] auto getOperator() const -> const lex::Token&;

  auto accept(NodeVisitor* visitor) const -> void override;

private:
  const NodePtr m_lhs;
  const lex::Token m_op;
  const NodePtr m_rhs;

  BinaryExprNode(NodePtr lhs, lex::Token op, NodePtr rhs);

  auto print(std::ostream& out) const -> std::ostream& override;
};

// Factories.
auto binaryExprNode(NodePtr lhs, lex::Token op, NodePtr rhs) -> NodePtr;

} // namespace parse
