#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "parse/error.hpp"
#include "parse/node_expr_anon_func.hpp"

namespace parse {

TEST_CASE("Parsing anonymous functions", "[parse]") {

  CHECK_EXPR(
      "lambda () 1",
      anonFuncExprNode({}, LAMBDA, ArgumentListDecl(OPAREN, {}, COMMAS(0), CPAREN), INT(1)));
  CHECK_EXPR(
      "impure lambda () 1",
      anonFuncExprNode({IMPURE}, LAMBDA, ArgumentListDecl(OPAREN, {}, COMMAS(0), CPAREN), INT(1)));
  CHECK_EXPR(
      "lambda (int x) 1",
      anonFuncExprNode(
          {},
          LAMBDA,
          ArgumentListDecl(
              OPAREN, {ArgumentListDecl::ArgSpec(TYPE("int"), ID("x"))}, COMMAS(0), CPAREN),
          INT(1)));
  CHECK_EXPR(
      "lambda (int x, int y) 1",
      anonFuncExprNode(
          {},
          LAMBDA,
          ArgumentListDecl(
              OPAREN,
              {ArgumentListDecl::ArgSpec(TYPE("int"), ID("x")),
               ArgumentListDecl::ArgSpec(TYPE("int"), ID("y"))},
              COMMAS(1),
              CPAREN),
          INT(1)));
  CHECK_EXPR(
      "lambda (int x, int y, bool z) x * y",
      anonFuncExprNode(
          {},
          LAMBDA,
          ArgumentListDecl(
              OPAREN,
              {ArgumentListDecl::ArgSpec(TYPE("int"), ID("x")),
               ArgumentListDecl::ArgSpec(TYPE("int"), ID("y")),
               ArgumentListDecl::ArgSpec(TYPE("bool"), ID("z"))},
              COMMAS(2),
              CPAREN),
          binaryExprNode(ID_EXPR("x"), STAR, ID_EXPR("y"))));

  SECTION("Errors") {
    CHECK_EXPR(
        "lambda ()",
        anonFuncExprNode(
            {},
            LAMBDA,
            ArgumentListDecl(PARENPAREN, {}, COMMAS(0), PARENPAREN),
            errInvalidPrimaryExpr(END)));
    CHECK_EXPR(
        "lambda",
        errInvalidAnonFuncExpr(
            {}, LAMBDA, ArgumentListDecl(END, {}, COMMAS(0), END), errInvalidPrimaryExpr(END)));
    CHECK_EXPR(
        "lambda (",
        errInvalidAnonFuncExpr(
            {}, LAMBDA, ArgumentListDecl(OPAREN, {}, COMMAS(0), END), errInvalidPrimaryExpr(END)));
    CHECK_EXPR(
        "lambda (int x",
        errInvalidAnonFuncExpr(
            {},
            LAMBDA,
            ArgumentListDecl(
                OPAREN, {ArgumentListDecl::ArgSpec(TYPE("int"), ID("x"))}, COMMAS(0), END),
            errInvalidPrimaryExpr(END)));
    CHECK_EXPR(
        "lambda (int",
        errInvalidAnonFuncExpr(
            {},
            LAMBDA,
            ArgumentListDecl(OPAREN, {ArgumentListDecl::ArgSpec(TYPE("int"), END)}, COMMAS(0), END),
            errInvalidPrimaryExpr(END)));
    CHECK_EXPR(
        "lambda (int x int y)",
        errInvalidAnonFuncExpr(
            {},
            LAMBDA,
            ArgumentListDecl(
                OPAREN,
                {ArgumentListDecl::ArgSpec(TYPE("int"), ID("x")),
                 ArgumentListDecl::ArgSpec(TYPE("int"), ID("y"))},
                COMMAS(0),
                CPAREN),
            errInvalidPrimaryExpr(END)));
    CHECK_EXPR(
        "lambda (int x,,)",
        errInvalidAnonFuncExpr(
            {},
            LAMBDA,
            ArgumentListDecl(
                OPAREN,
                {ArgumentListDecl::ArgSpec(TYPE("int"), ID("x")),
                 ArgumentListDecl::ArgSpec(Type(COMMA), CPAREN)},
                COMMAS(1),
                END),
            errInvalidPrimaryExpr(END)));
    CHECK_EXPR(
        "impure function () 1",
        errInvalidAnonFuncExpr(
            {IMPURE},
            ID("function"),
            ArgumentListDecl(PARENPAREN, {}, COMMAS(0), PARENPAREN),
            INT(1)));
    CHECK_EXPR(
        "impure action () 1",
        errInvalidAnonFuncExpr(
            {IMPURE},
            ID("action"),
            ArgumentListDecl(PARENPAREN, {}, COMMAS(0), PARENPAREN),
            INT(1)));
  }

  SECTION("Spans") {
    CHECK_EXPR_SPAN(" lambda () 1 + 2", input::Span(1, 15));
    CHECK_EXPR_SPAN("lambda (int i) i", input::Span(0, 15));
    CHECK_EXPR_SPAN("impure lambda (int i) i", input::Span(0, 22));
  }
}

} // namespace parse
