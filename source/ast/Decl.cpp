//
// Created by ruarq on 08/08/2024.
//

#include <fmt/format.h>

#include <iris/ast/Decl.hpp>
#include <iris/util.hpp>

namespace iris::ast {
  auto to_string(Context const& ctx, Decl const& decl) -> std::string {
    return std::visit(
        Overloaded{
            [&](FunctionDecl const& fn) -> std::string {
              std::string result;
              result += fmt::format("fn {}()", fn.signature.identifier.string(ctx));
              result += to_string(ctx, fn.block);
              return result;
            },
            [&](StructDecl const& struct_decl) -> std::string {
              std::string result = fmt::format("struct {} {{", struct_decl.identifier.string(ctx));
              return result + "}";
            },
            [](auto const&) -> std::string {
              throw std::runtime_error{
                  "iris::ast::to_string(iris::Context const &, iris::ast::Decl const &) : not "
                  "implemented"};
            },
        },
        decl);
  }

}  // namespace iris::ast