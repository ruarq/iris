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
    Type type;                            ///< The type of the parameters
  };

  /**
   * @brief Represents a function signature in the ast.
   * @note EBNF: function-signature = 'fn' identifier '(' param-list ')' [':' type]? ;
   */
  struct FunctionSignature {
    SourceRange range;          ///< The source range of the function signature.
    Identifier identifier;      ///< The identifier of the function.
    std::vector<Param> params;  ///< The parameters of the function.
    Type type;                  ///< The return type of the function.
  };

  /**
   * @brief Represents a function declaration in the ast.
   * @note EBNF: function-decl = function-signature block
   */
  struct FunctionDecl {
    FunctionSignature signature;
    Block block;
  };

  /**
   * @brief Represents a field in a struct declaration.
   * @note EBNF: field = identifier-list ':' type
   */
  struct Field {
    std::vector<Identifier> identifiers;  ///< The identifiers of the fields.
    Type type;                            ///< The type of the fields.
  };

  /**
   * @brief Represents a struct declaration in the ast.
   * @note EBNF: struct-decl = 'struct' identifier '{' field-list '}'
   */
  struct StructDecl {
    SourceRange range;
    Identifier identifier;
    std::vector<Field> fields;
  };
}  // namespace iris::ast

#endif  // IRIS_AST_DECL_HPP
