//
// Created by ruarq on 08/08/2024.
//

#include <iris/ast/Module.hpp>

namespace iris::ast {
  auto to_string(Context const& ctx, Module const& module) -> std::string {
    std::string result;
    for (auto const& decl : module.decls) {
      result += to_string(ctx, decl) + "\n";
    }
    return result;
  }
}  // namespace iris::ast