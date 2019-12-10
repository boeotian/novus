#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace vm {

TEST_CASE("Execute constants", "[vm]") {

  CHECK_EXPR(
      [](backend::Builder* builder) -> void {
        builder->addReserveConsts(1);
        builder->addLoadLitInt(42);
        builder->addStoreConst(0);

        builder->addLoadConst(0);
        builder->addConvIntString();
        builder->addPrintString();
      },
      "42");

  CHECK_EXPR(
      [](backend::Builder* builder) -> void {
        builder->addReserveConsts(2);
        builder->addLoadLitInt(42);
        builder->addStoreConst(0);

        builder->addLoadLitInt(1337);
        builder->addStoreConst(1);

        builder->addLoadConst(0);
        builder->addConvIntString();
        builder->addPrintString();

        builder->addLoadConst(1);
        builder->addConvIntString();
        builder->addPrintString();
      },
      "42",
      "1337");
}

} // namespace vm