//
// Created by ruarq on 02/08/2024.
//

#include <doctest/doctest.h>

#include <iris/parser/Parser.hpp>

using namespace iris;
using namespace iris::parser;

template <class... Ts> struct overloaded : Ts... {
  using Ts::operator()...;
};

template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

auto to_string(Context const &ctx, ast::Expr const &expr) -> std::string {
  return std::visit(overloaded{[&](ast::BinaryExpr const &binary) {
                                 return "(" + to_string(ctx, *binary.left) + " "
                                        + std::string{binary.op.range.literal(ctx)} + " "
                                        + to_string(ctx, *binary.right) + ")";
                               },
                               [&](ast::UnaryExpr const &unary) {
                                 return "(" + std::string{unary.op.range.literal(ctx)}
                                        + to_string(ctx, *unary.expr) + ")";
                               },
                               [&](ast::NameExpr const &name) {
                                 return std::string{name.identifier.string(ctx)};
                               },
                               [&]([[maybe_unused]] ast::ValueExpr const &value) -> std::string {
                                 return std::string{value.range.literal(ctx)};
                               }},
                    expr);
}

auto test_parser(std::string expr, std::string_view const expected) -> void {
  File file{"/DUMMY/PATH", std::move(expr)};
  Context context{std::move(file)};
  auto ast = parse(context);
  auto const result = to_string(context, ast);

  INFO("expected: ", expected);
  INFO("result  : ", result);
  CHECK_EQ(result, expected);
}

TEST_CASE("parse_expr") {
  /* ALL OPERATORS */
  test_parser("1 + a", "(1 + a)");
  test_parser("1 - a", "(1 - a)");
  test_parser("1 * b", "(1 * b)");
  test_parser("1 / b", "(1 / b)");
  test_parser("1 % b", "(1 % b)");
  test_parser("1 & c", "(1 & c)");
  test_parser("1 | c", "(1 | c)");
  test_parser("1 << c", "(1 << c)");
  test_parser("1 >> c", "(1 >> c)");
  test_parser("abc < abcd", "(abc < abcd)");
  test_parser("h <= j", "(h <= j)");
  test_parser("something > something_else", "(something > something_else)");
  test_parser("1 >= 1", "(1 >= 1)");
  test_parser("1 == 2", "(1 == 2)");
  test_parser("1234 != 1234.1234", "(1234 != 1234.1234)");
  test_parser("a && b", "(a && b)");
  test_parser("a || b", "(a || b)");

  /* ARITHMETIC OPERATORS */
  test_parser("1 + 2 * 3", "(1 + (2 * 3))");
  test_parser("1 * 2 + 3", "((1 * 2) + 3)");
  test_parser("1 * 2 + 3 * 4", "((1 * 2) + (3 * 4))");
  test_parser("1 + 2 * 3 + 4", "((1 + (2 * 3)) + 4)");
  test_parser("1 * 2 / 3 % 4", "(((1 * 2) / 3) % 4)");
  test_parser("1 + 2 * 3 - 4 / 5 + 6 % 7", "(((1 + (2 * 3)) - (4 / 5)) + (6 % 7))");

  /* LOGICAL OPERATORS */
  test_parser("a && b || c", "((a && b) || c)");
  test_parser("a || b && c", "(a || (b && c))");
  test_parser("a && b || c && d", "((a && b) || (c && d))");
  test_parser("a == b && c == d", "((a == b) && (c == d))");

  /* BINARY OPERATORS */
  test_parser("a & b == c", "((a & b) == c)");
  test_parser("a == b & c", "(a == (b & c))");
  test_parser("a & b | c", "(a & (b | c))");
  test_parser("a & c << 4", "(a & (c << 4))");
  test_parser("a | c >> 4", "(a | (c >> 4))");
  test_parser("a | c >> 4 & b", "((a | (c >> 4)) & b)");
}