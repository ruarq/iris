#include <doctest/doctest.h>
#include <iris/iris.hpp>

TEST_CASE("iris::hello_world()") {
  REQUIRE_EQ(iris::hello_world(), "Hello, World!\n");
}