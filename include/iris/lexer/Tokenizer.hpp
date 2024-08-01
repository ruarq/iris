//
// Created by ruarq on 01/08/2024.
//

#ifndef IRIS_LEXER_TOKENIZER_HPP
#define IRIS_LEXER_TOKENIZER_HPP

#include <iris/Context.hpp>
#include <unordered_map>

#include "Scanner.hpp"
#include "Token.hpp"

namespace iris::lexer {
  /**
   * @brief The tokenizer turns a stream of characters (from the Scanner) into tokens.
   */
  class Tokenizer {
  public:
    using TokenKindMap = std::unordered_map<std::string_view, TokenKind>;

  public:
    /**
     * @brief Check whether a character is a valid identifier character or not.
     * @param c The character to be checked
     * @return true if @a c is a valid character in an identifier
     * @return false if @a c isn't a valid character in an identifier
     */
    [[nodiscard]] static auto is_identifier(char c) -> bool;

    /**
     * @brief Check whether a character is a digit or not.
     * @param c The character to be checked
     * @return true if @a c is a digit
     * @return false if @a c isn't a digit
     */
    [[nodiscard]] static auto is_digit(char c) -> bool;

    /**
     * @brief Maps std::string_views to TokenKinds
     */
    static TokenKindMap const map_identifier_to_keyword;

  public:
    /**
     * @brief Construct a new tokenizer object.
     * @param context The context for the file being tokenized
     * @param scanner The scanner instance to use for tokenization
     */
    explicit Tokenizer(Context &context, Scanner const &scanner);

  public:
    /**
     * @brief Tokenize the next token.
     * @return The token
     */
    [[nodiscard]] auto next() -> Token;

  private:
    /**
     * @brief Read an identifier or keyword.
     * @return A token representing either an identifier, a keyword or a type
     */
    [[nodiscard]] auto read_identifier_or_keyword() -> Token;

    /**
     * @brief Read a number literal (either int or float).
     */
    [[nodiscard]] auto read_number_literal() -> Token;

    /**
     * @brief Read a sequence of digits.
     * @return The source range representing the digits
     */
    [[nodiscard]] auto read_digits() -> SourceRange;

    /**
     * @brief Read a char literal.
     */
    [[nodiscard]] auto read_char_literal() -> Token;

    /**
     * @brief Read a string literal.
     */
    [[nodiscard]] auto read_string_literal() -> Token;

    /**
     * @brief Read a token based off of a kind and a specific size.
     * @param kind The kind of the token
     * @param size The size of the token
     * @return The token
     */
    [[nodiscard]] auto read_token(TokenKind kind, std::size_t size = 1) -> Token;

    /**
     * @brief Get the source range at the start of token
     * @return The source range
     */
    [[nodiscard]] auto range_start() const -> SourceRange;

    /**
     * @brief Get the source range at the end of token
     * @return The source range
     */
    [[nodiscard]] auto range_end() const -> SourceRange;

  private:
    Context &context_;
    Scanner scanner_;
  };
}  // namespace iris::lexer

#endif  // IRIS_LEXER_TOKENIZER_HPP
