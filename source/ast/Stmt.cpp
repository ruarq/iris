//
// Created by ruarq on 06/08/2024.
//

#include <iris/ast/Stmt.hpp>
#include <iris/util.hpp>

namespace iris::ast {
  auto range(Stmt const &stmt) -> SourceRange {
    return std::visit(Overloaded{[](auto const &value) { return value.range; },
                                 [](ExprStmt const &expr) { return range(expr.expr); }},
                      stmt);
  }
}  // namespace iris::ast