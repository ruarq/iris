//
// Created by ruarq on 01/08/2024.
//

#include <doctest/doctest.h>

#include <iris/lexer/Scanner.hpp>
#include <string_view>

using namespace iris::lexer;

constexpr static std::string_view code = "fn main(): i32 { ret 0 }";

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
}