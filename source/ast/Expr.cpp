//
// Created by ruarq on 06/08/2024.
//

#include <fmt/format.h>

#include <iris/ast/Expr.hpp>
#include <iris/util.hpp>

namespace iris::ast {
  auto range(Expr const &expr) -> SourceRange {
    return std::visit([](auto const &value) { return value.range; }, expr);
  }

  auto to_string(Context const &ctx, Expr const &expr) -> std::string {
    return std::visit(
        Overloaded{
            [&](ValueExpr const &value) -> std::string {
              return std::visit(
                  Overloaded{
                      [&](std::string const &str) -> std::string { return "\"" + str + "\""; },
                      [&](char const &c) -> std::string {
                        return std::string{1, c};
                      },
                      [&](auto const &valuevalue) -> std::string {
                        return std::to_string(valuevalue);
                      },
                  },
                  value.value);
            },
            [&](NameExpr const &name) -> std::string {
              return std::string{name.identifier.string(ctx)};
            },
            [&](UnaryExpr const &unary) -> std::string {
              return fmt::format("({}{})", unary.op.range.literal(ctx),
                                 to_string(ctx, *unary.expr));
            },
            [&](BinaryExpr const &binary) -> std::string {
              return fmt::format("({} {} {})", to_string(ctx, *binary.left),
                                 binary.op.range.literal(ctx), to_string(ctx, *binary.right));
            },
            [&](AssignExpr const &assign) -> std::string {
              return fmt::format("({} = {})", to_string(ctx, *assign.target),
                                 to_string(ctx, *assign.value));
            },
            [&](CallExpr const &call) -> std::string {
              std::string args;
              if (!call.args.empty()) {
                auto itr = call.args.begin();
                args += to_string(ctx, *itr);
                ++itr;
                for (; itr != call.args.end(); ++itr) {
                  args += ", " + to_string(ctx, *itr);
                }
              }
              return fmt::format("({}({}))", to_string(ctx, *call.function), args);
            },
            [&](MemberSelectExpr const &member) -> std::string {
              return fmt::format("({}.{})", to_string(ctx, *member.value),
                                 to_string(ctx, *member.select));
            },
            [&](StructValueExpr const &struct_value) -> std::string {
              std::string args;
              if (!struct_value.args.empty()) {
                auto itr = struct_value.args.begin();
                args += fmt::format("{}: {}", itr->identifier.string(ctx),
                                    to_string(ctx, *itr->expr));
                ++itr;
                for (; itr != struct_value.args.end(); ++itr) {
                  args += fmt::format(", {}: {}", itr->identifier.string(ctx),
                                      to_string(ctx, *itr->expr));
                }
              }
              return fmt::format("{} {{ {} }}", to_string(ctx, *struct_value.expr), args);
            },
            [](auto const &) -> std::string {
              throw std::runtime_error{
                  "iris::ast::to_string(iris::ast::Expr const &) : not implemented"};
            },
        },
        expr);
  }
}  // namespace iris::ast