#include "parse/parser.hpp"
#include "op_precedence.hpp"
#include "parse/error.hpp"
#include "parse/nodes.hpp"
#include "utilities.hpp"
#include <vector>

namespace parse {

namespace internal {

auto ParserImpl::next() -> NodePtr { return nextStmt(); }

auto ParserImpl::nextStmt() -> NodePtr {
  if (peekToken(0).isEnd()) {
    return nullptr;
  }

  const auto kw = getKw(peekToken(0));
  if (kw) {
    switch (*kw) {
    case lex::Keyword::Fun:
      return nextStmtFuncDecl();
    case lex::Keyword::Struct:
      return nextStmtStructDecl();
    case lex::Keyword::Union:
      return nextStmtUnionDecl();
    case lex::Keyword::Enum:
      return nextStmtEnumDecl();
    default:
      break;
    }
  }

  if (peekToken(0).getKind() == lex::TokenKind::LineComment) {
    return nextComment();
  }

  return nextStmtExec();
}

auto ParserImpl::nextExpr() -> NodePtr {
  if (peekToken(0).isEnd()) {
    return nullptr;
  }
  return nextExpr(0);
}

auto ParserImpl::nextComment() -> NodePtr { return commentNode(consumeToken()); }

auto ParserImpl::nextStmtFuncDecl() -> NodePtr {
  auto kw       = consumeToken();
  auto id       = consumeToken();
  auto typeSubs = peekToken(0).getKind() == lex::TokenKind::SepOpenCurly
      ? std::optional<TypeSubstitutionList>{nextTypeSubstitutionList()}
      : std::nullopt;
  auto argList = nextArgDeclList();
  auto retType = std::optional<FuncDeclStmtNode::RetTypeSpec>{};
  if (peekToken(0).getKind() == lex::TokenKind::SepArrow) {
    auto arrow = consumeToken();
    auto type  = nextType();
    retType    = FuncDeclStmtNode::RetTypeSpec{arrow, std::move(type)};
  }
  auto body = nextExpr(0);

  auto typeSubsValid = !typeSubs || typeSubs->validate();
  auto idValid =
      id.getKind() == lex::TokenKind::Identifier || id.getCat() == lex::TokenCat::Operator;
  auto retTypeValid = !retType ||
      (retType->getArrow().getKind() == lex::TokenKind::SepArrow && retType->getType().validate());

  if (getKw(kw) == lex::Keyword::Fun && idValid && typeSubsValid && retTypeValid &&
      argList.validate()) {
    return funcDeclStmtNode(
        kw,
        std::move(id),
        std::move(typeSubs),
        std::move(argList),
        std::move(retType),
        std::move(body));
  }
  return errInvalidStmtFuncDecl(
      kw, std::move(id), std::move(typeSubs), argList, std::move(retType), std::move(body));
}

auto ParserImpl::nextStmtStructDecl() -> NodePtr {
  auto kw       = consumeToken();
  auto id       = consumeToken();
  auto typeSubs = peekToken(0).getKind() == lex::TokenKind::SepOpenCurly
      ? std::optional<TypeSubstitutionList>{nextTypeSubstitutionList()}
      : std::nullopt;
  auto isEmpty = peekToken(0).getKind() != lex::TokenKind::OpEq;
  auto eq      = isEmpty ? std::nullopt : std::optional{consumeToken()};
  auto fields  = std::vector<StructDeclStmtNode::FieldSpec>{};
  auto commas  = std::vector<lex::Token>{};
  if (!isEmpty) {
    while (peekToken(0).getKind() == lex::TokenKind::Identifier) {
      auto fieldType = nextType();
      auto fieldId   = consumeToken();
      fields.emplace_back(std::move(fieldType), std::move(fieldId));
      if (peekToken(0).getKind() == lex::TokenKind::SepComma) {
        commas.push_back(consumeToken());
      } else {
        break;
      }
    }
  }

  auto isValid = getKw(kw) == lex::Keyword::Struct && id.getKind() == lex::TokenKind::Identifier;
  if (typeSubs) {
    isValid &= typeSubs->validate();
  }
  if (!isEmpty) {
    isValid &= !fields.empty() &&
        std::all_of(
            fields.begin(),
            fields.end(),
            [](const auto& a) {
              return a.getIdentifier().getKind() == lex::TokenKind::Identifier &&
                  a.getType().validate();
            }) &&
        (commas.size() == fields.size() - 1);
  }

  if (isValid) {
    return structDeclStmtNode(
        kw, std::move(id), std::move(typeSubs), eq, std::move(fields), std::move(commas));
  }
  return errInvalidStmtStructDecl(kw, id, std::move(typeSubs), eq, fields, std::move(commas));
}

auto ParserImpl::nextStmtUnionDecl() -> NodePtr {
  auto kw       = consumeToken();
  auto id       = consumeToken();
  auto typeSubs = peekToken(0).getKind() == lex::TokenKind::SepOpenCurly
      ? std::optional<TypeSubstitutionList>{nextTypeSubstitutionList()}
      : std::nullopt;
  auto eq     = consumeToken();
  auto types  = std::vector<Type>{};
  auto commas = std::vector<lex::Token>{};
  while (peekToken(0).getKind() == lex::TokenKind::Identifier) {
    types.push_back(nextType());
    if (peekToken(0).getKind() == lex::TokenKind::SepComma) {
      commas.push_back(consumeToken());
    } else {
      break;
    }
  }

  const auto kwValid       = getKw(kw) == lex::Keyword::Union;
  const auto idValid       = id.getKind() == lex::TokenKind::Identifier;
  const auto typeSubsValid = !typeSubs || typeSubs->validate();
  const auto eqValid       = eq.getKind() == lex::TokenKind::OpEq;
  const auto typesValid =
      std::all_of(types.begin(), types.end(), [](const auto& t) { return t.validate(); });

  if (kwValid && idValid && typeSubsValid && eqValid && types.size() >= 2 && typesValid &&
      commas.size() == types.size() - 1) {
    return unionDeclStmtNode(
        kw, std::move(id), std::move(typeSubs), eq, std::move(types), std::move(commas));
  }
  return errInvalidStmtUnionDecl(
      kw, std::move(id), std::move(typeSubs), eq, types, std::move(commas));
}

auto ParserImpl::nextStmtEnumDecl() -> NodePtr {
  auto kw      = consumeToken();
  auto id      = consumeToken();
  auto eq      = consumeToken();
  auto entries = std::vector<EnumDeclStmtNode::EntrySpec>{};
  auto commas  = std::vector<lex::Token>{};
  while (peekToken(0).getKind() == lex::TokenKind::Identifier) {
    auto entryId    = consumeToken();
    auto entryValue = std::optional<EnumDeclStmtNode::ValueSpec>{};

    // Allow consume value specifiers that use '=' instead of ':', reason is that its common in
    // other languages and this way we can provide a proper error message in that case.
    if (peekToken(0).getKind() == lex::TokenKind::SepColon ||
        peekToken(0).getKind() == lex::TokenKind::OpEq) {
      auto colon = consumeToken();
      auto minus = peekToken(0).getKind() == lex::TokenKind::OpMinus ? std::optional{consumeToken()}
                                                                     : std::nullopt;
      auto value = consumeToken();
      entryValue = EnumDeclStmtNode::ValueSpec{colon, std::move(minus), std::move(value)};
    }
    entries.emplace_back(EnumDeclStmtNode::EntrySpec{std::move(entryId), std::move(entryValue)});
    if (peekToken(0).getKind() == lex::TokenKind::SepComma) {
      commas.push_back(consumeToken());
    } else {
      break;
    }
  }

  const auto kwValid = getKw(kw) == lex::Keyword::Enum;
  const auto idValid = id.getKind() == lex::TokenKind::Identifier;
  const auto eqValid = eq.getKind() == lex::TokenKind::OpEq;
  const auto entriesValid =
      std::all_of(entries.begin(), entries.end(), [](const auto& e) { return e.validate(); });
  const auto commasValid = commas.size() == entries.size() - 1;

  if (kwValid && idValid && eqValid && !entries.empty() && entriesValid && commasValid) {
    return enumDeclStmtNode(kw, std::move(id), eq, std::move(entries), std::move(commas));
  }
  return errInvalidStmtEnumDecl(kw, std::move(id), eq, entries, std::move(commas));
}

auto ParserImpl::nextStmtExec() -> NodePtr {
  auto action = consumeToken();
  auto open   = consumeToken();
  auto empty  = open.getKind() == lex::TokenKind::OpParenParen;

  auto args   = std::vector<NodePtr>{};
  auto commas = std::vector<lex::Token>{};
  if (!empty) {
    while (peekToken(0).getKind() != lex::TokenKind::SepCloseParen && !peekToken(0).isEnd()) {
      args.push_back(nextExpr(0));
      if (peekToken(0).getKind() == lex::TokenKind::SepComma) {
        commas.push_back(consumeToken());
      }
    }
  }
  auto close = empty ? open : consumeToken();

  const auto idValid     = action.getKind() == lex::TokenKind::Identifier;
  const auto commasValid = commas.size() == (args.empty() ? 0 : args.size() - 1);
  if (idValid && validateParentheses(open, close) && commasValid) {
    return execStmtNode(std::move(action), open, std::move(args), std::move(commas), close);
  }
  return errInvalidStmtExec(std::move(action), open, std::move(args), std::move(commas), close);
}

auto ParserImpl::nextExpr(const int minPrecedence) -> NodePtr {
  auto lhs = nextExprLhs();
  while (true) {
    // Handle binary operators, precedence controls if we should keep recursing or let the next
    // iteration handle them.
    const auto& nextToken       = peekToken(0);
    const auto rhsPrecedence    = getRhsOpPrecedence(nextToken);
    const auto rightAssociative = isRightAssociative(nextToken);
    if (rhsPrecedence == 0 || rhsPrecedence < minPrecedence ||
        (!rightAssociative && rhsPrecedence == minPrecedence)) {
      break;
    }

    switch (nextToken.getKind()) {
    case lex::TokenKind::SepOpenParen:
    case lex::TokenKind::OpParenParen:
      lhs = nextExprCall(std::move(lhs));
      break;
    case lex::TokenKind::OpSemi:
      lhs = nextExprGroup(std::move(lhs), rhsPrecedence);
      break;
    case lex::TokenKind::OpQMark:
      lhs = nextExprConditional(std::move(lhs));
      break;
    case lex::TokenKind::OpDot:
      lhs = nextExprField(std::move(lhs));
      break;
    case lex::TokenKind::SepOpenSquare:
      lhs = nextExprIndex(std::move(lhs));
      break;
    case lex::TokenKind::Keyword:
      if (getKw(nextToken) == lex::Keyword::Is) {
        lhs = nextExprIs(std::move(lhs));
        break;
      }
      [[fallthrough]];
    default:
      auto op  = consumeToken();
      auto rhs = nextExpr(rhsPrecedence);
      lhs      = binaryExprNode(std::move(lhs), op, std::move(rhs));
      break;
    }
  }
  return lhs;
}

auto ParserImpl::nextExprLhs() -> NodePtr {
  const auto& nextToken = peekToken(0);
  if (nextToken.getCat() == lex::TokenCat::Operator) {
    const auto opToken    = consumeToken();
    const auto precedence = getLhsOpPrecedence(opToken);
    if (precedence == 0) {
      return errInvalidUnaryOp(opToken, nextExpr(precedence));
    }
    return unaryExprNode(opToken, nextExpr(precedence));
  }

  return nextExprPrimary();
}

auto ParserImpl::nextExprGroup(NodePtr firstExpr, const int precedence) -> NodePtr {
  auto subExprs = std::vector<NodePtr>{};
  auto semis    = std::vector<lex::Token>{};

  subExprs.push_back(std::move(firstExpr));
  while (peekToken(0).getKind() == lex::TokenKind::OpSemi) {
    semis.push_back(consumeToken());
    subExprs.push_back(nextExpr(precedence));
  }

  return groupExprNode(std::move(subExprs), std::move(semis));
}

auto ParserImpl::nextExprPrimary() -> NodePtr {
  auto nextTok = peekToken(0);
  switch (nextTok.getCat()) {
  case lex::TokenCat::Literal:
    return litExprNode(consumeToken());
  case lex::TokenCat::Identifier: {
    auto id = consumeToken();
    if (peekToken(0).getKind() == lex::TokenKind::OpEq) {
      auto eq = consumeToken();
      return constDeclExprNode(std::move(id), eq, nextExpr(assignmentPrecedence));
    }
    auto typeParams = peekToken(0).getKind() == lex::TokenKind::SepOpenCurly
        ? std::optional<TypeParamList>{nextTypeParamList()}
        : std::nullopt;
    if (!typeParams || typeParams->validate()) {
      return idExprNode(std::move(id), std::move(typeParams));
    }
    return errInvalidIdExpr(std::move(id), std::move(typeParams));
  }
  case lex::TokenCat::Keyword:
    if (getKw(nextTok) == lex::Keyword::If) {
      return nextExprSwitch();
    }
    if (getKw(nextTok) == lex::Keyword::Lambda) {
      return nextExprAnonFunc();
    }
    [[fallthrough]];
  default:
    if (nextTok.getKind() == lex::TokenKind::SepOpenParen) {
      return nextExprParen();
    }
    return errInvalidPrimaryExpr(consumeToken());
  }
}

auto ParserImpl::nextExprField(NodePtr lhs) -> NodePtr {
  auto dot = consumeToken();
  auto id  = consumeToken();

  if (dot.getKind() == lex::TokenKind::OpDot && id.getKind() == lex::TokenKind::Identifier) {
    return fieldExprNode(std::move(lhs), std::move(dot), std::move(id));
  }
  return errInvalidFieldExpr(std::move(lhs), std::move(dot), std::move(id));
}

auto ParserImpl::nextExprIs(NodePtr lhs) -> NodePtr {
  auto kw   = consumeToken();
  auto type = nextType();
  auto id   = consumeToken();

  const auto idValid =
      id.getKind() == lex::TokenKind::Identifier || id.getKind() == lex::TokenKind::Discard;

  if (getKw(kw) == lex::Keyword::Is && type.validate() && idValid) {
    return isExprNode(std::move(lhs), kw, std::move(type), std::move(id));
  }
  return errInvalidIsExpr(std::move(lhs), kw, type, std::move(id));
}

auto ParserImpl::nextExprCall(NodePtr lhs) -> NodePtr {
  auto open  = consumeToken();
  auto empty = open.getKind() == lex::TokenKind::OpParenParen;

  auto args   = std::vector<NodePtr>{};
  auto commas = std::vector<lex::Token>{};
  if (!empty) {
    while (peekToken(0).getKind() != lex::TokenKind::SepCloseParen && !peekToken(0).isEnd()) {
      args.push_back(nextExpr(0));
      if (peekToken(0).getKind() == lex::TokenKind::SepComma) {
        commas.push_back(consumeToken());
      }
    }
  }
  auto close = empty ? open : consumeToken();

  const auto commasValid = commas.size() == (args.empty() ? 0 : args.size() - 1);
  if (validateParentheses(open, close) && commasValid) {
    return callExprNode(std::move(lhs), open, std::move(args), std::move(commas), close);
  }
  return errInvalidCallExpr(std::move(lhs), open, std::move(args), std::move(commas), close);
}

auto ParserImpl::nextExprIndex(NodePtr lhs) -> NodePtr {
  auto open   = consumeToken();
  auto args   = std::vector<NodePtr>{};
  auto commas = std::vector<lex::Token>{};
  while (peekToken(0).getKind() != lex::TokenKind::SepCloseSquare && !peekToken(0).isEnd()) {
    args.push_back(nextExpr(0));
    if (peekToken(0).getKind() == lex::TokenKind::SepComma) {
      commas.push_back(consumeToken());
    }
  }
  auto close = consumeToken();

  const auto openValid   = open.getKind() == lex::TokenKind::SepOpenSquare;
  const auto closeValid  = close.getKind() == lex::TokenKind::SepCloseSquare;
  const auto commasValid = commas.size() == args.size() - 1;
  if (openValid && closeValid && !args.empty() && commasValid) {
    return indexExprNode(std::move(lhs), open, std::move(args), std::move(commas), close);
  }
  return errInvalidIndexExpr(std::move(lhs), open, std::move(args), std::move(commas), close);
}

auto ParserImpl::nextExprConditional(NodePtr condExpr) -> NodePtr {
  auto qmark      = consumeToken();
  auto ifBranch   = nextExpr(groupingPrecedence);
  auto colon      = consumeToken();
  auto elseBranch = nextExpr(groupingPrecedence);

  if (qmark.getKind() == lex::TokenKind::OpQMark && colon.getKind() == lex::TokenKind::SepColon) {
    return conditionalExprNode(
        std::move(condExpr), qmark, std::move(ifBranch), colon, std::move(elseBranch));
  }
  return errInvalidConditionalExpr(
      std::move(condExpr), qmark, std::move(ifBranch), colon, std::move(elseBranch));
}

auto ParserImpl::nextExprParen() -> NodePtr {
  auto open  = consumeToken();
  auto expr  = nextExpr(0);
  auto close = consumeToken();

  if (open.getKind() == lex::TokenKind::SepOpenParen &&
      close.getKind() == lex::TokenKind::SepCloseParen) {
    return parenExprNode(open, std::move(expr), close);
  }
  return errInvalidParenExpr(open, std::move(expr), close);
}

auto ParserImpl::nextExprSwitch() -> NodePtr {
  auto ifClauses = std::vector<NodePtr>{};
  do {
    ifClauses.push_back(nextExprSwitchIf());
  } while (getKw(peekToken(0)) == lex::Keyword::If);
  auto elseClause = getKw(peekToken(0)) == lex::Keyword::Else ? nextExprSwitchElse() : nullptr;
  return switchExprNode(std::move(ifClauses), std::move(elseClause));
}

auto ParserImpl::nextExprSwitchIf() -> NodePtr {
  auto kw    = consumeToken();
  auto cond  = nextExpr(0);
  auto arrow = consumeToken();
  auto expr  = nextExpr(0);

  if (getKw(kw) == lex::Keyword::If && arrow.getKind() == lex::TokenKind::SepArrow) {
    return switchExprIfNode(kw, std::move(cond), arrow, std::move(expr));
  }
  return errInvalidSwitchIf(kw, std::move(cond), arrow, std::move(expr));
}

auto ParserImpl::nextExprSwitchElse() -> NodePtr {
  auto kw    = consumeToken();
  auto arrow = consumeToken();
  auto expr  = nextExpr(0);

  if (getKw(kw) == lex::Keyword::Else && arrow.getKind() == lex::TokenKind::SepArrow) {
    return switchExprElseNode(kw, arrow, std::move(expr));
  }
  return errInvalidSwitchElse(kw, arrow, std::move(expr));
}

auto ParserImpl::nextExprAnonFunc() -> NodePtr {
  auto kw      = consumeToken();
  auto argList = nextArgDeclList();
  auto body    = nextExpr(0);

  if (getKw(kw) == lex::Keyword::Lambda && argList.validate()) {
    return anonFuncExprNode(kw, std::move(argList), std::move(body));
  }
  return errInvalidAnonFuncExpr(kw, argList, std::move(body));
}

auto ParserImpl::nextType() -> Type {
  auto id = consumeToken();
  if (peekToken(0).getKind() == lex::TokenKind::SepOpenCurly) {
    return Type{id, nextTypeParamList()};
  }
  return Type{id};
}

auto ParserImpl::nextTypeParamList() -> TypeParamList {
  auto open   = consumeToken();
  auto params = std::vector<Type>{};
  auto commas = std::vector<lex::Token>{};
  while (peekToken(0).getKind() != lex::TokenKind::SepCloseCurly && !peekToken(0).isEnd()) {
    params.push_back(nextType());
    if (peekToken(0).getKind() == lex::TokenKind::SepComma) {
      commas.push_back(consumeToken());
    }
  }
  auto close = consumeToken();

  return TypeParamList(open, std::move(params), std::move(commas), close);
}

auto ParserImpl::nextTypeSubstitutionList() -> TypeSubstitutionList {
  auto open   = consumeToken();
  auto params = std::vector<lex::Token>{};
  auto commas = std::vector<lex::Token>{};
  while (peekToken(0).getKind() != lex::TokenKind::SepCloseCurly && !peekToken(0).isEnd()) {
    params.push_back(consumeToken());
    if (peekToken(0).getKind() == lex::TokenKind::SepComma) {
      commas.push_back(consumeToken());
    }
  }
  auto close = consumeToken();

  return TypeSubstitutionList(open, std::move(params), std::move(commas), close);
}

auto ParserImpl::nextArgDeclList() -> ArgumentListDecl {
  auto open   = consumeToken();
  auto empty  = open.getKind() == lex::TokenKind::OpParenParen;
  auto args   = std::vector<ArgumentListDecl::ArgSpec>{};
  auto commas = std::vector<lex::Token>{};
  if (!empty) {
    while (peekToken(0).getKind() == lex::TokenKind::Identifier ||
           peekToken(0).getKind() == lex::TokenKind::SepComma) {
      auto argType = nextType();
      auto argId   = consumeToken();
      args.emplace_back(argType, argId);
      if (peekToken(0).getKind() == lex::TokenKind::SepComma) {
        commas.push_back(consumeToken());
      }
    }
  }
  auto close = empty ? open : consumeToken();
  return ArgumentListDecl(open, std::move(args), std::move(commas), close);
}

auto ParserImpl::consumeToken() -> lex::Token {
  if (!m_readBuffer.empty()) {
    const auto t = std::move(m_readBuffer.front());
    m_readBuffer.pop_front();
    return t;
  }
  return getFromInput();
}

auto ParserImpl::peekToken(const size_t ahead) -> lex::Token& {
  for (auto i = m_readBuffer.size(); i <= ahead; i++) {
    m_readBuffer.push_back(getFromInput());
  }
  return m_readBuffer[ahead];
}

} // namespace internal

// Explicit instantiations.
template class Parser<lex::Token*, lex::Token*>;
template class Parser<std::vector<lex::Token>::iterator, std::vector<lex::Token>::iterator>;

} // namespace parse
