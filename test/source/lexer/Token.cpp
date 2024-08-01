//
// Created by ruarq on 01/08/2024.
//

#include <doctest/doctest.h>

#include <iris/lexer/Token.hpp>

using namespace iris;
using namespace iris::lexer;

constexpr static std::string_view code = "fn main(): i32 { ret 0 }";

TEST_SUITE("Token") {
  TEST_CASE("literal") {
    File const file{"/DUMMY/PATH", std::string{code}};
    Context context{file};

    SourceRange const range{3, 4, 1, 3};
    Token const main{TokenKind::Identifier, range};

    REQUIRE_EQ(range.literal(code), "main");
    auto const expected = range.literal(code);

    CHECK_EQ(range.literal(context), expected);
    CHECK_EQ(main.literal(context), expected);
  }
}