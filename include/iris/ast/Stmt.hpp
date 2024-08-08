//
// Created by ruarq on 06/08/2024.
//

#ifndef IRIS_AST_STMT_HPP
#define IRIS_AST_STMT_HPP

#include <iris/SourceRange.hpp>
#include <optional>
#include <variant>

#include "Expr.hpp"
#include "Identifier.hpp"

namespace iris::ast {
  struct ExprStmt;
  struct LetStmt;
  struct MutStmt;
  struct ReturnStmt;
  struct IfElseStmt;
  struct WhileStmt;

  /**
   * @brief Represents a statement in the ast.
   */
  using Stmt = std::variant<ExprStmt, LetStmt, MutStmt, ReturnStmt, IfElseStmt, WhileStmt>;

  /**
   * @brief Represents a block in the ast.
   * @note EBNF: block = '{' stmt-list '}' ;
   */
  using Block = std::vector<Stmt>;

  /**
   * @brief Represents an expression statement in the ast.
   * @note EBNF: expr-stmt = expr ;
   */
  struct ExprStmt {
    Expr expr;
  };

  /**
   * @brief Represents a let statement in the ast.
   * @note EBNF: let-stmt = 'let' identifier '=' expr ;
   */
  struct LetStmt {
    SourceRange range;
    Identifier identifier;
    Expr expr;
  };

  /**
   * @brief Represents a mut statement in the ast.
   * @note EBNF: mut-stmt = 'mut' identifier '=' expr ;
   */
  struct MutStmt {
    SourceRange range;
    Identifier identifier;
    Expr expr;
  };

  /**
   * @brief Represents a return statement in the ast.
   * @note EBNF: return-stmt = 'ret' expr ;
   */
  struct ReturnStmt {
    SourceRange range;
    Expr expr;
  };

  struct IfBody {
    SourceRange range;
    Expr condition;
    Block block;
  };

  struct ElseBody {
    SourceRange range;
    Block block;
  };

  struct IfElseStmt {
    IfBody then;
    std::optional<ElseBody> orelse;
  };

  struct WhileStmt {
    SourceRange range;
    Expr condition;
    Block block;
  };

  /**
   * @brief Get the range of a statement.
   * @param stmt The statement
   * @return the range of the statement
   */
  [[nodiscard]] auto range(Stmt const &stmt) -> SourceRange;
  [[nodiscard]] auto to_string(Context const &ctx, Stmt const &stmt) -> std::string;
  [[nodiscard]] auto to_string(Context const &ctx, Block const &block) -> std::string;
}  // namespace iris::ast

#endif  // IRIS_AST_STMT_HPP
