#pragma once
#include "lex/token.hpp"
#include "parse/node.hpp"
#include <vector>

namespace parse {

// Group expression. Comma seperated list of expressions.
// Example in source: 'a = 2, b = 3, a + b'.
class GroupExprNode final : public Node {
  friend auto groupExprNode(std::vector<NodePtr> subExprs, std::vector<lex::Token> semis)
      -> NodePtr;

public:
  GroupExprNode() = delete;

  auto operator==(const Node& rhs) const noexcept -> bool override;
  auto operator!=(const Node& rhs) const noexcept -> bool override;

  [[nodiscard]] auto operator[](unsigned int i) const -> const Node& override;
  [[nodiscard]] auto getChildCount() const -> unsigned int override;
  [[nodiscard]] auto getSpan() const -> input::Span override;

  auto accept(NodeVisitor* visitor) const -> void override;

private:
  const std::vector<NodePtr> m_subExprs;
  const std::vector<lex::Token> m_semis;

  GroupExprNode(std::vector<NodePtr> subExprs, std::vector<lex::Token> semis);

  auto print(std::ostream& out) const -> std::ostream& override;
};

// Factories.
auto groupExprNode(std::vector<NodePtr> subExprs, std::vector<lex::Token> semis) -> NodePtr;

} // namespace parse
