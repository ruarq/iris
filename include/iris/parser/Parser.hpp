//
// Created by ruarq on 02/08/2024.
//

#ifndef IRIS_PARSER_PARSER_HPP
#define IRIS_PARSER_PARSER_HPP

#include <functional>
#include <iris/Context.hpp>
#include <iris/ast/ast.hpp>
#include <iris/lexer/Lexer.hpp>
#include <optional>

namespace iris::parser {
  /**
   * @brief Represents the "binding level" or "precedence" of an operator.
   */
  enum class Binding {
    Assign = 0,  ///< For assignment operators.
    Init = Assign,
    Or,            ///< Logical or (||)
    And,           ///< Logical and (&&)
    Equality,      ///< Equality operators (==, !=)
    BAnd,          ///< Binary and (&)
    BXor,          ///< Binary exclusive or (^)
    BOr,           ///< Binary or (|)
    Comparison,    ///< Comparison operators (<, <=, >, >=)
    BShift,        ///< Binary shift operators (<<, >>)
    Term,          ///< Term expressions (+, -)
    Factor,        ///< Factor expressions (*, /, %)
    Unary,         ///< Unary operators (!, -)
    Call,          ///< Call function(args...)
    MemberSelect,  ///< Member select/dot operator (.)
    Primary,       ///< Primary expression, like literals, identifiers, etc.
  };

  /**
   * @brief Represents the associativity of an infix operator.
   */
  enum class Associativity {
    None,   ///< Not associative, meaning it can't be "chained"
    Left,   ///< Left associativity, a + b + c == (a + b) + c
    Right,  ///< Right associativity a = b = c == a = (b = c)
  };

  class Parser;

  /**
   * @brief A parse rule used in the pratt parser.
   */
  struct ParseRule {
    /**
     * @brief Nud function for primary expressions.
     */
    using NudFn = std::function<ast::Expr(Parser *)>;

    /**
     * @brief Prefix function for unary expressions.
     */
    using PrefixFn = std::function<ast::Expr(Parser *, Binding)>;

    /**
     * @brief Led function for infix expressions.
     */
    using LedFn = std::function<ast::Expr(Parser *, ast::Expr, Binding)>;

    /**
     * @brief Stores information about prefix operators.
     */
    struct Prefix {
      PrefixFn fn;      ///< The function to run for this prefix operator.
      Binding binding;  ///< The binding strength for this prefix operator.
    };

    std::optional<Prefix> prefix;  ///< The prefix rule.
    NudFn nud;                     ///< The nud function.
    LedFn led;                     ///< The led function.
    ast::BinaryOpKind
        bop;          ///< The kind of binary operator (if this rule represents a binary operator).
    Binding binding;  ///< The binding strength of the binary operator.
    Associativity associativity;  ///< The associativity of the binary operator.
  };

  /**
   * @brief Transforms a stream of tokens into an AST.
   */
  class Parser {
  public:
    /**
     * Construct a new parser object.
     * @param context The context
     */
    explicit Parser(Context &context);

  public:
    /**
     * @brief Parse a module.
     */
    [[nodiscard]] auto parse_module() -> ast::Module;

    /**
     * @brief Parse a declaration.
     */
    [[nodiscard]] auto parse_decl() -> ast::Decl;

    /**
     * @brief Parse a struct declaration.
     */
    [[nodiscard]] auto parse_struct_decl() -> ast::StructDecl;

    /**
     * @brief Parse the fields in a struct declaration.
     */
    [[nodiscard]] auto parse_fields() -> std::vector<ast::Field>;

    /**
     * @brief Parse a field in a struct declaration.
     */
    [[nodiscard]] auto parse_field() -> ast::Field;

    /**
     * @brief Parse a function declaration.
     */
    [[nodiscard]] auto parse_function_decl() -> ast::FunctionDecl;

    /**
     * @brief Parse the signature in a function declaration.
     */
    [[nodiscard]] auto parse_function_signature() -> ast::FunctionSignature;

    /**
     * @brief Parse function parameters.
     */
    [[nodiscard]] auto parse_params() -> std::vector<ast::Param>;

    /**
     * @brief Parse a function parameter.
     */
    [[nodiscard]] auto parse_param() -> ast::Param;

    /**
     * @brief Parse a block of statements.
     */
    [[nodiscard]] auto parse_block() -> ast::Block;

    /**
     * @brief Parse a statement.
     */
    [[nodiscard]] auto parse_stmt() -> ast::Stmt;

    /**
     * @brief Parse a let statement.
     */
    [[nodiscard]] auto parse_let_stmt() -> ast::LetStmt;

    /**
     * @brief Parse a mut statement.
     */
    [[nodiscard]] auto parse_mut_stmt() -> ast::MutStmt;

    /**
     * @brief Parse a return statement.
     */
    [[nodiscard]] auto parse_return_stmt() -> ast::ReturnStmt;

    /**
     * @brief Parse a expression statement.
     */
    [[nodiscard]] auto parse_expr_stmt() -> ast::ExprStmt;

    /**
     * @brief Parse an if-else statement.
     */
    [[nodiscard]] auto parse_if_else_stmt() -> ast::IfElseStmt;

    /**
     * @brief Parse the if-body of an if-else statement.
     */
    [[nodiscard]] auto parse_if_body() -> ast::IfBody;

    /**
     * @brief Parse the else-body of an if-else statement.
     */
    [[nodiscard]] auto parse_else_body() -> ast::ElseBody;

    /**
     * @brief Parse a while statement.
     */
    [[nodiscard]] auto parse_while_stmt() -> ast::WhileStmt;

    /**
     * @brief Parse an expression.
     * @param min_binding The minimum binding strength operators need to have to be parsed in the
     * expression.
     */
    [[nodiscard]] auto parse_expr(Binding min_binding = Binding::Init) -> ast::Expr;

    /**
     * @brief Parse a nud. This will get the parse rule for the current token and call it's nud
     * function.
     */
    [[nodiscard]] auto parse_nud() -> ast::Expr;

    /**
     * @brief Parse a binary expression.
     * @param left The left hand side of the binary expression
     * @param min_binding The minimum binding needed in the right expression
     */
    [[nodiscard]] auto parse_binary_expr(ast::Expr left, Binding min_binding) -> ast::Expr;

    /**
     * @brief Parse a member select expression.
     * @param value The value of the member select expression
     * @param min_binding The minimum binding for the select expression
     */
    [[nodiscard]] auto parse_member_select_expr(ast::Expr value, Binding min_binding) -> ast::Expr;

    /**
     * @brief Parse an assignment expression.
     * @param target The target to assign the value to
     * @param min_binding The minimum binding needed for the value expression
     */
    [[nodiscard]] auto parse_assign_expr(ast::Expr target, Binding min_binding) -> ast::Expr;

    /**
     * @brief Parse a call expression
     * @param callee The callee/function of the call expression.
     * @param min_binding Not used in this function. Exists to match the ParseRule::LedFn signature.
     */
    [[nodiscard]] auto parse_call_expr(ast::Expr callee, Binding min_binding) -> ast::Expr;

    /**
     * @brief Parse the arguments in a call expression.
     */
    [[nodiscard]] auto parse_args() -> std::vector<ast::Expr>;

    /**
     * @brief Parse a unary operator.
     */
    [[nodiscard]] auto parse_unary_op() -> ast::UnaryOp;

    /**
     * @brief Parse a unary expression.
     * @param min_binding The minimum binding needed to parse the expression the operator is being
     * applied to
     */
    [[nodiscard]] auto parse_unary_expr(Binding min_binding) -> ast::Expr;

    /**
     * @brief Parse a primary expression.
     */
    [[nodiscard]] auto parse_primary_expr() -> ast::Expr;

    /**
     * @brief Parse a value/literal expression.
     */
    [[nodiscard]] auto parse_value_expr() -> ast::ValueExpr;

    /**
     * @brief Parse a name expression.
     */
    [[nodiscard]] auto parse_name_expr() -> ast::NameExpr;
    // [[nodiscard]] auto parse_struct_value_expr(ast::Expr expr, Binding min_binding) -> ast::Expr;
    // [[nodiscard]] auto parse_named_args() -> std::vector<ast::NamedArg>;
    // [[nodiscard]] auto parse_named_arg() -> ast::NamedArg;

    /**
     * @brief Parse a *NON-EMPTY* list of identifiers.
     * @note EBNF: identifiers = identifier [',' identifier]*
     * @return a list of identifiers
     */
    [[nodiscard]] auto parse_identifiers() -> std::vector<ast::Identifier>;

    /**
     * @brief Parse an identifier.
     */
    [[nodiscard]] auto parse_identifier() -> ast::Identifier;

    /**
     * @brief Parse a type (annotation)
     */
    [[nodiscard]] auto parse_type() -> ast::TypeNode;

    /**
     * @brief Parse a named type.
     */
    [[nodiscard]] auto parse_name_type() -> ast::TypeNode;

    /**
     * @brief Parse an array type.
     */
    [[nodiscard]] auto parse_array_type() -> ast::TypeNode;

  private:
    /**
     * @brief Consume a token of a specific kind.
     * @param kind The expected kind of the token
     */
    auto consume(lexer::TokenKind kind) -> lexer::Token;

    /**
     * @brief Check if the current token is of a specific kind.
     * @param kind The token kind
     * @return true if the current token is of kind @a kind
     * @return false otherwise
     */
    auto match(lexer::TokenKind kind) -> bool;

    /**
     * @brief Check if the current token matches any of a list of specific kinds.
     * @tparam Ts Must be of type TokenKind
     * @param kinds The token kinds
     * @return true if the current token matches any kind in the list
     * @return false otherwise
     */
    template <typename... Ts>
      requires(std::same_as<Ts, lexer::TokenKind> && ...)
    auto match_any(Ts &&...kinds) -> bool {
      return (match(std::forward<Ts>(kinds)) || ...);
    }

    /**
     * @brief Check if the next tokens match a specific sequence of tokenkinds.
     * @tparam Ts Must be of type TokenKind
     * @param kinds The token kind sequence
     * @return true if all following tokens (including the current one) match the sequence of token
     * kinds
     * @return false otherwise
     */
    template <typename... Ts>
      requires(std::same_as<Ts, lexer::TokenKind> && ...)
    auto match_seq(Ts &&...kinds) -> bool {
      std::size_t i = 0;
      return ((lexer_.peek(i++).kind == kinds) && ...);
    }

    /**
     * @brief Report an error to the user.
     * @note Throws and (hopefully) crashes the compiler. Diagnostics will be implemented at some
     * point.
     * @param range The source range where the error occured
     * @param message A message on why the error occured
     */
    [[noreturn]] auto error(SourceRange const &range, std::string_view message) const -> void;

  private:
    Context &ctx_;        ///< The context being parsed.
    lexer::Lexer lexer_;  ///< The lexer which tokenizes the source.
  };

  /**
   * @brief Parse a context.
   */
  [[nodiscard]] auto parse(Context &context) -> ast::Module;
}  // namespace iris::parser

#endif  // IRIS_PARSER_PARSER_HPP
