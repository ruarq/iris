//
// Created by ruarq on 08/08/2024.
//

#ifndef IRIS_AST_DUMP_HPP
#define IRIS_AST_DUMP_HPP

#include <iris/Context.hpp>
#include <ostream>

#include "ast.hpp"

namespace iris::ast {
  class Dump {
  public:
    explicit Dump(Context const &ctx, std::ostream &out);

  public:
    auto dump(Module const &module) -> void;
    auto dump(Decl const &decl) -> void;
    auto dump(Block const &block) -> void;
    auto dump(Stmt const &stmt) -> void;
    auto dump(Expr const &expr) -> void;

  private:
    auto start_node(std::string_view name) -> void;
    auto field(std::string_view what) -> void;
    auto end_node() -> void;
    auto write_indent() -> void;
    auto indent() -> void;
    auto dedent() -> void;
    auto label(std::string_view label) -> void;

  private:
    Context const &ctx_;
    std::ostream &out_;
    std::size_t indent_{0};
    std::string label_;
  };

  auto dump(Context const &ctx, Module const &module) -> std::string;
}  // namespace iris::ast

#endif  // IRIS_AST_DUMP_HPP
