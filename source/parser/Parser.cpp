//
// Created by ruarq on 02/08/2024.
//

#include <fmt/format.h>

#include <cassert>
#include <charconv>
#include <functional>
#include <iris/parser/Parser.hpp>

#define IRIS_UNREACHABLE(x) (throw std::runtime_error{#x " : unreachable"})

namespace iris::parser {
  using enum lexer::TokenKind;

  auto parse_rule(lexer::TokenKind const kind) -> std::optional<ParseRule> {
    using enum Associativity;

    // TODO(ruarq): Put this in a lookup table.
    static std::unordered_map<lexer::TokenKind, ParseRule> map{
        {Identifier,
         ParseRule{std::nullopt, &Parser::parse_primary_expr, nullptr, {}, Binding::Primary, None}},
        {Lbool,
         ParseRule{std::nullopt, &Parser::parse_primary_expr, nullptr, {}, Binding::Primary, None}},
        {Lchar,
         ParseRule{std::nullopt, &Parser::parse_primary_expr, nullptr, {}, Binding::Primary, None}},
        {Lint,
         ParseRule{std::nullopt, &Parser::parse_primary_expr, nullptr, {}, Binding::Primary, None}},
        {Lfloat,
         ParseRule{std::nullopt, &Parser::parse_primary_expr, nullptr, {}, Binding::Primary, None}},
        {Lstr,
         ParseRule{std::nullopt, &Parser::parse_primary_expr, nullptr, {}, Binding::Primary, None}},
        {Plus, ParseRule{std::nullopt, nullptr, &Parser::parse_binary_expr, ast::BinaryOpKind::Add,
                         Binding::Term, Left}},
        {Dash, ParseRule{ParseRule::Prefix{&Parser::parse_unary_expr, Binding::Unary}, nullptr,
                         &Parser::parse_binary_expr, ast::BinaryOpKind::Sub, Binding::Term, Left}},
        {Asterisk, ParseRule{std::nullopt, nullptr, &Parser::parse_binary_expr,
                             ast::BinaryOpKind::Mul, Binding::Factor, Left}},
        {Slash, ParseRule{std::nullopt, nullptr, &Parser::parse_binary_expr, ast::BinaryOpKind::Div,
                          Binding::Factor, Left}},
        {Percent, ParseRule{std::nullopt, nullptr, &Parser::parse_binary_expr,
                            ast::BinaryOpKind::Mod, Binding::Factor, Left}},
        {Dot, ParseRule{std::nullopt,
                        nullptr,
                        &Parser::parse_member_select_expr,
                        {},
                        Binding::MemberSelect,
                        Left}},
        {LParen,
         ParseRule{std::nullopt, nullptr, &Parser::parse_call_expr, {}, Binding::Call, Left}},
        {BarBar, ParseRule{std::nullopt, nullptr, &Parser::parse_binary_expr, ast::BinaryOpKind::Or,
                           Binding::Or, Left}},
        {AmpersandAmpersand, ParseRule{std::nullopt, nullptr, &Parser::parse_binary_expr,
                                       ast::BinaryOpKind::And, Binding::And, Left}},
        {EqualEqual, ParseRule{std::nullopt, nullptr, &Parser::parse_binary_expr,
                               ast::BinaryOpKind::Eq, Binding::Equality, Left}},
        {ExclamEqual, ParseRule{std::nullopt, nullptr, &Parser::parse_binary_expr,
                                ast::BinaryOpKind::NEq, Binding::Equality, Left}},
        {Ampersand, ParseRule{std::nullopt, nullptr, &Parser::parse_binary_expr,
                              ast::BinaryOpKind::BAnd, Binding::BAnd, Left}},
        {Bar, ParseRule{std::nullopt, nullptr, &Parser::parse_binary_expr, ast::BinaryOpKind::BOr,
                        Binding::BOr, Left}},
        {Caret, ParseRule{std::nullopt, nullptr, &Parser::parse_binary_expr,
                          ast::BinaryOpKind::BXor, Binding::BXor, Left}},
        {LAngle, ParseRule{std::nullopt, nullptr, &Parser::parse_binary_expr, ast::BinaryOpKind::Lt,
                           Binding::Comparison, Left}},
        {RAngle, ParseRule{std::nullopt, nullptr, &Parser::parse_binary_expr, ast::BinaryOpKind::Gt,
                           Binding::Comparison, Left}},
        {LAngleEqual, ParseRule{std::nullopt, nullptr, &Parser::parse_binary_expr,
                                ast::BinaryOpKind::LEq, Binding::Comparison, Left}},
        {RAngleEqual, ParseRule{std::nullopt, nullptr, &Parser::parse_binary_expr,
                                ast::BinaryOpKind::GEq, Binding::Comparison, Left}},
        {LAngleLAngle, ParseRule{std::nullopt, nullptr, &Parser::parse_binary_expr,
                                 ast::BinaryOpKind::BLs, Binding::BShift, Left}},
        {RAngleRAngle, ParseRule{std::nullopt, nullptr, &Parser::parse_binary_expr,
                                 ast::BinaryOpKind::BRs, Binding::BShift, Left}},
        {Equal,
         ParseRule{std::nullopt, nullptr, &Parser::parse_assign_expr, {}, Binding::Assign, Right}},
        {Exclam, ParseRule{ParseRule::Prefix{&Parser::parse_unary_expr, Binding::Unary},
                           nullptr,
                           nullptr,
                           {},
                           Binding::Primary,
                           None}},
        // {LCurly,
        // ParseRule{std::nullopt, nullptr, &Parser::parse_struct_value_expr, Binding::Call, Left}},
    };

    if (map.contains(kind)) {
      return map.at(kind);
    }

    return std::nullopt;
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
      case Lstr:
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
      case Kif:
      case Kwhile:
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

    std::optional<ast::TypeNode> type = std::nullopt;
    if (match(Colon)) {
      lexer_.consume();
      type = parse_type();
    }
    return ast::FunctionSignature{range, identifier, std::move(params), std::move(type)};
  }

  auto Parser::parse_params() -> std::vector<ast::Param> {
    std::vector<ast::Param> params;

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
    switch (auto const token = lexer_.current(); token.kind) {
      case Klet:
        return parse_let_stmt();
      case Kmut:
        return parse_mut_stmt();
      case Kret:
        return parse_return_stmt();
      case Kif:
        return parse_if_else_stmt();
      case Kwhile:
        return parse_while_stmt();
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

  auto Parser::parse_if_else_stmt() -> ast::IfElseStmt {
    auto if_body = parse_if_body();
    std::optional<ast::ElseBody> else_body = std::nullopt;
    if (match(Kelse)) {
      else_body = parse_else_body();
    }
    return ast::IfElseStmt{std::move(if_body), std::move(else_body)};
  }

  auto Parser::parse_if_body() -> ast::IfBody {
    auto const token = consume(Kif);
    auto condition = parse_expr();
    auto block = parse_block();
    auto const range = token.range + ast::range(condition);
    return ast::IfBody{range, std::move(condition), std::move(block)};
  }

  auto Parser::parse_else_body() -> ast::ElseBody {
    auto const token = consume(Kelse);
    auto block = parse_block();
    return ast::ElseBody{token.range, std::move(block)};
  }

  auto Parser::parse_while_stmt() -> ast::WhileStmt {
    auto const token = consume(Kwhile);
    auto condition = parse_expr();
    auto block = parse_block();
    auto const range = token.range + ast::range(condition);
    return ast::WhileStmt{range, std::move(condition), std::move(block)};
  }

  auto Parser::parse_expr(Binding min_binding) -> ast::Expr {
    auto token = lexer_.current();
    auto rule = parse_rule(lexer_.current().kind);
    if (!rule) {
      error(token.range, "expected expression");
    }

    if (rule->prefix) {
      return rule->prefix->fn(this, rule->prefix->binding);
    }

    auto expr = parse_nud();

    token = lexer_.current();
    rule = parse_rule(token.kind);

    while (rule && rule->led) {
      if (rule->binding > min_binding && rule->associativity == Associativity::Left) {
        expr = rule->led(this, std::move(expr), rule->binding);
      } else if (rule->binding == min_binding && rule->associativity == Associativity::Right) {
        expr = rule->led(this, std::move(expr), rule->binding);
      } else {
        break;
      }

      token = lexer_.current();
      rule = parse_rule(token.kind);
    }

    return expr;
  }

  auto Parser::parse_nud() -> ast::Expr {
    auto const token = lexer_.current();
    auto const rule = parse_rule(token.kind);
    if (!rule || !rule->nud) {
      error(token.range, "expected expression");
    }
    return rule->nud(this);
  }

  auto Parser::parse_binary_expr(ast::Expr left, Binding) -> ast::Expr {
    auto token = lexer_.current();
    auto rule = parse_rule(token.kind);
    if (!rule || !rule->led) {
      error(token.range, "expected binary operator");
    }

    ast::BinaryOp const op{token.range, rule->bop};

    lexer_.consume();
    auto right = parse_expr(rule->binding);

    auto const range = ast::range(left) + ast::range(right);
    left = ast::BinaryExpr{range, std::make_unique<ast::Expr>(std::move(left)), op,
                           std::make_unique<ast::Expr>(std::move(right))};

    return left;
  }

  auto Parser::parse_member_select_expr(ast::Expr value, Binding const min_binding) -> ast::Expr {
    consume(Dot);
    auto select = parse_expr(min_binding);
    auto const range = ast::range(value) + ast::range(select);
    return ast::MemberSelectExpr{range, std::make_unique<ast::Expr>(std::move(value)),
                                 std::make_unique<ast::Expr>(std::move(select))};
  }

  auto Parser::parse_assign_expr(ast::Expr target, Binding const min_binding) -> ast::Expr {
    consume(Equal);
    auto value = parse_expr(min_binding);
    auto const range = ast::range(target) + ast::range(value);
    return ast::AssignExpr{range, std::make_unique<ast::Expr>(std::move(target)),
                           std::make_unique<ast::Expr>(std::move(value))};
  }

  auto Parser::parse_call_expr(ast::Expr callee, Binding) -> ast::Expr {
    consume(LParen);
    auto args = parse_args();
    auto const last = consume(RParen);
    auto const range = ast::range(callee) + last.range;
    return ast::CallExpr{range, std::make_unique<ast::Expr>(std::move(callee)), std::move(args)};
  }

  auto Parser::parse_args() -> std::vector<ast::Expr> {
    if (!is_expr(lexer_.current().kind)) {
      return {};
    }

    std::vector<ast::Expr> args;
    args.push_back(parse_expr());
    while (match(Comma)) {
      lexer_.consume();
      args.push_back(parse_expr());
    }
    return args;
  }

#if 0
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
#endif

  auto Parser::parse_unary_op() -> ast::UnaryOp {
    auto const token = lexer_.current();

    auto const result = to_unary_op(token.kind);
    if (!result.has_value()) {
      error(token.range, "expected unary op");
    }

    lexer_.consume();

    return ast::UnaryOp{token.range, result.value()};
  }

  auto Parser::parse_unary_expr(Binding const min_binding) -> ast::Expr {
    auto const op = parse_unary_op();
    auto expr = parse_expr(min_binding);

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

      case Identifier:
        return parse_name_expr();

      default:
        error(token.range, "expected primary expression");
    }
  }

  auto Parser::parse_value_expr() -> ast::ValueExpr {
    ast::Value value;
    auto const token = lexer_.current();
    auto const literal = token.literal(ctx_);

    switch (token.kind) {
      case Lint: {
        std::uint64_t int_value = 0;
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

  auto Parser::parse_struct_value_expr(ast::Expr expr, Binding) -> ast::Expr {
    consume(LCurly);
    auto named_args = parse_named_args();
    auto const range = ast::range(expr) + consume(RCurly).range;
    return ast::StructValueExpr{range, std::make_unique<ast::Expr>(std::move(expr)),
                                std::move(named_args)};
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

    while (match(Comma)) {
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

  auto Parser::parse_type() -> ast::TypeNode {
    using enum ast::BasicTypeKind;
    auto make_type = [this](auto kind) {
      auto const token = lexer_.consume();
      return ast::TypeNode{token.range, ast::BasicType{kind}};
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

  auto Parser::parse_name_type() -> ast::TypeNode {
    auto const identifier = parse_identifier();
    return ast::TypeNode{identifier.range, ast::NameType{identifier}};
  }

  auto Parser::parse_array_type() -> ast::TypeNode {
    auto const first = consume(LBracket);
    auto [_, type] = parse_type();
    consume(Comma);
    auto const token = consume(Lint);
    auto const literal = token.literal(ctx_);
    std::size_t element_count = 0;
    std::from_chars(literal.begin(), literal.end(), element_count);
    auto const last = consume(RBracket);
    auto const range = first.range + last.range;
    return ast::TypeNode{
        range, ast::ArrayType{std::make_unique<ast::Type>(std::move(type)), element_count}};
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

  auto parse(Context& context) -> ast::Module {
    Parser parser{context};
    return parser.parse_module();
  }
}  // namespace iris::parser