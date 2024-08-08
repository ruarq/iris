//
// Created by ruarq on 08/08/2024.
//

#include <fmt/format.h>
#include <fmt/ranges.h>

#include <iris/ast/Dump.hpp>
#include <ranges>
#include <sstream>

#include "iris/util.hpp"

/**
 * Hello, if you read this, please forgive me. I am lazy.
 */

namespace iris::ast {
  Dump::Dump(Context const &ctx, std::ostream &out) : ctx_(ctx), out_(out) {}

  auto Dump::dump(Module const &module) -> void {
    start_node("Module");
    for (auto const &decl : module.decls) {
      dump(decl);
    }
    end_node();
  }

  auto Dump::dump(Decl const &decl) -> void {
    auto dump_function_decl = [this](FunctionDecl const &fn) -> void {
      start_node("Function");
      field(fmt::format("Identifier: {}", fn.signature.identifier.string(ctx_)));
      field(format("Parameters: {}",
                   fmt::join(fn.signature.params
                                 | std::views::transform([this](Param const &param) -> std::string {
                                     return format(
                                         "{}: {}",
                                         fmt::join(param.identifiers
                                                       | std::views::transform(
                                                           [this](Identifier const &identifier)
                                                               -> std::string_view {
                                                             return identifier.string(ctx_);
                                                           }),
                                                   ", "),
                                         to_string(ctx_, param.type));
                                   }),
                             ", ")));
      if (fn.signature.type) {
        field(fmt::format("ReturnType: {}", to_string(ctx_, *fn.signature.type)));
      }
      dump(fn.block);
      end_node();
    };

    auto dump_struct_decl = [this](StructDecl const &struct_decl) -> void {
      start_node("Struct");
      field(fmt::format("Identifier: {}", struct_decl.identifier.string(ctx_)));
      field(format(
          "Fields: {}",
          fmt::join(
              struct_decl.fields | std::views::transform([this](Field const &field) -> std::string {
                return format(
                    "{}: {}",
                    fmt::join(field.identifiers
                                  | std::views::transform(
                                      [this](Identifier const &identifier) -> std::string_view {
                                        return identifier.string(ctx_);
                                      }),
                              ", "),
                    to_string(ctx_, field.type));
              }),
              ", ")));
      end_node();
    };

    std::visit(Overloaded{dump_function_decl, dump_struct_decl}, decl);
  }

  auto Dump::dump(Block const &block) -> void {
    start_node("Block");
    for (auto const &stmt : block) {
      dump(stmt);
    }
    end_node();
  }

  auto Dump::dump(Stmt const &stmt) -> void {
    auto dump_let_stmt = [this](LetStmt const &let) -> void {
      start_node("Let");
      field(fmt::format("Identifier: {}", let.identifier.string(ctx_)));
      label("Expr");
      dump(let.expr);
      end_node();
    };

    auto dump_mut_stmt = [this](MutStmt const &mut) -> void {
      start_node("Mut");
      field(fmt::format("Identifier: {}", mut.identifier.string(ctx_)));
      label("Expr");
      dump(mut.expr);
      end_node();
    };

    auto dump_return_stmt = [this](ReturnStmt const &ret) -> void {
      start_node("Return");
      dump(ret.expr);
      end_node();
    };

    auto dump_expr_stmt = [this](ExprStmt const &expr) -> void {
      start_node("ExprStmt");
      dump(expr.expr);
      end_node();
    };

    auto dump_if_else_stmt = [this](IfElseStmt const &if_else) -> void {
      start_node("IfElse");
      {
        start_node("IfBody");
        label("Condition");
        dump(if_else.then.condition);
        dump(if_else.then.block);
        end_node();

        if (if_else.orelse) {
          start_node("ElseBody");
          dump(if_else.orelse->block);
          end_node();
        }
      }
      end_node();
    };

    auto dump_while_stmt = [this](WhileStmt const &while_stmt) -> void {
      start_node("While");
      label("Condition");
      dump(while_stmt.condition);
      dump(while_stmt.block);
      end_node();
    };

    std::visit(Overloaded{dump_let_stmt, dump_mut_stmt, dump_return_stmt, dump_expr_stmt,
                          dump_if_else_stmt, dump_while_stmt},
               stmt);
  }

  auto Dump::dump(Expr const &expr) -> void {
    auto dump_value_expr = [this](ValueExpr const &expr) -> void {
      start_node("Value");
      field(expr.range.literal(ctx_));
      end_node();
    };

    auto dump_name_expr = [this](NameExpr const &name) -> void {
      start_node("Name");
      field(fmt::format("Identifier: {}", name.identifier.string(ctx_)));
      end_node();
    };

    auto dump_unary_expr = [this](UnaryExpr const &unary) -> void {
      start_node("Unary");
      field(fmt::format("Operator: {}", unary.op.range.literal(ctx_)));
      dump(*unary.expr);
      end_node();
    };

    auto dump_binary_expr = [this](BinaryExpr const &binary) -> void {
      start_node("Binary");
      field(fmt::format("Operator: {}", binary.op.range.literal(ctx_)));
      label("Left");
      dump(*binary.left);
      label("Right");
      dump(*binary.right);
      end_node();
    };

    auto dump_assign_expr = [this](AssignExpr const &assign) -> void {
      start_node("Assign");
      label("Target");
      dump(*assign.target);
      label("Value");
      dump(*assign.value);
      end_node();
    };

    auto dump_member_select_expr = [this](MemberSelectExpr const &member) -> void {
      start_node("MemberSelect");
      label("Value");
      dump(*member.value);
      label("Select");
      dump(*member.select);
      end_node();
    };

    auto dump_call_expr = [this](CallExpr const &call) -> void {
      start_node("Call");
      {
        label("Function");
        dump(*call.function);
        start_node("Args");
        for (auto const &arg : call.args) {
          dump(arg);
        }
        end_node();
      }
      end_node();
    };

    std::visit(
        Overloaded{
            dump_value_expr,
            dump_name_expr,
            dump_unary_expr,
            dump_binary_expr,
            dump_assign_expr,
            dump_member_select_expr,
            dump_call_expr,
            [this](auto const &unknown) -> void {
              start_node("Unknown");
              field(fmt::format("Literal: {}", unknown.range.literal(ctx_)));
              end_node();
            },
        },
        expr);
  }

  auto Dump::start_node(std::string_view const name) -> void {
    write_indent();
    if (!label_.empty()) {
      out_ << label_ << ": ";
      label_.clear();
    }
    out_ << name << "\n";
    indent();
  }

  auto Dump::field(std::string_view const what) -> void {
    write_indent();
    out_ << what << "\n";
  }

  auto Dump::end_node() -> void { dedent(); }

  auto Dump::write_indent() -> void {
    for (std::size_t i = 0; i < indent_; ++i) {
      out_ << "  ";
    }
  }

  auto Dump::indent() -> void { ++indent_; }

  auto Dump::dedent() -> void { --indent_; }

  auto Dump::label(std::string_view const label) -> void { label_ = std::string{label}; }

  auto dump(Context const &ctx, Module const &module) -> std::string {
    std::ostringstream out;
    Dump dump{ctx, out};
    dump.dump(module);
    return out.str();
  }

}  // namespace iris::ast