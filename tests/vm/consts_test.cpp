#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace vm {

TEST_CASE("Constants", "[vm]") {

  CHECK_EXPR(
      [](backend::Builder* builder) -> void {
        builder->addLoadLitInt(42);
        builder->addStoreConst(0);

        builder->addLoadConst(0);
        builder->addPrintInt();
      },
      "42");

  CHECK_EXPR(
      [](backend::Builder* builder) -> void {
        builder->addLoadLitInt(42);
        builder->addStoreConst(0);

        builder->addLoadLitInt(1337);
        builder->addStoreConst(1);

        builder->addLoadConst(0);
        builder->addPrintInt();

        builder->addLoadConst(1);
        builder->addPrintInt();
      },
      "42",
      "1337");
}

} // namespace vm
