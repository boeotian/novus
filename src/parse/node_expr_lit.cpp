#include "parse/node_expr_lit.hpp"

namespace parse {

LitExprNode::LitExprNode(lex::Token val) : m_val{std::move(val)} {}

auto LitExprNode::operator==(const Node& rhs) const noexcept -> bool {
  const auto r = dynamic_cast<const LitExprNode*>(&rhs);
  return r != nullptr && m_val == r->m_val;
}

auto LitExprNode::operator!=(const Node& rhs) const noexcept -> bool {
  return !LitExprNode::operator==(rhs);
}

auto LitExprNode::operator[](unsigned int /*unused*/) const -> const Node& {
  throw std::out_of_range{"No child at given index"};
}

auto LitExprNode::getChildCount() const -> unsigned int { return 0; }

auto LitExprNode::getSpan() const -> input::Span { return m_val.getSpan(); }

auto LitExprNode::getVal() const -> const lex::Token& { return m_val; }

auto LitExprNode::accept(NodeVisitor* visitor) const -> void { visitor->visit(*this); }

auto LitExprNode::print(std::ostream& out) const -> std::ostream& { return out << m_val; }

// Factories.
auto litExprNode(lex::Token val) -> NodePtr {
  return std::unique_ptr<LitExprNode>{new LitExprNode{std::move(val)}};
}

} // namespace parse
