//
// Created by ruarq on 31.07.24.
//

#ifndef IRIS_IRIS_HPP
#define IRIS_IRIS_HPP

#include "Context.hpp"
#include "ast/Dump.hpp"
#include "ast/ast.hpp"
#include "lexer/Lexer.hpp"
#include "parser/Parser.hpp"

namespace iris {
  auto dump_tokens(Context &context) -> void;
}

#endif  // IRIS_IRIS_HPP
