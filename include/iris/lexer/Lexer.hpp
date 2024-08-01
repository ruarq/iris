//
// Created by Anton Büttner on 01.08.24.
//

#ifndef IRIS_LEXER_LEXER_HPP
#define IRIS_LEXER_LEXER_HPP

#include <deque>

#include "Tokenizer.hpp"

namespace iris::lexer {
  /**
   * @brief The lexer is just a class to manage tokens more effectively, it's basically just an
   * interface to the Tokenizer.
   */
  class Lexer {
  public:
    /**
     * @brief The minimum size we want the buffer to have when we refill it
     */
    constexpr static std::size_t buffer_fill_size = 1024;

  public:
    /**
     * @brief Create a new lexer object
     * @param tokenizer
     */
    explicit Lexer(Tokenizer const &tokenizer);

  public:
    /**
     * @brief Peek @a amount of tokens ahead.
     * @param amount The amount of tokens to peek ahead
     * @return The token @a amount of tokens ahead
     */
    [[nodiscard]] auto peek(std::size_t amount = 1) -> Token;

    /**
     * @brief Get the current token
     * @return The current token
     */
    [[nodiscard]] auto current() -> Token;

    /**
     * @brief Consume the current token
     * @return The current token
     */
    auto consume() -> Token;

    /**
     * @brief Check whether the lexer reached EOF.
     * @return true if the lexer reached EOF
     * @return false otherwise
     */
    [[nodiscard]] auto is_eof() -> bool;

  private:
    /**
     * @brief Read @a amount of tokens into the token buffer.
     * @param amount The amount of tokens to read
     */
    auto read_tokens(std::size_t amount) -> void;

  private:
    Tokenizer tokenizer_;       ///< The tokenizer, this is where we get the tokens from.
    std::deque<Token> buffer_;  ///< This is where we store the tokens coming from the tokenizer.
  };
}  // namespace iris::lexer

#endif  // IRIS_LEXER_LEXER_HPP
