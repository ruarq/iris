//
// Created by ruarq on 02/08/2024.
//

#include <fmt/format.h>

#include <cassert>
#include <charconv>
#include <iris/parser/Parser.hpp>

#define IRIS_UNREACHABLE(x) (throw std::runtime_error{#x " : unreachable"})

using namespace iris;
using namespace iris::parser;
using enum lexer::TokenKind;

enum class Associativity { Left, Right };

struct BinaryOpInfo {
  ast::BinaryOpKind op;
  std::size_t precedence;
  Associativity associativity;
};

static auto binary_op_info(lexer::TokenKind const kind) -> std::optional<BinaryOpInfo> {
  using enum ast::BinaryOpKind;
  using enum Associativity;

  constexpr std::optional<BinaryOpInfo> lut[] = {
      {/* LParen */},
      {/* RParen */},
      {/* LBracket */},
      {/* RBracket */},
      {/* LCurly */},
      {/* RCurly */},
      {/* Comma */},
      // Dot
      BinaryOpInfo{.op = MemberSelect, .precedence = 12, .associativity = Left},
      {/* Colon */},
      // Plus
      BinaryOpInfo{.op = Add, .precedence = 10, .associativity = Left},
      {/* PlusPlus */},
      // Dash
      BinaryOpInfo{.op = Sub, .precedence = 10, .associativity = Left},
      {/* DashDash */},
      // Asterisk
      BinaryOpInfo{.op = Mul, .precedence = 11, .associativity = Left},
      // Slash
      BinaryOpInfo{.op = Div, .precedence = 11, .associativity = Left},
      // Percent
      BinaryOpInfo{.op = Mod, .precedence = 11, .associativity = Left},
      // LAngleLAngle
      BinaryOpInfo{.op = BLs, .precedence = 9, .associativity = Left},
      // RAngleRAngle
      BinaryOpInfo{.op = BRs, .precedence = 9, .associativity = Left},
      // Ampersand
      BinaryOpInfo{.op = BAnd, .precedence = 5, .associativity = Left},
      // Bar
      BinaryOpInfo{.op = BOr, .precedence = 7, .associativity = Left},
      // Caret
      BinaryOpInfo{.op = BXor, .precedence = 6, .associativity = Left},
      // AmpersandAmpersand
      BinaryOpInfo{.op = And, .precedence = 3, .associativity = Left},
      // BarBar
      BinaryOpInfo{.op = Or, .precedence = 2, .associativity = Left},
      {/* Exclam */},
      // Equal,
      BinaryOpInfo{.op = Assign, .precedence = 1, .associativity = Right},
      // PlusEqual
      BinaryOpInfo{.op = AssignAdd, .precedence = 1, .associativity = Right},
      // DashEqual
      BinaryOpInfo{.op = AssignSub, .precedence = 1, .associativity = Right},
      // AsteriskEqual
      BinaryOpInfo{.op = AssignMul, .precedence = 1, .associativity = Right},
      // SlashEqual
      BinaryOpInfo{.op = AssignDiv, .precedence = 1, .associativity = Right},
      // PercentEqual
      BinaryOpInfo{.op = AssignMod, .precedence = 1, .associativity = Right},
      // TODO: LAngleLAngleEqual
      {},
      // TODO: RAngleRAngleEqual
      {},
      // TODO: AmpersandEqual
      {},
      // TODO: AmpersandAmpersandEqual
      {},
      // TODO: BarEqual
      {},
      // TODO: BarBarEqual
      {},
      // TODO: CaretEqual
      {},
      // LAngle
      BinaryOpInfo{.op = Lt, .precedence = 8, .associativity = Left},
      // LAngleEqual
      BinaryOpInfo{.op = LEq, .precedence = 8, .associativity = Left},
      // EqualEqual
      BinaryOpInfo{.op = Eq, .precedence = 4, .associativity = Left},
      // ExclamEqual
      BinaryOpInfo{.op = NEq, .precedence = 4, .associativity = Left},
      // RAngleEqual
      BinaryOpInfo{.op = GEq, .precedence = 8, .associativity = Left},
      // RAngle
      BinaryOpInfo{.op = Gt, .precedence = 8, .associativity = Left},
      {/* Kfn */},
      {/* Klet */},
      {/* Kmut */},
      {/* Kret */},
      {/* Kstruct */},
      {/* Ti8 */},
      {/* Ti16 */},
      {/* Ti32 */},
      {/* Ti64 */},
      {/* Tu8 */},
      {/* Tu16 */},
      {/* Tu32 */},
      {/* Tu64 */},
      {/* Tisz */},
      {/* Tusz */},
      {/* Tf32 */},
      {/* Tf64 */},
      {/* Tstr */},
      {/* Tbool */},
      {/* Lint */},
      {/* Lfloat */},
      {/* Lbool */},
      {/* Lchar */},
      {/* Lstr */},
      {/* Identifier */},
      {/* EndOfFile */},
      {/* Unknown */},
  };

  static_assert(std::size(lut) == static_cast<std::size_t>(Count),
                "lut.size() != TokenKind::Count");
  return lut[static_cast<std::size_t>(kind)];
}

auto to_unary_op(lexer::TokenKind const kind) -> std::optional<ast::UnaryOpKind> {
  using enum ast::UnaryOpKind;

  switch (kind) {
    case Dash:
      return Neg;
    case Exclam:
      return Not;
    default:
      return std::nullopt;
  }
}

auto is_expr(lexer::TokenKind const kind) -> bool {
  switch (kind) {
    case Identifier:
    case Lbool:
    case Lchar:
    case Lfloat:
    case Lint:
    case Exclam:
    case Dash:
      return true;
    default:
      return false;
  }
}

auto is_stmt(lexer::TokenKind const kind) -> bool {
  switch (kind) {
    case Klet:
    case Kmut:
    case Kret:
      return true;
    default:
      return is_expr(kind);
  }
}

auto is_decl(lexer::TokenKind const kind) -> bool {
  switch (kind) {
    case Kfn:
    case Kstruct:
      return true;
    default:
      return false;
  }
}

Parser::Parser(Context& context) : ctx_(context), lexer_(lexer::Lexer::from(context)) {}

auto Parser::parse_module() -> ast::Module {
  std::vector<ast::Decl> decls;
  while (!lexer_.is_eof()) {
    decls.push_back(parse_decl());
  }
  return ast::Module{std::move(decls)};
}

auto Parser::parse_decl() -> ast::Decl {
  switch (auto const token = lexer_.current(); token.kind) {
    case Kfn:
      return parse_function_decl();
    case Kstruct:
      return parse_struct_decl();
    default:
      error(token.range, "expected declaration");
  }
}

auto Parser::parse_struct_decl() -> ast::StructDecl {
  auto const first = consume(Kstruct);
  auto const identifier = parse_identifier();
  consume(LCurly);
  auto fields = parse_fields();
  auto const last = consume(RCurly);
  auto const range = first.range + last.range;
  return ast::StructDecl{range, identifier, std::move(fields)};
}

auto Parser::parse_fields() -> std::vector<ast::Field> {
  std::vector<ast::Field> fields;

  if (match(Identifier)) {
    fields.push_back(parse_field());
  } else {
    return {};
  }

  while (match(Comma)) {
    lexer_.consume();
    fields.push_back(parse_field());
  }

  return fields;
}

auto Parser::parse_field() -> ast::Field {
  auto identifiers = parse_identifiers();
  consume(Colon);
  auto type = parse_type();
  return ast::Field{std::move(identifiers), std::move(type)};
}

auto Parser::parse_function_decl() -> ast::FunctionDecl {
  auto signature = parse_function_signature();
  auto block = parse_block();
  return ast::FunctionDecl{std::move(signature), std::move(block)};
}

auto Parser::parse_function_signature() -> ast::FunctionSignature {
  auto const token = consume(Kfn);
  auto const identifier = parse_identifier();

  consume(LParen);
  auto params = parse_params();
  auto const range = token.range + consume(RParen).range;

  ast::Type type = ast::BasicType{ast::BasicTypeKind::Unit};
  if (match(Colon)) {
    lexer_.consume();
    type = parse_type();
  }
  return ast::FunctionSignature{range, identifier, std::move(params), std::move(type)};
}

auto Parser::parse_params() -> std::vector<ast::Param> {
  std::vector<ast::Param> params;

  fmt::print("test: {}\n", lexer_.current().literal(ctx_));

  if (match(Identifier)) {
    params.push_back(parse_param());
  } else {
    return {};
  }

  while (match(Comma)) {
    lexer_.consume();
    params.push_back(parse_param());
  }

  return params;
}

auto Parser::parse_param() -> ast::Param {
  auto identifiers = parse_identifiers();
  consume(Colon);
  auto type = parse_type();
  return ast::Param{std::move(identifiers), std::move(type)};
}

auto Parser::parse_block() -> ast::Block {
  ast::Block block;
  consume(LCurly);
  while (is_stmt(lexer_.current().kind)) {
    block.push_back(parse_stmt());
  }
  consume(RCurly);
  return block;
}

auto Parser::parse_stmt() -> ast::Stmt {
  auto const token = lexer_.current();
  switch (token.kind) {
    case Klet:
      return parse_let_stmt();
    case Kmut:
      return parse_mut_stmt();
    case Kret:
      return parse_return_stmt();
    default:
      break;
  }

  return parse_expr_stmt();
}

auto Parser::parse_let_stmt() -> ast::LetStmt {
  auto const token = consume(Klet);
  auto const identifier = parse_identifier();
  consume(Equal);
  auto expr = parse_expr();
  auto const range = token.range + ast::range(expr);
  return ast::LetStmt{range, identifier, std::move(expr)};
}

auto Parser::parse_mut_stmt() -> ast::MutStmt {
  auto const token = consume(Kmut);
  auto const identifier = parse_identifier();
  consume(Equal);
  auto expr = parse_expr();
  auto const range = token.range + ast::range(expr);
  return ast::MutStmt{range, identifier, std::move(expr)};
}

auto Parser::parse_return_stmt() -> ast::ReturnStmt {
  auto const token = consume(Kret);
  auto expr = parse_expr();
  return ast::ReturnStmt{token.range, std::move(expr)};
}

auto Parser::parse_expr_stmt() -> ast::ExprStmt { return ast::ExprStmt{parse_expr()}; }

auto Parser::parse_expr() -> ast::Expr { return parse_binary_expr(parse_primary_expr()); }

auto Parser::parse_binary_expr(ast::Expr left, std::size_t precedence) -> ast::Expr {
  using enum Associativity;

  auto token = lexer_.current();
  auto info = binary_op_info(token.kind);
  while (info && info->precedence >= precedence) {
    auto const op = ast::BinaryOp{token.range, info->op};

    lexer_.consume();
    auto right = parse_primary_expr();

    auto token2 = lexer_.current();
    auto info2 = binary_op_info(token2.kind);
    while (info2
           && (info2->precedence > info->precedence
               || (info2->associativity == Right && info2->precedence == info->precedence))) {
      auto next_precedence = info->precedence;
      if (info2->precedence > info->precedence) {
        ++next_precedence;
      }
      right = parse_binary_expr(std::move(right), next_precedence);

      token2 = lexer_.current();
      info2 = binary_op_info(token2.kind);
    }

    auto const range = ast::range(left) + ast::range(right);
    left = ast::BinaryExpr{range, std::make_unique<ast::Expr>(std::move(left)), op,
                           std::make_unique<ast::Expr>(std::move(right))};

    token = lexer_.current();
    info = binary_op_info(token.kind);
  }

  return left;
}

auto Parser::parse_unary_op() -> ast::UnaryOp {
  auto const token = lexer_.current();

  auto const result = to_unary_op(token.kind);
  if (!result.has_value()) {
    error(token.range, "expected unary op");
  }

  lexer_.consume();

  return ast::UnaryOp{token.range, result.value()};
}

auto Parser::parse_unary_expr() -> ast::UnaryExpr {
  auto const op = parse_unary_op();
  auto expr = parse_primary_expr();

  auto const range = op.range + ast::range(expr);
  return ast::UnaryExpr{range, op, std::make_unique<ast::Expr>(std::move(expr))};
}

auto Parser::parse_primary_expr() -> ast::Expr {
  switch (auto const token = lexer_.current(); token.kind) {
    case Lint:
    case Lfloat:
    case Lchar:
    case Lbool:
    case Lstr:
      return parse_value_expr();

    case Exclam:
    case Dash:
      return parse_unary_expr();

    case Identifier:
      if (match_seq(Identifier, LCurly)) {
        return parse_struct_value_expr();
      }
      return parse_name_expr();

    default:
      error(token.range, "expected expression");
  }
}

auto Parser::parse_value_expr() -> ast::ValueExpr {
  ast::Value value;
  auto const token = lexer_.current();
  auto const literal = token.literal(ctx_);

  switch (token.kind) {
    case Lint: {
      std::int64_t int_value = 0;
      std::from_chars(literal.begin(), literal.end(), int_value);
      value = int_value;
      break;
    }

    case Lfloat: {
      long double ld_value = 0.0;
      std::from_chars(literal.begin(), literal.end(), ld_value);
      value = ld_value;
      break;
    }

    case Lchar:
      assert(literal.size() == 1);
      value = literal[0];
      break;

    case Lbool:
      value = literal == "true";
      break;

    case Lstr:
      value = std::string{literal};
      break;

    default:
      error(token.range, "expected literal");
  }

  lexer_.consume();
  return ast::ValueExpr{token.range, std::move(value)};
}

auto Parser::parse_name_expr() -> ast::NameExpr {
  auto const identifier = parse_identifier();
  return ast::NameExpr{identifier.range, identifier};
}

auto Parser::parse_struct_value_expr() -> ast::StructValueExpr {
  auto const identifier = parse_identifier();
  consume(LCurly);
  auto named_args = parse_named_args();
  auto const range = identifier.range + consume(RCurly).range;
  return ast::StructValueExpr{range, identifier, std::move(named_args)};
}

auto Parser::parse_named_args() -> std::vector<ast::NamedArg> {
  if (!match(Identifier)) {
    return {};
  }

  std::vector<ast::NamedArg> args;
  args.push_back(parse_named_arg());
  while (match(Comma)) {
    lexer_.consume();
    args.push_back(parse_named_arg());
  }
  return args;
}

auto Parser::parse_named_arg() -> ast::NamedArg {
  auto const identifier = parse_identifier();
  consume(Colon);
  auto expr = parse_expr();
  auto const range = identifier.range + ast::range(expr);
  return ast::NamedArg{range, identifier, std::make_unique<ast::Expr>(std::move(expr))};
}

auto Parser::parse_identifiers() -> std::vector<ast::Identifier> {
  std::vector<ast::Identifier> identifiers;
  if (match(Identifier)) {
    identifiers.push_back(parse_identifier());
  } else {
    error(lexer_.current().range, "expected identifier");
  }

  fmt::print("here: {}\n", lexer_.current().literal(ctx_));

  while (match(Comma)) {
    fmt::print("...\n");
    lexer_.consume();
    identifiers.push_back(parse_identifier());
  }

  return identifiers;
}

auto Parser::parse_identifier() -> ast::Identifier {
  auto const token = consume(Identifier);
  std::string literal{token.literal(ctx_)};
  auto const id = ctx_.intern(std::move(literal));
  return ast::Identifier{token.range, id};
}

auto Parser::parse_type() -> ast::Type {
  using enum ast::BasicTypeKind;
  auto make_type = [this](auto kind) {
    lexer_.consume();
    return ast::BasicType{kind};
  };

  switch (auto const token = lexer_.current(); token.kind) {
    case Ti8:
      return make_type(Int8);
    case Ti16:
      return make_type(Int16);
    case Ti32:
      return make_type(Int32);
    case Ti64:
      return make_type(Int64);
    case Tu8:
      return make_type(UInt8);
    case Tu16:
      return make_type(UInt16);
    case Tu32:
      return make_type(UInt32);
    case Tu64:
      return make_type(UInt64);
    case Tisz:
      return make_type(ISize);
    case Tusz:
      return make_type(USize);
    case Tf32:
      return make_type(Float32);
    case Tf64:
      return make_type(Float64);
    case Tbool:
      return make_type(Bool);
    case Tstr:
      return make_type(Str);
    case LBracket:
      return parse_array_type();
    case Identifier:
      return parse_name_type();
    default:
      error(token.range, "expected type");
  }
}

auto Parser::parse_name_type() -> ast::NameType {
  auto const identifier = parse_identifier();
  return ast::NameType{identifier};
}

auto Parser::parse_array_type() -> ast::ArrayType {
  consume(LBracket);
  auto element_type = parse_type();
  consume(Comma);
  auto const token = consume(Lint);
  auto const literal = token.literal(ctx_);
  std::size_t element_count = 0;
  std::from_chars(literal.begin(), literal.end(), element_count);
  consume(RBracket);
  return ast::ArrayType{std::make_unique<ast::Type>(std::move(element_type)), element_count};
}

auto Parser::consume(lexer::TokenKind const kind) -> lexer::Token {
  if (auto const token = lexer_.current(); kind != token.kind) {
    error(token.range,
          fmt::format("unexpected token {} expected {}", static_cast<std::size_t>(token.kind),
                      static_cast<std::size_t>(kind)));
  }

  return lexer_.consume();
}

auto Parser::match(lexer::TokenKind const kind) -> bool { return lexer_.current().kind == kind; }

auto Parser::error(SourceRange const& range, std::string_view message) const -> void {
  throw std::runtime_error{fmt::format("Error (line {} column {}): token '{}' {}", range.line,
                                       range.column, range.literal(ctx_), message)};
}

auto parser::parse(Context& context) -> ast::Module {
  Parser parser{context};
  return parser.parse_module();
}