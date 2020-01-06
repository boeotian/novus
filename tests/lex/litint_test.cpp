#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "lex/error.hpp"

namespace lex {

TEST_CASE("Lexing integer literals", "[lex]") {

  SECTION("Single values") {
    CHECK_TOKENS("0", litIntToken(0));
    CHECK_TOKENS("42", litIntToken(42));
    CHECK_TOKENS("00042", litIntToken(42));
    CHECK_TOKENS("2147483647", litIntToken(2147483647));
  }

  SECTION("Hex values") {
    CHECK_TOKENS("0x", litIntToken(0));
    CHECK_TOKENS("0X", litIntToken(0));
    CHECK_TOKENS("0x0", litIntToken(0));
    CHECK_TOKENS("0X0", litIntToken(0));
    CHECK_TOKENS("0x4", litIntToken(0x4));
    CHECK_TOKENS("0xF", litIntToken(0xF));
    CHECK_TOKENS("0xFF", litIntToken(0xFF));
    CHECK_TOKENS("0x7FFFFFFF", litIntToken(0x7FFFFFFF));
    CHECK_TOKENS("0xf", litIntToken(0xf));
    CHECK_TOKENS("0xFf", litIntToken(0xFf));
    CHECK_TOKENS("0x7FffFfFf", litIntToken(0x7FffFfFf));
    CHECK_TOKENS("0x2A", litIntToken(42));
    CHECK_TOKENS("0x539", litIntToken(1337));
    CHECK_TOKENS("0x1E240", litIntToken(0x1E240));
  }

  SECTION("Sequences") {
    CHECK_TOKENS("0 0", litIntToken(0), litIntToken(0));
    CHECK_TOKENS("1 2 3", litIntToken(1), litIntToken(2), litIntToken(3));
    CHECK_TOKENS("1,2", litIntToken(1), basicToken(TokenKind::SepComma), litIntToken(2));

    CHECK_TOKENS("0x 0x123", litIntToken(0), litIntToken(0x123));
    CHECK_TOKENS("0x1 0x2 0x3", litIntToken(1), litIntToken(2), litIntToken(3));
    CHECK_TOKENS("0xF,0Xf", litIntToken(15), basicToken(TokenKind::SepComma), litIntToken(15));
  }

  SECTION("Digit seperators") {
    CHECK_TOKENS("1_000", litIntToken(1000));
    CHECK_TOKENS("1_2___3", litIntToken(123));

    CHECK_TOKENS("0xF_F_F", litIntToken(0xFFF));
    CHECK_TOKENS("0xA_B___C", litIntToken(0xABC));
  }

  SECTION("Errors") {
    CHECK_TOKENS("2147483648", errLitIntTooBig());
    CHECK_TOKENS("99999999999999999999", errLitIntTooBig());
    CHECK_TOKENS("13a4a2", errLitNumberInvalidChar());
    CHECK_TOKENS("13a4好2", errLitNumberInvalidChar());
    CHECK_TOKENS("13a4\a2", errLitNumberInvalidChar());
    CHECK_TOKENS("0a", errLitNumberInvalidChar());
    CHECK_TOKENS("0_", errLitNumberEndsWithSeperator());

    CHECK_TOKENS("0x8FFFFFFF", errLitIntTooBig());
    CHECK_TOKENS("0xFFFFFFFFFFFFFFFFFF2", errLitIntTooBig());
    CHECK_TOKENS("0x13ga4a2", errLitHexInvalidChar());
    CHECK_TOKENS("0x13a4好2", errLitHexInvalidChar());
    CHECK_TOKENS("0x13a4\a2", errLitHexInvalidChar());
    CHECK_TOKENS("0x_", errLitNumberEndsWithSeperator());
  }

  SECTION("Spans") {
    CHECK_SPANS(" 123 ", input::Span{1, 3});
    CHECK_SPANS(" 123  0", input::Span{1, 3}, input::Span{6});
    CHECK_SPANS("1__2__3", input::Span{0, 6});
    CHECK_SPANS("99999999999999999999", input::Span{0, 19});
    CHECK_SPANS("12a12", input::Span{0, 4});
    CHECK_SPANS("11____", input::Span{0, 5});

    CHECK_SPANS(" 0x1F2F3 ", input::Span{1, 7});
    CHECK_SPANS(" 0xF1A2C3  0x", input::Span{1, 8}, input::Span{11, 12});
    CHECK_SPANS("0xA__B__C", input::Span{0, 8});
    CHECK_SPANS("0x123456789ABCDEF", input::Span{0, 16});
    CHECK_SPANS("0xhelloworld", input::Span{0, 11});
    CHECK_SPANS("0xa____", input::Span{0, 6});
  }
}

} // namespace lex
