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
    Expr expr;  ///< The expression in the expression statement.
  };

  /**
   * @brief Represents a let statement in the ast.
   * @note EBNF: let-stmt = 'let' identifier '=' expr ;
   */
  struct LetStmt {
    SourceRange range;      ///< The range of the let statement.
    Identifier identifier;  ///< The identifier which @a expr is being assigned to.
    Expr expr;              ///< The expression being assigned.
  };

  /**
   * @brief Represents a mut statement in the ast.
   * @note EBNF: mut-stmt = 'mut' identifier '=' expr ;
   */
  struct MutStmt {
    SourceRange range;      ///< The source range of the mut statement.
    Identifier identifier;  ///< The identifier which @a expr is being assigned to.
    Expr expr;              ///< The expression being assigned.
  };

  /**
   * @brief Represents a return statement in the ast.
   * @note EBNF: return-stmt = 'ret' expr ;
   */
  struct ReturnStmt {
    SourceRange range;  ///< The source range of the return statement.
    Expr expr;          ///< The expression being returned.
  };

  /**
   * @brief Represents the if-part of a ifelse statement.
   * @note EBNF: if-body = 'if' expr block
   */
  struct IfBody {
    SourceRange range;  ///< The source range of the if part, only includes 'if' expr
    Expr condition;     ///< The condition based on which block will be run or not.
    Block block;        ///< The block/body of the if part.
  };

  /**
   * @brief Represents the else-part of a ifelse statement.
   * @note EBNF: else-body = 'else' block
   */
  struct ElseBody {
    SourceRange range;  ///< The source range of 'else'
    Block block;        ///< The block being run if the condition in the if-body is false.
  };

  /**
   * @brief Represents an if-else statement in the AST.
   */
  struct IfElseStmt {
    IfBody then;                     ///< The if-body part, contains the condition and the if block.
    std::optional<ElseBody> orelse;  ///< The else-body part, contains the else block.
  };

  /**
   * @brief Represents a while statement in the AST.
   */
  struct WhileStmt {
    SourceRange range;  ///< The source range of the while statement. Includes <'while' expr>
    Expr condition;     ///< The condition based on which the block will be run or not.
    Block block;        ///< The block being run based on whether the condition is true or false.
  };

  /**
   * @brief Get the range of a statement.
   * @param stmt The statement
   * @return the range of the statement
   */
  [[nodiscard]] auto range(Stmt const &stmt) -> SourceRange;

  /**
   * @brief Convert a statement to a string.
   * @param ctx The context the statement belongs to
   * @param stmt The statement
   */
  [[nodiscard]] auto to_string(Context const &ctx, Stmt const &stmt) -> std::string;

  /**
   * @brief Convert a block to a string.
   * @param ctx The context the block belongs to
   * @param block The block
   */
  [[nodiscard]] auto to_string(Context const &ctx, Block const &block) -> std::string;
}  // namespace iris::ast

#endif  // IRIS_AST_STMT_HPP
