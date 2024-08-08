//
// Created by ruarq on 06/08/2024.
//

#ifndef IRIS_AST_MODULE_HPP
#define IRIS_AST_MODULE_HPP

#include <vector>

#include "Decl.hpp"

namespace iris::ast {
  struct Module {
    std::vector<Decl> decls;
  };

  [[nodiscard]] auto to_string(Context const &ctx, Module const &module) -> std::string;
}  // namespace iris::ast

#endif  // IRIS_AST_MODULE_HPP
