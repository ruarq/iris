//
// Created by Anton Büttner on 01.08.24.
//

#include <iris/lexer/Lexer.hpp>

using namespace iris::lexer;

Lexer::Lexer(Tokenizer const &tokenizer) : tokenizer_(tokenizer) {}

auto Lexer::peek(std::size_t const amount) -> Token {
  if (amount >= buffer_.size()) {
    auto const fill_size = std::max(amount + 1, buffer_fill_size);
    read_tokens(fill_size - buffer_.size());
  }

  return buffer_[amount];
}

auto Lexer::current() -> Token { return peek(0); }

auto Lexer::consume() -> Token {
  auto const token = current();
  buffer_.pop_front();
  return token;
}

auto Lexer::is_eof() -> bool { return current().kind == TokenKind::EndOfFile; }

auto Lexer::read_tokens(std::size_t amount) -> void {
  for (; amount > 0; --amount) {
    buffer_.push_back(tokenizer_.next());
  }
}
