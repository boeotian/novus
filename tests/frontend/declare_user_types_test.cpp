#include "catch2/catch.hpp"
#include "frontend/diag_defs.hpp"
#include "helpers.hpp"

namespace frontend {

TEST_CASE("Analyzing user-type declarations", "[frontend]") {

  SECTION("Declare basic struct") {
    const auto& output = ANALYZE("struct s = int i");
    REQUIRE(output.isSuccess());
    CHECK(TYPE_EXISTS(output, "s"));
  }

  SECTION("Declare templated struct") {
    const auto& output = ANALYZE("struct tuple{T1, T2} = T1 a, T2 b "
                                 "struct s = tuple{int, string} a, tuple{float, bool} b");
    REQUIRE(output.isSuccess());
    CHECK(TYPE_EXISTS(output, "tuple__int_string"));
    CHECK(TYPE_EXISTS(output, "tuple__float_bool"));
  }

  SECTION("Declare basic union") {
    const auto& output = ANALYZE("union u = int, float");
    REQUIRE(output.isSuccess());
    CHECK(TYPE_EXISTS(output, "u"));
  }

  SECTION("Declare templated union") {
    const auto& output = ANALYZE("union u{T1, T2} = T1, T2 "
                                 "struct s = u{int, string} a, u{float, bool} b");
    REQUIRE(output.isSuccess());
    CHECK(TYPE_EXISTS(output, "u__int_string"));
    CHECK(TYPE_EXISTS(output, "u__float_bool"));
  }

  SECTION("Declare templated struct and union") {
    const auto& output = ANALYZE("struct null "
                                 "union option{T} = T, null "
                                 "struct optTuple{T1, T2} = option{T1} a, option{T2} b "
                                 "union u{T1, T2} = optTuple{T1, T2}, float "
                                 "struct s = u{int, string} a, u{float, bool} b");
    REQUIRE(output.isSuccess());
    CHECK(TYPE_EXISTS(output, "option__int"));
    CHECK(TYPE_EXISTS(output, "option__float"));
    CHECK(TYPE_EXISTS(output, "option__string"));
    CHECK(TYPE_EXISTS(output, "option__bool"));
    CHECK(TYPE_EXISTS(output, "optTuple__int_string"));
    CHECK(TYPE_EXISTS(output, "optTuple__float_bool"));
    CHECK(TYPE_EXISTS(output, "u__int_string"));
    CHECK(TYPE_EXISTS(output, "u__float_bool"));
  }

  SECTION("Diagnostics") {
    CHECK_DIAG("struct int = bool i", errTypeAlreadyDeclared(src, "int", input::Span{7, 9}));
    CHECK_DIAG("union int = int, bool", errTypeAlreadyDeclared(src, "int", input::Span{6, 8}));
    CHECK_DIAG(
        "struct print = bool i", errTypeNameConflictsWithAction(src, "print", input::Span{7, 11}));
    CHECK_DIAG(
        "union print = int, bool",
        errTypeNameConflictsWithAction(src, "print", input::Span{6, 10}));
    CHECK_DIAG(
        "struct s = int i "
        "struct s = bool b",
        errTypeAlreadyDeclared(src, "s", input::Span{24, 24}));
    CHECK_DIAG(
        "union u = int, bool "
        "union u = bool, int",
        errTypeAlreadyDeclared(src, "u", input::Span{26, 26}));
    CHECK_DIAG(
        "union s = int, bool "
        "struct s = bool b",
        errTypeAlreadyDeclared(src, "s", input::Span{27, 27}));
    CHECK_DIAG(
        "union s{T} = T, bool "
        "struct s = bool b",
        errTypeTemplateAlreadyDeclared(src, "s", input::Span{28, 28}));
    CHECK_DIAG(
        "union s{T} = T, bool "
        "struct s{T} = T t",
        errTypeTemplateAlreadyDeclared(src, "s", input::Span{28, 28}));
  }
}

} // namespace frontend
