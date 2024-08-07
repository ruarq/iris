//
// Created by ruarq on 06/08/2024.
//

#include <iris/ast/Expr.hpp>

namespace iris::ast {
  auto range(Expr const &expr) -> SourceRange {
    return std::visit([](auto const &value) { return value.range; }, expr);
  }
}  // namespace iris::ast