//
// Created by ruarq on 01/08/2024.
//

#include <doctest/doctest.h>

#include <array>
#include <iris/lexer/Tokenizer.hpp>

using namespace iris;
using namespace iris::lexer;
using enum TokenKind;

constexpr static std::string_view code
    = "()[]{},: "                                                // SYNTACTIC ELEMENTS
      "+ ++- --*/% "                                             // ARITHMETIC OPERATORS
      "<<>>&|^ "                                                 // BINARY OPERATORS
      "&&||! "                                                   // LOGICAL OPERATORS
      "=+=-=*=/=%=<<=>>=&=&&=|=||=^= "                           // ASSIGNMENT OPERATORS
      "< <===!=>=> "                                             // COMPARISON OPERATORS
      "fn let mut ret "                                          // KEYWORDS
      "i8 i16 i32 i64 u8 u16 u32 u64 isz usz f32 f64 str bool "  // TYPES
      "1234 1234.0912049 true false 'a' \"hello, world!\" "      // LITERALS
      "abc main uint hello something "                           // IDENTIFIERS
    ;

constexpr static std::array expected = {
    /* SYNTACTIC ELEMENTS */
    LParen,
    RParen,
    LBracket,
    RBracket,
    LCurly,
    RCurly,
    Comma,
    Colon,

    /* ARITHMETIC OPERATORS */
    Plus,
    PlusPlus,
    Dash,
    DashDash,
    Asterisk,
    Slash,
    Percent,

    /* BINARY OPERATORS */
    LAngleLAngle,
    RAngleRAngle,
    Ampersand,
    Bar,
    Caret,

    /* LOGICAL OPERATORS */
    AmpersandAmpersand,
    BarBar,
    Exclam,

    /* ASSIGNMENT OPERATORS */
    Equal,
    PlusEqual,
    DashEqual,
    AsteriskEqual,
    SlashEqual,
    PercentEqual,
    LAngleLAngleEqual,
    RAngleRAngleEqual,
    AmpersandEqual,
    AmpersandAmpersandEqual,
    BarEqual,
    BarBarEqual,
    CaretEqual,

    /* COMPARISON OPERATORS */
    LAngle,
    LAngleEqual,
    EqualEqual,
    ExclamEqual,
    RAngleEqual,
    RAngle,

    /* KEYWORDS */
    Kfn,
    Klet,
    Kmut,
    Kret,

    /* TYPES */
    Ti8,
    Ti16,
    Ti32,
    Ti64,
    Tu8,
    Tu16,
    Tu32,
    Tu64,
    Tisz,
    Tusz,
    Tf32,
    Tf64,
    Tstr,
    Tbool,

    /* LITERALS */
    Lint,
    Lfloat,
    Lbool,
    Lbool,
    Lchar,
    Lstr,

    /* IDENTIFIERS */
    Identifier,
    Identifier,
    Identifier,
    Identifier,
    Identifier,
};

TEST_SUITE("Tokenizer") {
  TEST_CASE("next") {
    File const file{"/DUMMY/PATH", std::string{code}};
    Context context{file};
    Scanner scanner{file.content()};
    Tokenizer tokenizer{context, scanner};
    for (auto const kind : expected) {
      auto const token = tokenizer.next();
      INFO("Token: ", token.literal(context));
      CHECK_EQ(kind, token.kind);
    }

    CHECK_EQ(tokenizer.next().kind, EndOfFile);
  }
}