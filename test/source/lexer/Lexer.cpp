//
// Created by ruarq on 01/08/2024.
//

#include <doctest/doctest.h>

#include <iris/lexer/Lexer.hpp>

using namespace iris;
using namespace iris::lexer;

static constexpr std::string_view code = R"(
  fn main(): i32 {
    let a = 10
    let b = 20
    ret a + b
  }
)";

TEST_CASE("Lexer") {
  File const file{"/DUMMY/PATH", std::string{code}};
  Context context{file};

  Scanner const scanner{file.content()};
  REQUIRE(!scanner.is_eof());

  Tokenizer tokenizer{context, scanner};
  Lexer lexer{tokenizer};
  REQUIRE(!lexer.is_eof());

  while (!lexer.is_eof()) {
    CHECK_EQ(lexer.consume(), tokenizer.next());
  }

  CHECK(lexer.is_eof());
}