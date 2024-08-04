//
// Created by ruarq on 02/08/2024.
//

#include <fmt/format.h>

#include <iris/parser/Parser.hpp>

#define IRIS_UNREACHABLE(x) (throw std::runtime_error{#x " : unreachable"})

using namespace iris::parser;
using enum iris::lexer::TokenKind;

auto Parser::to_binary_op(lexer::TokenKind const kind) -> std::optional<ast::BinaryOp> {
  using enum ast::BinaryOp;

  switch (kind) {
    case Plus:
      return Add;
    case Dash:
      return Sub;
    case Asterisk:
      return Mul;
    case Slash:
      return Div;
    case Percent:
      return Mod;

    case LAngle:
      return Lt;
    case LAngleEqual:
      return LEq;
    case EqualEqual:
      return Eq;
    case ExclamEqual:
      return NEq;
    case RAngleEqual:
      return GEq;
    case RAngle:
      return Gt;

    case AmpersandAmpersand:
      return And;
    case BarBar:
      return Or;

    case Ampersand:
      return BAnd;
    case Bar:
      return BOr;
    case Caret:
      return BXor;
    case LAngleLAngle:
      return BLs;
    case RAngleRAngle:
      return BRs;

    default:
      return std::nullopt;
  }
}

auto Parser::to_unary_op(lexer::TokenKind const kind) -> std::optional<ast::UnaryOp> {
  using enum ast::UnaryOp;

  switch (kind) {
    case Dash:
      return Neg;
    case Exclam:
      return Not;
    default:
      return std::nullopt;
  }
}

Parser::Parser(Context& context, lexer::Lexer& lexer) : ctx_(context), lexer_(lexer) {}

auto Parser::parse_expr() -> ast::Expr { return parse_term_expr(); }

auto Parser::parse_binary_op() -> ast::BinaryOp {}

auto Parser::parse_binary_expr() -> ast::Expr {}

auto Parser::parse_unary_op() -> ast::UnaryOp {
  auto const token = lexer_.current();

  auto const result = to_unary_op(token.kind);
  if (!result.has_value()) {
    error(token.range, "expected unary op");
  }

  lexer_.consume();

  return result.value();
}

auto Parser::parse_unary_expr() -> ast::UnaryExpr {
  auto const op = parse_unary_op();
  auto expr = parse_term_expr();
  return ast::UnaryExpr{SourceRange{}, op, std::make_unique<ast::Expr>(std::move(expr))};
}

auto Parser::parse_term_expr() -> ast::Expr {
  auto const token = lexer_.current();

  switch (token.kind) {
    case Lint:
    case Lfloat:
    case Lchar:
    case Lbool:
    case Lstr:
      return parse_value_expr();

    case Exclam:
    case Dash:
      return parse_unary_expr();

    case Identifier:
      return parse_name_expr();

    default:
      error(token.range, "expected expression");
  }
}

auto Parser::parse_value_expr() -> ast::ValueExpr {
  ast::Value value;
  auto const token = lexer_.current();

  switch (token.kind) {
    case Lint:
    case Lfloat:
    case Lchar:
    case Lbool:
    case Lstr:
      value = 1;
      break;

    default:
      error(token.range, "expected literal");
  }

  lexer_.consume();
  return ast::ValueExpr{token.range, std::move(value)};
}

auto Parser::parse_name_expr() -> ast::NameExpr {
  auto const identifier = parse_identifier();
  return ast::NameExpr{identifier.range, identifier};
}

auto Parser::parse_identifier() -> ast::Identifier {
  auto const token = consume(Identifier);
  std::string literal{token.literal(ctx_)};
  auto const id = ctx_.intern(std::move(literal));
  return ast::Identifier{token.range, id};
}

auto Parser::consume(lexer::TokenKind const kind) -> lexer::Token {
  if (auto const token = lexer_.current(); kind != token.kind) {
    error(token.range, "unexpected token");
  }

  return lexer_.consume();
}

auto Parser::match(lexer::TokenKind const kind) -> bool { return lexer_.current().kind == kind; }

auto Parser::error(SourceRange const& range, std::string_view message) -> void {
  throw std::runtime_error{fmt::format("Error (line {} column {}): token '{}' {}", range.line,
                                       range.column, range.literal(ctx_), message)};
}
