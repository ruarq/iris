//
// Created by ruarq on 01/08/2024.
//

#ifndef IRIS_AST_EXPR_HPP
#define IRIS_AST_EXPR_HPP

#include <iris/SourceRange.hpp>
#include <memory>
#include <variant>

#include "Identifier.hpp"

namespace iris::ast {
  struct BinaryExpr;
  struct UnaryExpr;
  struct ValueExpr;
  struct StructValueExpr;
  struct NameExpr;
  struct MemberSelectExpr;
  struct CallExpr;
  struct AssignExpr;

  /**
   * @brief Represents an expression in the ast.
   */
  using Expr = std::variant<ValueExpr, NameExpr, UnaryExpr, BinaryExpr, StructValueExpr,
                            MemberSelectExpr, CallExpr, AssignExpr>;
  using ExprPtr = std::unique_ptr<Expr>;

  /**
   * @brief Represents a constant value.
   */
  using Value = std::variant<std::int64_t, std::uint64_t, long double, bool, std::string, char>;

  /**
   * @brief Represents a identifier/hame expression in the ast.
   * @note EBNF: identifier
   */
  struct NameExpr {
    SourceRange range;
    Identifier identifier;
  };

  /**
   * @brief Represents a constant value expression in the ast.
   * @note EBNF: literal
   */
  struct ValueExpr {
    SourceRange range;
    Value value;
  };

  /**
   * @brief Describes the kind of a unary operation.
   */
  enum class UnaryOpKind {
    Not,  ///< Logical not (!)
    Neg,  ///< Unary minus, negation (-)
  };

  /**
   * @brief Represents a unary operator.
   * @note EBNF: unary-op = '!' | '-' ;
   */
  struct UnaryOp {
    SourceRange range;
    UnaryOpKind kind;
  };

  /**
   * @brief Represents a unary expression in the ast.
   * @note EBNF: unary-expr = unary-op primary-expr ;
   */
  struct UnaryExpr {
    SourceRange range;
    UnaryOp op;
    ExprPtr expr;
  };

  /**
   * @brief Describes the kind of a binary operation.
   */
  enum class BinaryOpKind {
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
   * @brief Represents a binary operator in the ast.
   * @note EBNF: binary-op = '+' | '-' | '*' | ... ;
   */
  struct BinaryOp {
    SourceRange range;
    BinaryOpKind kind;
  };

  /**
   * @brief Represents a binary expression in the ast.
   * @note EBNF: binary-expr = expr binary-op expr
   */
  struct BinaryExpr {
    SourceRange range;
    ExprPtr left;
    BinaryOp op;
    ExprPtr right;
  };

  /**
   * @brief Represents a named arg in a struct value expression.
   * @note EBNF: named-arg = identifier ':' type
   */
  struct NamedArg {
    SourceRange range;
    Identifier identifier;
    ExprPtr expr;
  };

  /**
   * @brief Represents a struct value expression in the ast.
   * @note EBNF: struct-value-expr = identifier '{' named-arg-list '}' ;
   */
  struct StructValueExpr {
    SourceRange range;
    Identifier identifier;
    std::vector<NamedArg> args;
  };

  struct CallExpr {
    SourceRange range;
    ExprPtr function;
    std::unique_ptr<std::vector<Expr>> args;
  };

  struct MemberSelectExpr {
    SourceRange range;
    ExprPtr value;
    ExprPtr select;
  };

  struct AssignExpr {
    SourceRange range;
    ExprPtr target;
    ExprPtr value;
  };

  auto range(Expr const &expr) -> SourceRange;
}  // namespace iris::ast

#endif  // IRIS_AST_EXPR_HPP
