//
// Created by ruarq on 01/08/2024.
//

#ifndef IRIS_AST_EXPR_HPP
#define IRIS_AST_EXPR_HPP

#include <memory>
#include <variant>

#include "Identifier.hpp"

namespace iris::ast {
  struct BinaryExpr;
  struct UnaryExpr;
  struct ValueExpr;
  struct NameExpr;

  /**
   * @brief Represents an expression in the ast.
   */
  using Expr = std::variant<ValueExpr, NameExpr, UnaryExpr, BinaryExpr>;
  using ExprPtr = std::unique_ptr<Expr>;

  /**
   * @brief Represents a constant value.
   */
  using Value = std::variant<std::int64_t, std::uint64_t, double, bool, std::string, char>;

  /**
   * @brief Represents a identifier/hame expression in the ast.
   */
  struct NameExpr {
    SourceRange range;
    Identifier identifier;
  };

  /**
   * @brief Represents a constant value expression in the ast.
   */
  struct ValueExpr {
    SourceRange range;
    Value value;
  };

  /**
   * @brief Describes the kind of a unary operation.
   */
  enum class UnaryOp {
    Not,  ///< Logical not (!)
    Neg,  ///< Unary minus, negation (-)
  };

  /**
   * @brief Represents a unary expression in the ast.
   */
  struct UnaryExpr {
    SourceRange range;
    UnaryOp op;
    ExprPtr expr;
  };

  /**
   * @brief Describes the kind of a binary operation.
   */
  enum class BinaryOp {
    /* ARITHMETIC */

    Add,  ///< Addition (+)
    Sub,  ///< Subtraction (-)
    Mul,  ///< Multiplication (*)
    Div,  ///< Division (/)
    Mod,  ///< Modulo, remainder (%)

    /* COMPARISON */

    Lt,   ///< Less than (<)
    LEq,  ///< Less equal (<=)
    Eq,   ///< Equal (==)
    NEq,  ///< Not equal (!=)
    GEq,  ///< Greater equal (>=)
    Gt,   ///< Greater than (>)

    /* LOGICAL */

    And,  ///< Logical and (&&)
    Or,   ///< Logical or (||)

    /* BINARY */

    BAnd,  ///< Binary and (&)
    BOr,   ///< Binary or (|)
    BXor,  ///< Binary exclusive or (^)
    BLs,   ///< Binary left shift (<<)
    BRs,   ///< Binary right shift (>>)
  };

  /**
   * @brief Represents a binary expression in the ast.
   */
  struct BinaryExpr {
    ExprPtr left;
    BinaryOp op;
    ExprPtr right;
  };
}  // namespace iris::ast

#endif  // IRIS_AST_EXPR_HPP
