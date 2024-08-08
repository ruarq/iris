//
// Created by ruarq on 01/08/2024.
//

#include <iris/lexer/Tokenizer.hpp>

using namespace iris::lexer;

Tokenizer::TokenKindMap const Tokenizer::map_identifier_to_keyword = {
    /* KEYWORDS */

    {"fn", TokenKind::Kfn},
    {"let", TokenKind::Klet},
    {"mut", TokenKind::Kmut},
    {"ret", TokenKind::Kret},
    {"struct", TokenKind::Kstruct},
    {"if", TokenKind::Kif},
    {"else", TokenKind::Kelse},
    {"while", TokenKind::Kwhile},

    /* TYPES */

    {"i8", TokenKind::Ti8},
    {"i16", TokenKind::Ti16},
    {"i32", TokenKind::Ti32},
    {"i64", TokenKind::Ti64},
    {"u8", TokenKind::Tu8},
    {"u16", TokenKind::Tu16},
    {"u32", TokenKind::Tu32},
    {"u64", TokenKind::Tu64},
    {"isz", TokenKind::Tisz},
    {"usz", TokenKind::Tusz},
    {"f32", TokenKind::Tf32},
    {"f64", TokenKind::Tf64},
    {"str", TokenKind::Tstr},
    {"bool", TokenKind::Tbool},

    /* LITERALS */
    {"true", TokenKind::Lbool},
    {"false", TokenKind::Lbool},
};

auto Tokenizer::is_identifier(char const c) -> bool {
  auto const is_lower = c >= 'a' && c <= 'z';
  auto const is_upper = c >= 'A' && c <= 'Z';
  return is_lower || is_upper || c == '_' || is_digit(c);
}

auto Tokenizer::is_digit(char const c) -> bool { return c >= '0' && c <= '9'; }

Tokenizer::Tokenizer(Context& context, Scanner const& scanner)
    : context_(context), scanner_(scanner) {}

auto Tokenizer::next() -> Token {
  using enum TokenKind;

  while (!scanner_.is_eof()) {
    auto const current = scanner_.current();

    if (is_digit(current)) {
      return read_number_literal();
    }

    // `is_identifier` also returns true if the char is a digit, but we don't want identifiers to
    // start with digits. That's why it's important that we call `is_identifier` after we've
    // confirmed that the current char is not a digit, which we accomplish by trying to tokenize
    // number literals first, as seen above.
    if (is_identifier(current)) {
      return read_identifier_or_keyword();
    }

    switch (current) {
        /* WHITESPACE */

      case ' ':
      case '\t':
      case '\n':
      case '\r':
        scanner_.advance();
        break;

      /* COMMENTS */
      case '#':
        while (scanner_.current() != '\n') {
          scanner_.advance();
        }
        scanner_.advance();  // Skip the newline
        break;

      case '\'':
        return read_char_literal();
      case '"':
        return read_string_literal();

        /* SYNTACTIC ELEMENTS */

      case '(':
        return read_token(LParen);
      case ')':
        return read_token(RParen);
      case '[':
        return read_token(LBracket);
      case ']':
        return read_token(RBracket);
      case '{':
        return read_token(LCurly);
      case '}':
        return read_token(RCurly);
      case ',':
        return read_token(Comma);
      case '.':
        return read_token(Dot);
      case ':':
        return read_token(Colon);

        /* +, ++, += */
      case '+': {
        switch (scanner_.peek(1)) {
          case '+':
            return read_token(PlusPlus, 2);
          case '=':
            return read_token(PlusEqual, 2);
          default:
            break;
        }
        return read_token(Plus);
      }

      /* -, --, -= */
      case '-': {
        switch (scanner_.peek(1)) {
          case '-':
            return read_token(DashDash, 2);
          case '=':
            return read_token(DashEqual, 2);
          default:
            break;
        }
        return read_token(Dash);
      }

        /* *, *= */
      case '*': {
        if (scanner_.peek(1) == '=') {
          return read_token(AsteriskEqual, 2);
        }
        return read_token(Asterisk);
      }

      /* /, /= */
      case '/': {
        if (scanner_.peek(1) == '=') {
          return read_token(SlashEqual, 2);
        }
        return read_token(Slash);
      }

      /* %, %= */
      case '%': {
        if (scanner_.peek(1) == '=') {
          return read_token(PercentEqual, 2);
        }
        return read_token(Percent);
      }

      /* <, <<, <=, <<= */
      case '<': {
        switch (scanner_.peek(1)) {
          case '<': {
            if (scanner_.peek(2) == '=') {
              return read_token(LAngleLAngleEqual, 3);
            }
            return read_token(LAngleLAngle, 2);
          }
          case '=':
            return read_token(LAngleEqual, 2);
          default:
            break;
        }
        return read_token(LAngle);
      }

      /* >, >>, >=, >>= */
      case '>': {
        switch (scanner_.peek(1)) {
          case '>': {
            if (scanner_.peek(2) == '=') {
              return read_token(RAngleRAngleEqual, 3);
            }
            return read_token(RAngleRAngle, 2);
          }
          case '=':
            return read_token(RAngleEqual, 2);
          default:
            break;
        }
        return read_token(RAngle);
      }

        /* &, &&, &=, &&= */
      case '&': {
        switch (scanner_.peek(1)) {
          case '&': {
            if (scanner_.peek(2) == '=') {
              return read_token(AmpersandAmpersandEqual, 3);
            }
            return read_token(AmpersandAmpersand, 2);
          }
          case '=':
            return read_token(AmpersandEqual, 2);
          default:
            break;
        }
        return read_token(Ampersand);
      }

      /* |, ||, |=, ||= */
      case '|': {
        switch (scanner_.peek(1)) {
          case '|': {
            if (scanner_.peek(2) == '=') {
              return read_token(BarBarEqual, 3);
            }
            return read_token(BarBar, 2);
          }
          case '=':
            return read_token(BarEqual, 2);
          default:
            break;
        }
        return read_token(Bar);
      }

      /* ^, ^= */
      case '^': {
        if (scanner_.peek(1) == '=') {
          return read_token(CaretEqual, 2);
        }
        return read_token(Caret);
      }

      /* !, != */
      case '!': {
        if (scanner_.peek(1) == '=') {
          return read_token(ExclamEqual, 2);
        }
        return read_token(Exclam);
      }

      /* =, == */
      case '=': {
        if (scanner_.peek(1) == '=') {
          return read_token(EqualEqual, 2);
        }
        return read_token(Equal);
      }

      default:
        // TODO(ruarq): Emit diagnostic message.
        return read_token(Unknown);
    }
  }

  return Token{EndOfFile, scanner_.position};
}

auto Tokenizer::read_identifier_or_keyword() -> Token {
  auto const first = range_start();
  while (is_identifier(scanner_.current())) {
    scanner_.advance();
  }
  auto const last = range_end();
  auto const range = first + last;
  auto const literal = range.literal(context_);

  auto const result = map_identifier_to_keyword.find(literal);
  auto kind = TokenKind::Identifier;

  if (result != map_identifier_to_keyword.end()) {
    kind = result->second;
  }

  return Token{kind, range};
}

auto Tokenizer::read_number_literal() -> Token {
  auto kind = TokenKind::Lint;
  auto range = read_digits();

  if (scanner_.current() == '.') {
    scanner_.advance();
    kind = TokenKind::Lfloat;
    range += read_digits();
  }

  return Token{kind, range};
}

auto Tokenizer::read_digits() -> SourceRange {
  auto const first = range_start();
  while (is_digit(scanner_.current())) {
    scanner_.advance();
  }
  auto const last = range_end();
  return first + last;
}

auto Tokenizer::read_char_literal() -> Token {
  scanner_.advance();  // '
  auto const token = read_token(TokenKind::Lchar);
  if (scanner_.current() != '\'') {
    // TODO(ruarq): Emit diagnostic error.
    // We'll return a valid token anyway, so that we can
    // detect more errors in later passes.
    return token;
  }

  scanner_.advance();  // '
  return token;
}

auto Tokenizer::read_string_literal() -> Token {
  scanner_.advance();  // "
  auto const first = range_start();
  while (scanner_.current() != '\n' && scanner_.current() != '"') {
    scanner_.advance();
  }
  auto const last = range_end();
  Token const token{TokenKind::Lstr, first + last};

  if (scanner_.current() != '"') {
    // TODO(ruarq): Emit diagnostic error.
    return token;
  }
  scanner_.advance();  // "
  return token;
}

auto Tokenizer::read_token(TokenKind const kind, std::size_t const size) -> Token {
  auto range = scanner_.position;
  range.size = size;
  scanner_.advance(size);
  return Token{kind, range};
}

auto Tokenizer::range_start() const -> SourceRange { return scanner_.position; }

auto Tokenizer::range_end() const -> SourceRange {
  auto position = scanner_.position;
  position.size = 0;
  return position;
}
