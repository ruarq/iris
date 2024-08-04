//
// Created by ruarq on 02/08/2024.
//

#include <doctest/doctest.h>

#include <iris/parser/Parser.hpp>

using namespace iris;
using namespace iris::parser;

TEST_CASE("parse_expr") {
  File file{"/DUMMY/PATH", "!-name"};
  Context context{std::move(file)};

  auto lexer = lexer::Lexer::from(context);
  REQUIRE_EQ(lexer.current().kind, lexer::TokenKind::Exclam);

  Parser parser{context, lexer};
  auto expr = parser.parse_expr();
}