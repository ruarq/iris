//
// Created by ruarq on 06/08/2024.
//

#ifndef IRIS_UTIL_HPP
#define IRIS_UTIL_HPP

#include <span>

namespace iris {
  // https://en.cppreference.com/w/cpp/utility/variant/visit
  // helper type for the visitor #4
  template <class... Ts> struct Overloaded : Ts... {
    using Ts::operator()...;
  };
  // explicit deduction guide (not needed as of C++20)
  template <class... Ts> Overloaded(Ts...) -> Overloaded<Ts...>;
}  // namespace iris

#endif  // IRIS_UTIL_HPP
