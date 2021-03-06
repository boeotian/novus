#pragma once
#include "lex/token.hpp"
#include "parse/node.hpp"

namespace parse {

// If condition and branch combination for a switch expression.
// Example in source: 'if a > 42 -> "hello world"'.
class SwitchExprIfNode final : public Node {
  friend auto switchExprIfNode(lex::Token kw, NodePtr cond, lex::Token arrow, NodePtr rhs)
      -> NodePtr;

public:
  SwitchExprIfNode() = delete;

  auto operator==(const Node& rhs) const noexcept -> bool override;
  auto operator!=(const Node& rhs) const noexcept -> bool override;

  [[nodiscard]] auto operator[](unsigned int i) const -> const Node& override;
  [[nodiscard]] auto getChildCount() const -> unsigned int override;
  [[nodiscard]] auto getSpan() const -> input::Span override;

  auto accept(NodeVisitor* visitor) const -> void override;

private:
  const lex::Token m_kw;
  const NodePtr m_cond;
  const lex::Token m_arrow;
  const NodePtr m_rhs;

  SwitchExprIfNode(lex::Token kw, NodePtr cond, lex::Token arrow, NodePtr rhs);

  auto print(std::ostream& out) const -> std::ostream& override;
};

// Factories.
auto switchExprIfNode(lex::Token kw, NodePtr cond, lex::Token arrow, NodePtr rhs) -> NodePtr;

} // namespace parse
