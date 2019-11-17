#pragma once
#include "lex/token.hpp"
#include "parse/node.hpp"
#include "parse/node_stmt_func_decl.hpp"

namespace parse {

auto errLexError(lex::Token errToken) -> NodePtr;

auto errInvalidStmtFuncDecl(
    lex::Token kw,
    lex::Token id,
    lex::Token open,
    const std::vector<FuncDeclStmtNode::arg>& args,
    std::vector<lex::Token> commas,
    lex::Token close,
    lex::Token arrow,
    lex::Token retType,
    NodePtr body) -> NodePtr;

auto errInvalidStmtExec(
    lex::Token action,
    lex::Token open,
    std::vector<NodePtr> args,
    std::vector<lex::Token> commas,
    lex::Token close) -> NodePtr;

auto errInvalidPrimaryExpr(lex::Token token) -> NodePtr;

auto errInvalidUnaryOp(lex::Token op, NodePtr rhs) -> NodePtr;

auto errInvalidParenExpr(lex::Token open, NodePtr expr, lex::Token close) -> NodePtr;

auto errInvalidCallExpr(
    lex::Token func,
    lex::Token open,
    std::vector<NodePtr> args,
    std::vector<lex::Token> commas,
    lex::Token close) -> NodePtr;

auto errInvalidConditionalExpr(
    NodePtr cond, lex::Token qmark, NodePtr ifBranch, lex::Token colon, NodePtr elseBranch)
    -> NodePtr;

auto errInvalidSwitchIf(lex::Token kw, NodePtr cond, lex::Token arrow, NodePtr rhs) -> NodePtr;

auto errInvalidSwitchElse(lex::Token kw, lex::Token arrow, NodePtr rhs) -> NodePtr;

} // namespace parse
