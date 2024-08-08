//
// Created by ruarq on 06/08/2024.
//

#include <fmt/format.h>

#include <iris/ast/Stmt.hpp>
#include <iris/util.hpp>

namespace iris::ast {
  auto range(Stmt const &stmt) -> SourceRange {
    return std::visit(Overloaded{[](auto const &value) { return value.range; },
                                 [](IfElseStmt const &if_else) { return if_else.then.range; },
                                 [](ExprStmt const &expr) { return range(expr.expr); }},
                      stmt);
  }

  auto to_string(Context const &ctx, Stmt const &stmt) -> std::string {
    return std::visit(
        Overloaded{
            [&](ExprStmt const &expr_stmt) -> std::string {
              return to_string(ctx, expr_stmt.expr);
            },
            [&](ReturnStmt const &return_stmt) -> std::string {
              return fmt::format("ret {}", to_string(ctx, return_stmt.expr));
            },
            [&](LetStmt const &let) -> std::string {
              return fmt::format("let {} = {}", let.identifier.string(ctx),
                                 to_string(ctx, let.expr));
            },
            [&](MutStmt const &mut) -> std::string {
              return fmt::format("mut {} = {}", mut.identifier.string(ctx),
                                 to_string(ctx, mut.expr));
            },
            [&](IfElseStmt const &if_else) -> std::string {
              std::string result = fmt::format("if {} {}", to_string(ctx, if_else.then.condition),
                                               to_string(ctx, if_else.then.block));
              if (if_else.orelse) {
                result += fmt::format(" else {}", to_string(ctx, if_else.orelse->block));
              }
              return result;
            },
            [&](WhileStmt const &while_stmt) -> std::string {
              return fmt::format("while {} {}", to_string(ctx, while_stmt.condition),
                                 to_string(ctx, while_stmt.block));
            },
            [](auto const &) -> std::string {
              throw std::runtime_error{
                  "iris::ast::to_string(iris::Context const &, iris::ast::Stmt const &) "
                  ": not implemented"};
            },
        }  // namespace iris::ast
        ,
        stmt);
  }

  auto to_string(Context const &ctx, Block const &block) -> std::string {
    std::string result = "{";
    if (!block.empty()) {
      auto itr = block.begin();
      result += to_string(ctx, *itr);
      ++itr;
      for (; itr != block.end(); ++itr) {
        result += " " + to_string(ctx, *itr);
      }
    }
    return result + "}";
  }
}  // namespace iris::ast