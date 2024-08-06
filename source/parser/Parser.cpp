//
// Created by ruarq on 02/08/2024.
//

#include <fmt/format.h>

#include <cassert>
#include <iris/parser/Parser.hpp>

#define IRIS_UNREACHABLE(x) (throw std::runtime_error{#x " : unreachable"})

using namespace iris;
using namespace iris::parser;
using enum lexer::TokenKind;

enum class Associativity { Left, Right };

struct BinaryOpInfo {
  ast::BinaryOpKind op;
  std::size_t precedence;
  Associativity associativity;
};

#define DEF(x) [static_cast<std::size_t>(x)]

static auto binary_op_info(lexer::TokenKind const kind) -> std::optional<BinaryOpInfo> {
  using enum ast::BinaryOpKind;
  using enum Associativity;

  constexpr BinaryOpInfo lut[] = {
      DEF(BarBar) = {.op = Or, .precedence = 1, .associativity = Left},
      DEF(AmpersandAmpersand) = {.op = And, .precedence = 2, .associativity = Left},
      DEF(EqualEqual) = {.op = Eq, .precedence = 3, .associativity = Left},
      DEF(ExclamEqual) = {.op = NEq, .precedence = 3, .associativity = Left},
      DEF(Ampersand) = {.op = BAnd, .precedence = 4, .associativity = Left},
      DEF(Caret) = {.op = BXor, .precedence = 5, .associativity = Left},
      DEF(Bar) = {.op = BOr, .precedence = 6, .associativity = Left},
      DEF(LAngle) = {.op = Lt, .precedence = 7, .associativity = Left},
      DEF(RAngle) = {.op = Gt, .precedence = 7, .associativity = Left},
      DEF(LAngleEqual) = {.op = LEq, .precedence = 7, .associativity = Left},
      DEF(RAngleEqual) = {.op = GEq, .precedence = 7, .associativity = Left},
      DEF(LAngleLAngle) = {.op = BLs, .precedence = 8, .associativity = Left},
      DEF(RAngleRAngle) = {.op = BRs, .precedence = 8, .associativity = Left},
      DEF(Plus) = {.op = Add, .precedence = 9, .associativity = Left},
      DEF(Dash) = {.op = Sub, .precedence = 9, .associativity = Left},
      DEF(Asterisk) = {.op = Mul, .precedence = 10, .associativity = Left},
      DEF(Slash) = {.op = Div, .precedence = 10, .associativity = Left},
      DEF(Percent) = {.op = Mod, .precedence = 10, .associativity = Left},
  };

  auto const idx = static_cast<std::size_t>(kind);
  if (idx >= std::size(lut)) {
    return std::nullopt;
  }

  return lut[idx];
}

#undef DEF

auto Parser::to_unary_op(lexer::TokenKind const kind) -> std::optional<ast::UnaryOpKind> {
  using enum ast::UnaryOpKind;

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

auto Parser::parse_expr() -> ast::Expr { return parse_binary_expr(); }

/**
 *       1 + 2  * 3   + 2  == 9
 *         ^    ^     ^    ^
 * p:      1    2     1    0
 * >> (((1 + (2 * 3)) + (2 * 3)) == 9)
 */
auto Parser::parse_binary_expr(std::size_t precedence) -> ast::Expr {
  using enum Associativity;

  auto left = parse_primary_expr();

  auto token = lexer_.current();
  auto info = binary_op_info(token.kind);
  while (info && info->precedence > precedence) {
    auto const op = ast::BinaryOp{token.range, info->op};

    lexer_.consume();
    auto right = parse_primary_expr();

    auto token2 = lexer_.current();
    auto info2 = binary_op_info(token2.kind);
    while (info2
           && (info2->precedence > info->precedence
               || (info2->associativity == Right && info2->precedence == info->precedence))) {
      lexer_.consume();

      auto next_precedence = info->precedence;
      if (info2->precedence > info->precedence) {
        ++next_precedence;
      }
      auto right2 = parse_binary_expr(next_precedence);
      right = ast::BinaryExpr{std::make_unique<ast::Expr>(std::move(right)),
                              ast::BinaryOp{token2.range, info2->op},
                              std::make_unique<ast::Expr>(std::move(right2))};

      token2 = lexer_.current();
      info2 = binary_op_info(token2.kind);
    }

    left = ast::BinaryExpr{std::make_unique<ast::Expr>(std::move(left)), op,
                           std::make_unique<ast::Expr>(std::move(right))};

    token = lexer_.current();
    info = binary_op_info(token.kind);
  }

  return left;
}

auto Parser::parse_unary_op() -> ast::UnaryOp {
  auto const token = lexer_.current();

  auto const result = to_unary_op(token.kind);
  if (!result.has_value()) {
    error(token.range, "expected unary op");
  }

  lexer_.consume();

  return ast::UnaryOp{token.range, result.value()};
}

auto Parser::parse_unary_expr() -> ast::UnaryExpr {
  auto const op = parse_unary_op();
  auto expr = parse_primary_expr();

  // TODO(ruarq): Set correct range for unary op.
  return ast::UnaryExpr{op.range, op, std::make_unique<ast::Expr>(std::move(expr))};
}

auto Parser::parse_primary_expr() -> ast::Expr {
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
      value = static_cast<std::uint64_t>(1);
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

auto parser::parse(Context& context) -> ast::Expr {
  auto lexer = lexer::Lexer::from(context);
  Parser parser{context, lexer};
  return parser.parse_expr();
}
