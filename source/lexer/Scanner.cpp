//
// Created by ruarq on 01/08/2024.
//

#include <fmt/format.h>

#include <iris/lexer/Scanner.hpp>
#include <stdexcept>

using namespace iris::lexer;

Scanner::Scanner(std::string_view const buffer) : buffer_(buffer), current_(0) {}

auto Scanner::peek(std::size_t const amount) const -> char {
  if (current_ + amount >= buffer_.size()) {
    throw std::out_of_range{fmt::format("{} : current_({}) + amount({}) >= buffer_.size({})",
                                        "Scanner::peek(std::size_t)", current_, amount,
                                        buffer_.size())};
  }

  return buffer_[current_ + amount];
}

auto Scanner::current() const -> char { return peek(0); }

auto Scanner::advance(std::size_t const amount) -> void { current_ += amount; }

auto Scanner::consume() -> char {
  auto const c = current();
  ++current_;
  return c;
}

auto Scanner::is_eof() const -> bool { return current_ >= buffer_.size(); }
