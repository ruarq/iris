//
// Created by ruarq on 01/08/2024.
//

#include <doctest/doctest.h>

#include <iris/SourceRange.hpp>
#include <iris/lexer/Scanner.hpp>
#include <string_view>

using namespace iris;
using namespace iris::lexer;

constexpr static std::string_view code = "fn main(): i32 {\n\tret 0\n}\n";

TEST_SUITE("Scanner") {
  TEST_CASE("is_eof") {
    SUBCASE("empty span") {
      Scanner scanner{{}};
      CHECK(scanner.is_eof());
    }

    SUBCASE("read until is_eof") {
      Scanner scanner{code};
      REQUIRE(!scanner.is_eof());

      for (std::size_t i = 0; i < code.size(); ++i) {
        CHECK(!scanner.is_eof());
        scanner.advance();
      }

      CHECK(scanner.is_eof());
    }
  }

  TEST_CASE("peek") {
    Scanner scanner{code};
    REQUIRE(!scanner.is_eof());

    for (std::size_t current = 0; current < code.size(); ++current) {
      CHECK_EQ(code[current], scanner.peek(current));
    }
  }

  TEST_CASE("current") {
    Scanner scanner{code};
    REQUIRE(!scanner.is_eof());

    for (auto current : code) {
      CHECK_EQ(current, scanner.current());
      scanner.advance();
    }
  }

  TEST_CASE("advance") {
    Scanner scanner{code};
    REQUIRE(!scanner.is_eof());

    for (std::size_t i = 0; i < code.size(); ++i) {
      CHECK(!scanner.is_eof());
      scanner.advance();
    }

    CHECK(scanner.is_eof());
  }

  TEST_CASE("consume") {
    Scanner scanner{code};
    REQUIRE(!scanner.is_eof());
    for (auto current : code) {
      CHECK_EQ(current, scanner.consume());
    }
  }

  TEST_CASE("position") {
    Scanner scanner{code};
    REQUIRE(!scanner.is_eof());
    REQUIRE_EQ(scanner.position, SourceRange{});

    std::size_t line = 1;
    std::size_t column = 1;

    for (std::size_t i = 0; i < code.size(); ++i) {
      CHECK_EQ(scanner.position.offset, i);
      CHECK_EQ(scanner.position.size, 1);
      CHECK_EQ(scanner.position.line, line);
      CHECK_EQ(scanner.position.column, column);

      if (code[i] == '\n') {
        ++line;
        column = 0;
      }

      ++column;

      SUBCASE("literal") {
        auto const literal = scanner.position.literal(code);

        REQUIRE_EQ(literal.size(), 1);
        CHECK_EQ(code[i], literal[0]);
      }

      scanner.advance();
    }
  }
}