//
// Created by ruarq on 02/08/2024.
//

#ifndef IRIS_PARSER_PARSER_HPP
#define IRIS_PARSER_PARSER_HPP

#include <iris/Context.hpp>
#include <iris/ast/ast.hpp>
#include <iris/lexer/Lexer.hpp>
#include <optional>

namespace iris::parser {
  class Parser {
  private:
    [[nodiscard]] static auto to_binary_op(lexer::TokenKind kind) -> std::optional<ast::BinaryOp>;
    [[nodiscard]] static auto to_unary_op(lexer::TokenKind kind) -> std::optional<ast::UnaryOp>;

  public:
    Parser(Context &context, lexer::Lexer &lexer);

  public:
    [[nodiscard]] auto parse_expr() -> ast::Expr;
    [[nodiscard]] auto parse_binary_op() -> ast::BinaryOp;
    [[nodiscard]] auto parse_binary_expr() -> ast::Expr;
    [[nodiscard]] auto parse_unary_op() -> ast::UnaryOp;
    [[nodiscard]] auto parse_unary_expr() -> ast::UnaryExpr;
    [[nodiscard]] auto parse_term_expr() -> ast::Expr;
    [[nodiscard]] auto parse_value_expr() -> ast::ValueExpr;
    [[nodiscard]] auto parse_name_expr() -> ast::NameExpr;

    [[nodiscard]] auto parse_identifier() -> ast::Identifier;

  private:
    auto consume(lexer::TokenKind kind) -> lexer::Token;
    auto match(lexer::TokenKind kind) -> bool;

    template <typename... Ts>
      requires(std::same_as<Ts, lexer::TokenKind> && ...)
    auto match_any(Ts &&...kinds) -> bool {
      return (match(std::forward<Ts>(kinds)) || ...);
    }

    [[noreturn]] auto error(SourceRange const &range, std::string_view message) -> void;

  private:
    Context &ctx_;
    lexer::Lexer &lexer_;
  };
}  // namespace iris::parser

#endif  // IRIS_PARSER_PARSER_HPP
