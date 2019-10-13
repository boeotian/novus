#pragma once
#include "lex/token.hpp"
#include "parse/node.hpp"
#include "parse/node_error.hpp"
#include <memory>
#include <string>

namespace parse {

auto errLexError(lex::Token errToken) -> NodePtr;

auto errInvalidStmtStart(lex::Token token) -> NodePtr;

} // namespace parse
