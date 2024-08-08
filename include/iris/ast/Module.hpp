//
// Created by ruarq on 06/08/2024.
//

#ifndef IRIS_AST_MODULE_HPP
#define IRIS_AST_MODULE_HPP

#include <vector>

#include "Decl.hpp"

namespace iris::ast {
  /**
   * @brief A module (or translation unit) in the AST.
   */
  struct Module {
    std::vector<Decl> decls;  ///< The declarations in this module.
  };

  /**
   * @brief Convert a module to a string.
   * @param ctx The context the module belongs to
   * @param module The module
   */
  [[nodiscard]] auto to_string(Context const &ctx, Module const &module) -> std::string;
}  // namespace iris::ast

#endif  // IRIS_AST_MODULE_HPP
