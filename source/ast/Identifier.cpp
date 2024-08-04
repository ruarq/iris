//
// Created by ruarq on 01/08/2024.
//

#include <iris/ast/Identifier.hpp>

using namespace iris::ast;

Identifier::Identifier(SourceRange const& range, std::size_t const id) : range(range), id_(id) {}

auto Identifier::string(Context const& context) const -> std::string_view {
  return context.get(id_);
}

auto Identifier::operator==(Identifier const& other) const -> bool { return id_ == other.id_; }

auto Identifier::operator!=(Identifier const& other) const -> bool { return !(*this == other); }
