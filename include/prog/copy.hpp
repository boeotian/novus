#pragma once
#include "prog/program.hpp"

namespace prog {

auto copyType(const Program& from, Program* to, sym::TypeId id) -> bool;

auto copyFunc(const Program& from, Program* to, sym::FuncId id) -> bool;

} // namespace prog