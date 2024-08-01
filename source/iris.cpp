#include <fmt/format.h>

#include <iris/iris.hpp>

namespace iris {
  auto dump_tokens(Context& context) -> void {
    using namespace lexer;

    Scanner const scanner{context.file.content()};
    Tokenizer const tokenizer{context, scanner};
    Lexer lexer{tokenizer};

    fmt::print("Dumping tokens for {}\n", context.file.path().string());
    while (!lexer.is_eof()) {
      auto const [_, range] = lexer.consume();
      fmt::print("{}:{}: '{}'\n", range.line, range.column, range.literal(context));
    }
  }

}  // namespace iris