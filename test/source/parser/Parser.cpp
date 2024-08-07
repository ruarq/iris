//
// Created by ruarq on 02/08/2024.
//

#include <doctest/doctest.h>

#include <iris/parser/Parser.hpp>
#include <iris/util.hpp>

using namespace iris;
using namespace iris::parser;

auto to_string(Context const &ctx, ast::Expr const &expr) -> std::string {
  return std::visit(
      Overloaded{
          [&](ast::BinaryExpr const &binary) {
            return "(" + to_string(ctx, *binary.left) + " "
                   + std::string{binary.op.range.literal(ctx)} + " " + to_string(ctx, *binary.right)
                   + ")";
          },
          [&](ast::UnaryExpr const &unary) {
            return "(" + std::string{unary.op.range.literal(ctx)} + to_string(ctx, *unary.expr)
                   + ")";
          },
          [&](ast::NameExpr const &name) { return std::string{name.identifier.string(ctx)}; },
          [&](ast::ValueExpr const &value) { return std::string{value.range.literal(ctx)}; },
          [&](ast::AssignExpr const &assign) {
            return "(" + to_string(ctx, *assign.target) + " = " + to_string(ctx, *assign.value)
                   + ")";
          },
          [&](ast::MemberSelectExpr const &member) {
            return "(" + to_string(ctx, *member.value) + "." + to_string(ctx, *member.select) + ")";
          },
          [&](ast::CallExpr const &call) {
            std::string args;
            if (call.args && !call.args->empty()) {
              auto itr = call.args->begin();
              args += to_string(ctx, *itr);
              ++itr;
              while (itr != call.args->end()) {
                args += ", " + to_string(ctx, *itr);
                ++itr;
              }
            }
            return to_string(ctx, *call.function) + "(" + args + ")";
          },
          [](auto const &) -> std::string { return "NOT IMPLEMENTED"; },
      },
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

auto left(std::string const &op) -> void {
  test_parser("a " + op + " b " + op + " c", "((a " + op + " b) " + op + " c)");
}

auto right(std::string const &op) -> void {
  test_parser("a " + op + " b " + op + " c", "(a " + op + " (b " + op + " c))");
}

auto prec_eq(std::string const &left, std::string const &right) -> void {
  test_parser("a " + left + " b " + right + " c", "((a " + left + " b) " + right + " c)");
  test_parser("a " + right + " b " + left + " c", "((a " + right + " b) " + left + " c)");
}

auto prec_gt(std::string const &left, std::string const &right) -> void {
  test_parser("a " + left + " b " + right + " c", "((a " + left + " b) " + right + " c)");
  test_parser("a " + right + " b " + left + " c", "(a " + right + " (b " + left + " c))");
}

auto run_expr(std::string expr) -> void {
  Context context{File{"/DUMMY/PATH", std::move(expr)}};
  Parser parser{context};
  [[maybe_unused]] auto _ = parser.parse_expr();
}

TEST_SUITE("Parser") {
  TEST_CASE("arithmetic operators") {
    test_parser("1 + a", "(1 + a)");
    test_parser("1 - a", "(1 - a)");
    test_parser("1 * b", "(1 * b)");
    test_parser("1 / b", "(1 / b)");
    test_parser("1 % b", "(1 % b)");
  }

  TEST_CASE("binary operators") {
    test_parser("1 & c", "(1 & c)");
    test_parser("1 | c", "(1 | c)");
    test_parser("1 << c", "(1 << c)");
    test_parser("1 >> c", "(1 >> c)");
  }

  TEST_CASE("relational operators") {
    test_parser("abc < abcd", "(abc < abcd)");
    test_parser("h <= j", "(h <= j)");
    test_parser("something > something_else", "(something > something_else)");
    test_parser("1 >= 1", "(1 >= 1)");
  }

  TEST_CASE("equality operators") {
    test_parser("1 == 2", "(1 == 2)");
    test_parser("1234 != 1234.1234", "(1234 != 1234.1234)");
  }

  TEST_CASE("logical operators") {
    test_parser("a && b", "(a && b)");
    test_parser("a || b", "(a || b)");
  }

  TEST_CASE("assignment operators") {
    test_parser("a = b", "(a = b)");
    // test_parser("a += b", "(a += b)");
    // test_parser("a -= b", "(a -= b)");
    // test_parser("a *= b", "(a *= b)");
    // test_parser("a /= b", "(a /= b)");
    // test_parser("a %= b", "(a %= b)");
  }

  TEST_CASE("arithmetic precedence") {
    test_parser("1 + 2 * 3 - 4 / 5 + 6 % 7", "(((1 + (2 * 3)) - (4 / 5)) + (6 % 7))");
  }

  TEST_CASE("arithmetic associativity") {
    left("+");
    left("-");
    left("*");
    left("/");
    left("%");
  }

  TEST_CASE("logical precedence") {
    test_parser("a && b || c", "((a && b) || c)");
    test_parser("a || b && c", "(a || (b && c))");
    test_parser("a && b || c && d", "((a && b) || (c && d))");
  }

  TEST_CASE("logical associativity") {
    left("&&");
    left("||");
  }

  TEST_CASE("binary operator precedence") {
    // >> == <<
    test_parser("a << b >> c", "((a << b) >> c)");
    test_parser("a >> b << c", "((a >> b) << c)");

    // & < |
    test_parser("a & b | c", "(a & (b | c))");
    test_parser("a | b & c", "((a | b) & c)");
  }

  TEST_CASE("binary operator associativity") {
    left("&");
    left("|");
    left("^");
    left(">>");
    left("<<");
  }

  TEST_CASE("assignment operator precedence") {
    test_parser("a = b + c", "(a = (b + c))");
    test_parser("a = b && c", "(a = (b && c))");
  }

  TEST_CASE("assignment operator associativity") { right("="); }

  TEST_CASE("precedence") {
    // MemberSelect > Call
    test_parser("a.b(c)", "(a.b)(c)");
    test_parser("a(b.c)", "a((b.c))");

    // Call > Factor
    test_parser("a * b(c)", "(a * b(c))");
    test_parser("a(b * c)", "a((b * c))");

    prec_eq("*", "/");
    prec_eq("/", "%");

    // Factor > Term
    prec_gt("*", "+");
    prec_eq("+", "-");

    // Term > BShift
    prec_gt("+", "<<");
    prec_eq("<<", ">>");

    // BShift > Comparison
    prec_gt("<<", "<");
    prec_eq("<", ">");
    prec_eq(">", "<=");
    prec_eq("<=", ">=");

    // Comparison > BOr
    prec_gt("<", "|");

    // BOr > BXor
    prec_gt("|", "^");

    // BXor > BAnd
    prec_gt("^", "&");

    // BAnd > Equality
    prec_gt("&", "==");
    prec_eq("==", "!=");

    // Equality > And
    prec_gt("==", "&&");

    // And > Or
    prec_gt("&&", "||");

    // Or > Assign
    prec_gt("||", "=");
  }

  TEST_CASE("unary operators") {
    test_parser("-a", "(-a)");
    test_parser("!a", "(!a)");
    // test_parser("!!a", "(!(!a))");
    test_parser("!a.b", "(!(a.b))");
  }
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
                 [&](ast::ExprStmt const &expr) { return to_string(ctx, expr.expr); },

                 [](auto const &) -> std::string { return "NOT IMPLEMENTED"; }},
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
  Context context{File{"/DUMMY/PATH", "{let a = 10 mut b = 10 + 20 a = a + b + 10 ret a + b}"}};
  Parser parser{context};
  auto ast = parser.parse_block();
  CHECK_EQ(to_string(context, ast),
           "{\nlet a = 10\nmut b = (10 + 20)\n(a = ((a + b) + 10))\nret (a + b)\n}");
}