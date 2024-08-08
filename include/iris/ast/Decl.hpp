//
// Created by ruarq on 06/08/2024.
//

#ifndef IRIS_AST_DECL_HPP
#define IRIS_AST_DECL_HPP

#include <variant>
#include <vector>

#include "Identifier.hpp"
#include "Stmt.hpp"
#include "Type.hpp"

namespace iris::ast {
  struct FunctionDecl;
  struct StructDecl;

  /**
   * @brief Represents a declaration in the ast.
   */
  using Decl = std::variant<FunctionDecl, StructDecl>;

  /**
   * @brief Represents (a) param(s) in a function signature.
   * @note EBNF: param = identifier-list ':' type ;
   */
  struct Param {
    std::vector<Identifier> identifiers;  ///< The identifiers/parameters
    TypeNode type;                        ///< The type of the parameters
  };

  /**
   * @brief Represents a function signature in the ast.
   * @note EBNF: function-signature = 'fn' identifier '(' param-list ')' [':' type]? ;
   */
  struct FunctionSignature {
    SourceRange range;             ///< The source range of the function signature.
    Identifier identifier;         ///< The identifier of the function.
    std::vector<Param> params;     ///< The parameters of the function.
    std::optional<TypeNode> type;  ///< The return type of the function.
  };

  /**
   * @brief Represents a function declaration in the ast.
   * @note EBNF: function-decl = function-signature block
   */
  struct FunctionDecl {
    FunctionSignature signature;  ///< The signature of the function.
    Block block;                  ///< The block (body) of the function.
  };

  /**
   * @brief Represents a field in a struct declaration.
   * @note EBNF: field = identifier-list ':' type
   */
  struct Field {
    std::vector<Identifier> identifiers;  ///< The identifiers of the fields.
    TypeNode type;                        ///< The type of the fields.
  };

  /**
   * @brief Represents a struct declaration in the ast.
   * @note EBNF: struct-decl = 'struct' identifier '{' field-list '}'
   */
  struct StructDecl {
    SourceRange range;          ///< The range of the struct declaration. Only includes the `struct
                                ///< identifier` part.
    Identifier identifier;      ///< The name of the struct.
    std::vector<Field> fields;  ///< The fields of the struct.
  };

  [[nodiscard]] auto to_string(Context const &ctx, Decl const &decl) -> std::string;
}  // namespace iris::ast

#endif  // IRIS_AST_DECL_HPP
