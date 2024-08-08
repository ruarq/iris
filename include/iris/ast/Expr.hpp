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
  // struct StructValueExpr;
  struct NameExpr;
  struct MemberSelectExpr;
  struct CallExpr;
  struct AssignExpr;

  /**
   * @brief Represents an expression in the ast.
   */
  using Expr = std::variant<ValueExpr, NameExpr, UnaryExpr, BinaryExpr, /* StructValueExpr, */
                            MemberSelectExpr, CallExpr, AssignExpr>;
  using ExprPtr = std::unique_ptr<Expr>;

  /**
   * @brief Represents a constant value.
   */
  using Value = std::variant<std::uint64_t, long double, bool, std::string, char>;

  /**
   * @brief Represents a identifier/hame expression in the ast.
   * @note EBNF: identifier
   */
  struct NameExpr {
    SourceRange range;      ///< The source range of the name expression.
    Identifier identifier;  ///< The identifier of the name expression.
  };

  /**
   * @brief Represents a constant value expression in the ast.
   * @note EBNF: literal
   */
  struct ValueExpr {
    SourceRange range;  ///< The source range of the value expression.
    Value value;        ///< The value of the value expression.
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
    SourceRange range;  ///< The source range of the unary operator.
    UnaryOpKind kind;   ///< The kind of the unary operator.
  };

  /**
   * @brief Represents a unary expression in the ast.
   * @note EBNF: unary-expr = unary-op primary-expr ;
   */
  struct UnaryExpr {
    SourceRange range;  ///< The source range of the unary expression.
    UnaryOp op;         ///< The operator of the unary expression.
    ExprPtr expr;       ///< The expression the unary operator is being applied to.
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
    SourceRange range;  ///< The range of the binary operator.
    BinaryOpKind kind;  ///< The kind of the binary operator.
  };

  /**
   * @brief Represents a binary expression in the ast.
   * @note EBNF: binary-expr = expr binary-op expr
   */
  struct BinaryExpr {
    SourceRange range;  ///< The source range of the binary expression.
    ExprPtr left;       ///< The left hand side expression of the (infix) binary expression.
    BinaryOp op;        ///< The operator which is being applied to @a left and @a right.
    ExprPtr right;      ///< The right hand side expression of the (infix) binary expression.
  };

#if 0
  /**
   * @brief Represents a named arg in a struct value expression.
   * @note EBNF: named-arg = identifier ':' type
   */
  struct NamedArg {
    SourceRange range; ///< The range of the named argument.
    Identifier identifier; ///< The identifier of the named argument.
    ExprPtr expr; ///<
  };

  /**
   * @brief Represents a struct value expression in the ast.
   * @note EBNF: struct-value-expr = expr '{' named-arg-list '}' ;
   */
  struct StructValueExpr {
    SourceRange range;
    ExprPtr expr;
    std::vector<NamedArg> args;
  };
#endif

  /**
   * @brief Represents a call expression in the AST.
   * @note EBNF: call-expr = expr '(' arg-list `)`
   */
  struct CallExpr {
    SourceRange range;       ///< The source range of the call expression.
    ExprPtr function;        ///< The function (expression) being called.
    std::vector<Expr> args;  ///< The arguments which should be applied to the function.
  };

  /**
   * @brief Represents a member select (aka the '.' operator) in the AST.
   * @note EBNF: member-select-expr = expr '.' expr
   */
  struct MemberSelectExpr {
    SourceRange range;  ///< The source range of the member select expression.
    ExprPtr value;      ///< The value a member should be selected from.
    ExprPtr select;     ///< The value which should be selected.
  };

  /**
   * @brief Represents an assignment expression in the AST.
   * @note EBNF: assign-expr = expr '=' expr ;
   */
  struct AssignExpr {
    SourceRange range;  ///< The source range of the assignment.
    ExprPtr target;  ///< The target of the assignment, i.e. where @a value should be assigned to.
    ExprPtr value;   ///< The value to be assigned to @a target.
  };

  /**
   * @brief Get the source range of any expression.
   * @param expr The expression
   */
  auto range(Expr const &expr) -> SourceRange;

  /**
   * @brief Convert an expression to a string.
   * @note The real question is, if there is a need for this function. Right now, the testing code
   * for the parser uses it. It feels kind of weird to maintain two different algorithms (see
   * iris/ast/Dump.hpp) to convert an AST to a string representation. Especially because the
   * iris/ast/Dump.hpp thing is much better.
   * @param ctx The context the expression belongs to
   * @param expr The expression
   */
  auto to_string(Context const &ctx, Expr const &expr) -> std::string;
}  // namespace iris::ast

#endif  // IRIS_AST_EXPR_HPP
