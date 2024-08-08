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
  enum class Binding {
    Assign = 0,
    Init = Assign,
    Or,
    And,
    Equality,
    BAnd,
    BXor,
    BOr,
    Comparison,
    BShift,
    Term,
    Factor,
    Unary,
    Call,
    MemberSelect,
    Primary,
  };

  enum class Associativity {
    None,
    Left,
    Right,
  };

  class Parser;

  struct ParseRule {
    using NudFn = std::function<ast::Expr(Parser *)>;
    using PrefixFn = std::function<ast::Expr(Parser *, Binding)>;
    using LedFn = std::function<ast::Expr(Parser *, ast::Expr, Binding)>;

    struct Prefix {
      PrefixFn fn;
      Binding binding;
    };

    std::optional<Prefix> prefix;
    NudFn nud;
    LedFn led;
    ast::BinaryOpKind bop;
    Binding binding;
    Associativity associativity;
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
    [[nodiscard]] auto parse_module() -> ast::Module;
    [[nodiscard]] auto parse_decl() -> ast::Decl;
    [[nodiscard]] auto parse_struct_decl() -> ast::StructDecl;
    [[nodiscard]] auto parse_fields() -> std::vector<ast::Field>;
    [[nodiscard]] auto parse_field() -> ast::Field;
    [[nodiscard]] auto parse_function_decl() -> ast::FunctionDecl;
    [[nodiscard]] auto parse_function_signature() -> ast::FunctionSignature;
    [[nodiscard]] auto parse_params() -> std::vector<ast::Param>;
    [[nodiscard]] auto parse_param() -> ast::Param;

    [[nodiscard]] auto parse_block() -> ast::Block;
    [[nodiscard]] auto parse_stmt() -> ast::Stmt;
    [[nodiscard]] auto parse_let_stmt() -> ast::LetStmt;
    [[nodiscard]] auto parse_mut_stmt() -> ast::MutStmt;
    [[nodiscard]] auto parse_return_stmt() -> ast::ReturnStmt;
    [[nodiscard]] auto parse_expr_stmt() -> ast::ExprStmt;
    [[nodiscard]] auto parse_if_else_stmt() -> ast::IfElseStmt;
    [[nodiscard]] auto parse_if_body() -> ast::IfBody;
    [[nodiscard]] auto parse_else_body() -> ast::ElseBody;
    [[nodiscard]] auto parse_while_stmt() -> ast::WhileStmt;

    [[nodiscard]] auto parse_expr(Binding min_binding = Binding::Init) -> ast::Expr;
    [[nodiscard]] auto parse_nud() -> ast::Expr;
    // [[nodiscard]] auto parse_binary_op() -> ast::BinaryOp;
    [[nodiscard]] auto parse_binary_expr(ast::Expr left, Binding min_binding) -> ast::Expr;
    [[nodiscard]] auto parse_member_select_expr(ast::Expr value, Binding min_binding) -> ast::Expr;
    [[nodiscard]] auto parse_assign_expr(ast::Expr target, Binding min_binding) -> ast::Expr;
    [[nodiscard]] auto parse_call_expr(ast::Expr callee, Binding min_binding) -> ast::Expr;
    [[nodiscard]] auto parse_args() -> std::vector<ast::Expr>;
    // [[nodiscard]] auto parse_binary_expr(ast::Expr left, std::size_t precedence = 0) ->
    // ast::Expr;
    [[nodiscard]] auto parse_unary_op() -> ast::UnaryOp;
    [[nodiscard]] auto parse_unary_expr(Binding min_binding) -> ast::Expr;
    [[nodiscard]] auto parse_primary_expr() -> ast::Expr;
    [[nodiscard]] auto parse_value_expr() -> ast::ValueExpr;
    [[nodiscard]] auto parse_name_expr() -> ast::NameExpr;
    [[nodiscard]] auto parse_struct_value_expr(ast::Expr expr, Binding min_binding) -> ast::Expr;
    [[nodiscard]] auto parse_named_args() -> std::vector<ast::NamedArg>;
    [[nodiscard]] auto parse_named_arg() -> ast::NamedArg;

    /**
     * @brief Parse a *NON-EMPTY* list of identifiers.
     * @note EBNF: identifiers = identifier [',' identifier]*
     * @return a list of identifiers
     */
    [[nodiscard]] auto parse_identifiers() -> std::vector<ast::Identifier>;
    [[nodiscard]] auto parse_identifier() -> ast::Identifier;
    [[nodiscard]] auto parse_type() -> ast::TypeNode;
    [[nodiscard]] auto parse_name_type() -> ast::TypeNode;
    [[nodiscard]] auto parse_array_type() -> ast::TypeNode;

  private:
    auto consume(lexer::TokenKind kind) -> lexer::Token;
    auto match(lexer::TokenKind kind) -> bool;

    template <typename... Ts>
      requires(std::same_as<Ts, lexer::TokenKind> && ...)
    auto match_any(Ts &&...kinds) -> bool {
      return (match(std::forward<Ts>(kinds)) || ...);
    }

    template <typename... Ts>
      requires(std::same_as<Ts, lexer::TokenKind> && ...)
    auto match_seq(Ts &&...kinds) -> bool {
      std::size_t i = 0;
      return ((lexer_.peek(i++).kind == kinds) && ...);
    }

    [[noreturn]] auto error(SourceRange const &range, std::string_view message) const -> void;

  private:
    Context &ctx_;
    lexer::Lexer lexer_;
  };

  /**
   * @brief Parse a context.
   */
  [[nodiscard]] auto parse(Context &context) -> ast::Module;
}  // namespace iris::parser

#endif  // IRIS_PARSER_PARSER_HPP
