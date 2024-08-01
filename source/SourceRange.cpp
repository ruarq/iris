//
// Created by ruarq on 01/08/2024.
//

#include <algorithm>
#include <iris/SourceRange.hpp>

using namespace iris;

auto SourceRange::join(SourceRange const& a, SourceRange const& b) -> SourceRange {
  auto const offset = std::min(a.offset, b.offset);
  auto const end = std::max(a.end(), b.end());
  auto const size = end - offset;

  if (b.offset < a.offset) {
    return {offset, size, b.line, b.column};
  }

  return {offset, size, a.line, a.column};
}

SourceRange::SourceRange(std::size_t const offset, std::size_t const size, std::size_t const line,
                         std::size_t const column)
    : offset(offset), size(size), line(line), column(column) {}

auto SourceRange::end() const -> std::size_t { return offset + size; }

auto SourceRange::literal(std::string_view const buffer) const -> std::string_view {
  return buffer.substr(offset, size);
}

auto SourceRange::literal(Context const& context) const -> std::string_view {
  return literal(context.file.content());
}

auto SourceRange::operator+(SourceRange const& other) const -> SourceRange {
  return join(*this, other);
}

auto SourceRange::operator+=(SourceRange const& other) -> SourceRange& {
  return *this = *this + other;
}

auto SourceRange::operator==(SourceRange const& other) const -> bool {
  // Theoretically, if both source ranges have the same offset their line and column
  // parameters would also be the same. But this function will probably only be used in tests for
  // convenience, so it doesn't really matter whether it's slow or not.
  return offset == other.offset && size == other.size && line == other.line
         && column == other.column;
}

auto SourceRange::operator!=(SourceRange const& other) const -> bool { return !(*this == other); }
