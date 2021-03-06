#include "prog/expr/node_call.hpp"
#include "internal/implicit_conv.hpp"
#include "utilities.hpp"
#include <sstream>
#include <stdexcept>

namespace prog::expr {

CallExprNode::CallExprNode(
    sym::FuncId func, sym::TypeId resultType, std::vector<NodePtr> args, CallMode mode) :
    Node{CallExprNode::getKind()},
    m_func{func},
    m_resultType{resultType},
    m_args{std::move(args)},
    m_mode{mode} {}

auto CallExprNode::operator==(const Node& rhs) const noexcept -> bool {
  const auto r = dynamic_cast<const CallExprNode*>(&rhs);
  return r != nullptr && m_func == r->m_func && nodesEqual(m_args, r->m_args);
}

auto CallExprNode::operator!=(const Node& rhs) const noexcept -> bool {
  return !CallExprNode::operator==(rhs);
}

auto CallExprNode::operator[](unsigned int i) const -> const Node& {
  if (i >= m_args.size()) {
    throw std::out_of_range("No child at given index");
  }
  return *m_args[i];
}

auto CallExprNode::getChildCount() const -> unsigned int { return m_args.size(); }

auto CallExprNode::getType() const noexcept -> sym::TypeId { return m_resultType; }

auto CallExprNode::toString() const -> std::string {
  auto oss = std::ostringstream{};
  oss << "call-" << m_func;
  return oss.str();
}

auto CallExprNode::clone(Rewriter* rewriter) const -> std::unique_ptr<Node> {
  return std::unique_ptr<CallExprNode>{
      new CallExprNode{m_func, m_resultType, cloneNodes(m_args, rewriter), m_mode}};
}

auto CallExprNode::getFunc() const noexcept -> sym::FuncId { return m_func; }

auto CallExprNode::getArgs() const noexcept -> const std::vector<NodePtr>& { return m_args; }

auto CallExprNode::getMode() const noexcept -> CallMode { return m_mode; }

auto CallExprNode::isFork() const noexcept -> bool { return m_mode == CallMode::Forked; }

auto CallExprNode::isLazy() const noexcept -> bool { return m_mode == CallMode::Lazy; }

auto CallExprNode::accept(NodeVisitor* visitor) const -> void { visitor->visit(*this); }

// Factories.
auto callExprNode(const Program& prog, sym::FuncId func, std::vector<NodePtr> args) -> NodePtr {
  const auto& funcDecl = prog.getFuncDecl(func);
  return callExprNode(prog, func, funcDecl.getOutput(), std::move(args), CallMode::Normal);
}

auto callExprNode(
    const Program& prog,
    sym::FuncId func,
    sym::TypeId resultType,
    std::vector<NodePtr> args,
    CallMode mode) -> NodePtr {

  if (anyNodeNull(args)) {
    throw std::invalid_argument{"Call node cannot contain a null argument"};
  }
  const auto& funcDecl = prog.getFuncDecl(func);

  // Validate mode.
  switch (mode) {
  case CallMode::Forked:
    if (funcDecl.getKind() != sym::FuncKind::User) {
      throw std::invalid_argument{"Only user functions can be forked"};
    }
    if (!prog.isFuture(resultType)) {
      throw std::invalid_argument{"Forks have to return a future type"};
    }
    break;
  case CallMode::Lazy:
    if (funcDecl.getKind() != sym::FuncKind::User) {
      throw std::invalid_argument{"Only user functions can be lazy"};
    }
    if (funcDecl.isAction()) {
      throw std::invalid_argument{"Lazy calls cannot be made to actions (impure)"};
    }
    if (!prog.isLazy(resultType)) {
      throw std::invalid_argument{"Lazy calls have to return a lazy type"};
    }
  case CallMode::Normal:
    break;
  }

  // Validate arguments.
  const auto& funcInput = funcDecl.getInput();
  if (funcInput.getCount() != args.size()) {
    throw std::invalid_argument{"Call node contains incorrect number of arguments"};
  }

  // Apply implicit conversions if necessary (and throw if types are incompatible).
  internal::applyImplicitConversions(prog, funcDecl.getInput(), &args);

  return std::unique_ptr<CallExprNode>{
      new CallExprNode{funcDecl.getId(), resultType, std::move(args), mode}};
}

} // namespace prog::expr
