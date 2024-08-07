//
// Created by ruarq on 02/08/2024.
//

#include <doctest/doctest.h>

#include <iris/parser/Parser.hpp>
#include <iris/util.hpp>

using namespace iris;
using namespace iris::parser;

auto to_string(Context const &ctx, ast::Expr const &expr) -> std::string {
  return std::visit(Overloaded{[&](ast::BinaryExpr const &binary) {
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
  Context context{File{"/DUMMY/PATH", std::move(expr)}};
  Parser parser{context};
  auto ast = parser.parse_expr();
  auto const result = to_string(context, ast);

  INFO("expected: ", expected);
  INFO("result  : ", result);
  CHECK_EQ(result, expected);
}

auto run_expr(std::string expr) -> void {
  Context context{File{"/DUMMY/PATH", std::move(expr)}};
  Parser parser{context};
  [[maybe_unused]] auto _ = parser.parse_expr();
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
  test_parser("a = b", "(a = b)");
  test_parser("a += b", "(a += b)");
  test_parser("a -= b", "(a -= b)");
  test_parser("a *= b", "(a *= b)");
  test_parser("a /= b", "(a /= b)");
  test_parser("a %= b", "(a %= b)");

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

  /* ASSIGNMENT OPERATORS */
  test_parser("a = b = c = 10", "(a = (b = (c = 10)))");
  test_parser("a = b += 10", "(a = (b += 10))");
  test_parser("a = b -= c + 10 = d * 10", "(a = (b -= ((c + 10) = (d * 10))))");

  /* MAKE SURE THE PARSER REJECTS SOME STUFF */
  CHECK_THROWS_WITH(run_expr("a = "), "Error (line 1 column 5): token '' expected expression");
  CHECK_THROWS_WITH(run_expr("= b"), "Error (line 1 column 1): token '=' expected expression");
}

auto to_string(Context const &ctx, ast::Stmt const &stmt) -> std::string {
  return std::visit(
      Overloaded{[&](ast::LetStmt const &let) {
                   return "let " + std::string{let.identifier.string(ctx)} + " = "
                          + to_string(ctx, let.expr);
                 },
                 [&](ast::MutStmt const &mut) {
                   return "mut " + std::string{mut.identifier.string(ctx)} + " = "
                          + to_string(ctx, mut.expr);
                 },
                 [&](ast::ReturnStmt const &ret) { return "ret " + to_string(ctx, ret.expr); },
                 [&](ast::ExprStmt const &expr) { return to_string(ctx, expr.expr); }},
      stmt);
}

auto to_string(Context const &ctx, ast::Block const &block) -> std::string {
  std::string stmts;
  for (auto const &stmt : block) {
    stmts += to_string(ctx, stmt) + "\n";
  }
  return "{\n" + stmts + "}";
}

TEST_CASE("parse_block") {
  Context context{File{"/DUMMY/PATH", "{let a = 10 mut b = 10 + 20 a += b + 10 ret a + b}"}};
  Parser parser{context};
  auto ast = parser.parse_block();
  CHECK_EQ(to_string(context, ast),
           "{\nlet a = 10\nmut b = (10 + 20)\n(a += (b + 10))\nret (a + b)\n}");
}