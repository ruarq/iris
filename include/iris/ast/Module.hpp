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
}  // namespace iris::ast

#endif  // IRIS_AST_MODULE_HPP
