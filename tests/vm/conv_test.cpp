#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace vm {

TEST_CASE("Execute conversions", "[vm]") {

  SECTION("Int to Float") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(42);
          asmb->addConvIntFloat();

          asmb->addConvFloatString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "42");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(-2147483647);
          asmb->addConvIntFloat();

          asmb->addConvFloatString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "-2.14748e+09");
  }

  SECTION("Int to long") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(42);
          asmb->addConvIntLong();

          asmb->addConvLongString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "42");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(-2147483647);
          asmb->addConvIntLong();

          asmb->addConvLongString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "-2147483647");
  }

  SECTION("Long to int") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitLong(42);
          asmb->addConvLongInt();

          asmb->addConvIntString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "42");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitLong(-2147483647);
          asmb->addConvLongInt();

          asmb->addConvIntString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "-2147483647");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitLong(9223372036854775807L);
          asmb->addConvLongInt();

          asmb->addConvIntString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "-1");
  }

  SECTION("Float to Int") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(42.1F); // NOLINT: Magic numbers
          asmb->addConvFloatInt();

          asmb->addConvIntString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "42");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(42.9F); // NOLINT: Magic numbers
          asmb->addConvFloatInt();

          asmb->addConvIntString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "42");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(0.001F); // NOLINT: Magic numbers
          asmb->addConvFloatInt();

          asmb->addConvIntString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "0");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(2147483648.0F); // NOLINT: Magic numbers
          asmb->addConvFloatInt();

          asmb->addConvIntString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "-2147483648"); // Unrepresentable by int, high bit 1 all others 0.
  }

  SECTION("Int to String") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(0);
          asmb->addConvIntString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "0");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(42);
          asmb->addConvIntString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "42");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(2147483647);
          asmb->addConvIntString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "2147483647");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(-2147483647);
          asmb->addConvIntString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "-2147483647");
  }

  SECTION("Long to String") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitLong(0);
          asmb->addConvLongString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "0");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitLong(42);
          asmb->addConvLongString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "42");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitLong(2147483647);
          asmb->addConvLongString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "2147483647");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitLong(-2147483647);
          asmb->addConvLongString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "-2147483647");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitLong(9223372036854775807L);
          asmb->addConvLongString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "9223372036854775807");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitLong(-9223372036854775807L);
          asmb->addConvLongString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "-9223372036854775807");
  }

  SECTION("Float to String") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(0.0F);
          asmb->addConvFloatString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "0");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(42.0F); // NOLINT: Magic numbers
          asmb->addConvFloatString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "42");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(42.1337F); // NOLINT: Magic numbers
          asmb->addConvFloatString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "42.1337");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(0.42F); // NOLINT: Magic numbers
          asmb->addConvFloatString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "0.42");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(999999.0F); // NOLINT: Magic numbers
          asmb->addConvFloatString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "999999");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(99999.1F); // NOLINT: Magic numbers
          asmb->addConvFloatString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "99999.1");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(0.000001F); // NOLINT: Magic numbers
          asmb->addConvFloatString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "1e-06");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(9999999999.0F); // NOLINT: Magic numbers
          asmb->addConvFloatString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "1e+10"); // Rounding error.
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(-0.42F); // NOLINT: Magic numbers
          asmb->addConvFloatString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "-0.42");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(-9999999999.0F); // NOLINT: Magic numbers
          asmb->addConvFloatString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "-1e+10"); // Rounding error.
  }

  SECTION("Char to String") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(48);
          asmb->addConvCharString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "0");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(10);
          asmb->addConvCharString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "\n");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(38);
          asmb->addConvCharString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "&");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(32);
          asmb->addConvCharString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        " ");
  }

  SECTION("Int to Char") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(0);
          asmb->addConvIntChar();

          asmb->addConvIntString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "0");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(42);
          asmb->addConvIntChar();

          asmb->addConvIntString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "42");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(255);
          asmb->addConvIntChar();

          asmb->addConvIntString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "255");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(256);
          asmb->addConvIntChar();

          asmb->addConvIntString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "0");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(-1);
          asmb->addConvIntChar();

          asmb->addConvIntString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "255");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(1337);
          asmb->addConvIntChar();

          asmb->addConvIntString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "57");
  }

  SECTION("Add chars") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt('a');
          asmb->addLoadLitInt('b');
          asmb->addAddChar();

          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "ab");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(256 + 'a');
          asmb->addLoadLitInt(256 + 'b');
          asmb->addAddChar();

          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "ab");
  }
}

} // namespace vm
