#pragma once
#include <iostream>

namespace lex {

enum class TokenKind {
  End,
  OpPlus,
  OpMinus,
  OpStar,
  OpSlash,
  OpRem,
  OpAmp,
  OpAmpAmp,
  OpPipe,
  OpPipePipe,
  OpEq,
  OpEqEq,
  OpBang,
  OpBangEq,
  OpLe,
  OpLeEq,
  OpGt,
  OpGtEq,
  OpSemi,
  OpQMark,
  OpDot,
  SepOpenParen,
  SepCloseParen,
  SepOpenCurly,
  SepCloseCurly,
  SepComma,
  SepColon,
  SepArrow,
  LitInt,
  LitFloat,
  LitBool,
  LitString,
  Keyword,
  Identifier,
  Discard,
  Error
};

auto operator<<(std::ostream& out, const TokenKind& rhs) -> std::ostream&;

} // namespace lex
