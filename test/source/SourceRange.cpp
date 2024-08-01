//
// Created by ruarq on 01/08/2024.
//

#include <doctest/doctest.h>

#include <iris/SourceRange.hpp>
#include <random>

using namespace iris;

template <std::integral T> static auto random(T const from = 0, T const to = 99) -> T {
  static std::random_device device;
  static std::mt19937 engine{device()};
  return std::uniform_int_distribution<T>{from, to}(engine);
}

TEST_SUITE("SourceRange") {
  TEST_CASE("end") {
    for (std::size_t i = 0; i < 1000; ++i) {
      auto const offset = random<std::size_t>();
      auto const size = random<std::size_t>();
      auto const end = offset + size;

      SourceRange source_range{offset, size, 1, 1};
      CHECK_EQ(end, source_range.end());
    }
  }

  TEST_CASE("join, operator+") {
    for (std::size_t i = 0; i < 1000; ++i) {
      auto const expected_offset = random<std::size_t>();
      auto const size = expected_offset + random<std::size_t>();
      auto const expected_line = random<std::size_t>();
      auto const expected_column = random<std::size_t>();

      auto const offset_add = random<std::size_t>();
      auto const size_add = offset_add + random<std::size_t>();
      auto const line_add = random<std::size_t>();
      auto const column_add = random<std::size_t>();

      SourceRange const a{expected_offset, size, expected_line, expected_column};

      SourceRange const b{expected_offset + offset_add, size + size_add, expected_line + line_add,
                          expected_column + column_add};

      auto const expected_size = size + size_add + offset_add;

      auto ab = SourceRange::join(a, b);
      auto ba = SourceRange::join(b, a);

      CHECK_EQ(ab.offset, expected_offset);
      CHECK_EQ(ab.size, expected_size);
      CHECK_EQ(ab.line, expected_line);
      CHECK_EQ(ab.column, expected_column);

      // This would only work if a.offset would correspond to a.line and a.column, which it does
      // when the scanner generates it, but not when we assign values to the attributes randomly.
      // This is why this test will be done in the scanner tests. But we can check if the offset and
      // size are equal.
      // CHECK_EQ(ab, ba);

      CHECK_EQ(ab.offset, ba.offset);
      CHECK_EQ(ab.size, ba.size);

      // Make sure that operator+ does the same thing as join
      CHECK_EQ(ab, a + b);
      CHECK_EQ(ba, b + a);
    }
  }
}