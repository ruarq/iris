//
// Created by ruarq on 01/08/2024.
//

#ifndef IRIS_LEXER_TOKEN_HPP
#define IRIS_LEXER_TOKEN_HPP

#include <iris/Context.hpp>
#include <iris/SourceRange.hpp>

namespace iris::lexer {
  enum class TokenKind {
    /* SYNTACTIC ELEMENTS */

    LParen,    ///< (
    RParen,    ///< )
    LBracket,  ///< [
    RBracket,  ///< ]
    LCurly,    ///< {
    RCurly,    ///< }
    Comma,     ///< ,
    Dot,       ///< .
    Colon,     ///< :
    // Semicolon,  ///< ;

    /* ARITHMETHIC OPERATORS */

    Plus,      ///< +
    PlusPlus,  ///< ++
    Dash,      ///< -
    DashDash,  ///< --
    Asterisk,  ///< *
    Slash,     ///< /
    Percent,   ///< %

    /* BINARY OPERATORS */

    LAngleLAngle,  ///< <<
    RAngleRAngle,  ///< >>
    Ampersand,     ///< &
    Bar,           ///< |
    Caret,         ///< ^

    /* LOGICAL OPERATORS */

    AmpersandAmpersand,  ///< && @note yes, seriously I'm spelling it out.
    BarBar,              ///< ||
    Exclam,              ///< !

    /* ASSIGNMENT OPERATORS */

    Equal,                    ///< =
    PlusEqual,                ///< +=
    DashEqual,                ///< -=
    AsteriskEqual,            ///< *=
    SlashEqual,               ///< /=
    PercentEqual,             ///< %=
    LAngleLAngleEqual,        ///< <<=
    RAngleRAngleEqual,        ///< >>=
    AmpersandEqual,           ///< &=
    AmpersandAmpersandEqual,  ///< &&= @note And YES, HERE TOO.
    BarEqual,                 ///< |=
    BarBarEqual,              ///< ||=
    CaretEqual,               ///< ^=

    /* COMPARISON OPERATORS */

    LAngle,       ///< <
    LAngleEqual,  ///< <=
    EqualEqual,   ///< ==
    ExclamEqual,  ///< !=
    RAngleEqual,  ///< >=
    RAngle,       ///< >

    /* KEYWORDS */

    Kfn,      ///< fn
    Klet,     ///< let
    Kmut,     ///< mut
    Kret,     ///< ret
    Kstruct,  ///< struct
    Kif,      ///< if
    Kelse,    ///< else
    Kwhile,   ///< while

    /* TYPES */

    Ti8,    ///< i8
    Ti16,   ///< i16
    Ti32,   ///< i32
    Ti64,   ///< i64
    Tu8,    ///< u8
    Tu16,   ///< u16
    Tu32,   ///< u32
    Tu64,   ///< u64
    Tisz,   ///< isz
    Tusz,   ///< usz
    Tf32,   ///< f32
    Tf64,   ///< f64
    Tstr,   ///< str
    Tbool,  ///< bool

    /* LITERALS */

    Lint,    ///< integer literals
    Lfloat,  ///< floating point literals
    Lbool,   ///< boolean literals
    Lchar,   ///< character literals
    Lstr,    ///< string literals

    /* OTHER */

    Identifier,  ///< all kinds of identifiers
    EndOfFile,   ///< end of file
    Unknown,     ///< unknown token

    Count  ///< The amount of tokens existing.
  };

  /**
   * @brief Represents a token in source file.
   */
  class Token {
  public:
    /**
     * @brief Construct a new token object.
     * @param kind The kind of the token
     * @param range The source range of the token
     */
    Token(TokenKind kind, SourceRange const &range);

  public:
    /**
     * @brief Get the literal of the token.
     * @note Exists just for convenience, simply just calls `token.range.literal(context)`.
     * @param context The context which the token belongs to
     * @return The literal of the token
     */
    [[nodiscard]] auto literal(Context const &context) const -> std::string_view;

    /**
     * @brief Check two tokens for equality.
     * @param other The other token
     * @return true if both tokens are equal
     * @return false if the tokens are inequal
     */
    [[nodiscard]] auto operator==(Token const &other) const -> bool;

    /**
     * @brief Check two tokens for inequality.
     * @param other The other token
     * @return true if both tokens are inequal
     * @return false if both tokens are equal
     */
    [[nodiscard]] auto operator!=(Token const &other) const -> bool;

  public:
    TokenKind kind;     ///< The kind of the token.
    SourceRange range;  ///< The source range of the token.
  };
}  // namespace iris::lexer

#endif  // IRIS_LEXER_TOKEN_HPP
