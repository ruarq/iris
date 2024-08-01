//
// Created by Anton Büttner on 01.08.24.
//

#ifndef IRIS_LEXER_SCANNER_HPP
#define IRIS_LEXER_SCANNER_HPP

#include <iris/SourceRange.hpp>
#include <string_view>

namespace iris::lexer {
  /**
   * @brief The scanner is used by the @a iris::lexer::Tokenizer to read a buffer character by
   * character.
   */
  class Scanner {
  public:
    /**
     * @brief Construct a new Scanner object from a buffer
     * @param buffer The buffer
     */
    explicit Scanner(std::string_view buffer);

  public:
    /**
     * @brief Peek ahead by a specific amount of characters.
     * @param amount The amount of characters to peek ahead
     * @return The character @a amount characters ahead
     */
    [[nodiscard]] auto peek(std::size_t amount = 0) const -> char;

    /**
     * @brief Get the current character
     * @return iris::lexer::Scanner::peek(0)
     */
    [[nodiscard]] auto current() const -> char;

    /**
     * @brief Advances the scanners position by @a amount characters.
     * @param amount The amount to move ahead
     */
    auto advance(std::size_t amount = 1) -> void;

    /**
     * @brief Consume the current character
     * @return the current character
     */
    [[nodiscard]] auto consume() -> char;

    /**
     * @brief Check if the scanner reached the end of the buffer.
     * @return true if the scanner reached the end of the buffer
     * @return false if the scanner did not reach the end of the buffer (yet)
     */
    [[nodiscard]] auto is_eof() const -> bool;

  public:
    SourceRange position;  ///< The current position of the scanner as SourceRange

  private:
    std::string_view buffer_;  ///< The buffer the scanner reads from
    std::size_t current_;      ///< The current position in the buffer
  };
}  // namespace iris::lexer

#endif  // IRIS_LEXER_SCANNER_HPP
