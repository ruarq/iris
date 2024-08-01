//
// Created by ruarq on 01/08/2024.
//

#include <iris/lexer/Token.hpp>

using namespace iris::lexer;

Token::Token(TokenKind const kind, SourceRange const &range) : kind(kind), range(range) {}

auto Token::literal(Context const &context) const -> std::string_view {
  return range.literal(context);
}

auto Token::operator==(Token const &other) const -> bool {
  return kind == other.kind && range == other.range;
}

auto Token::operator!=(Token const &other) const -> bool { return !(*this == other); }
