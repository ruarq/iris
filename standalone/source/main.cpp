#include <cxxopts.hpp>
#include <iostream>
#include <iris/iris.hpp>
#include <iris/version.hpp>

auto main(int const argc, char** argv) -> int {
  cxxopts::Options options("irisc", "Compiler for the iris programming language.");
  options.add_options()("h,help", "display this information")("dump-ast",
                                                              "dump the abstract syntax tree");
  auto const result = options.parse(argc, argv);
  if (result["help"].as<bool>()) {
    std::cout << options.help();
  }

  for (auto const& filename : result.unmatched()) {
    auto ctx = iris::Context::from(filename);
    auto ast = iris::parser::parse(ctx);
    if (result["dump-ast"].as<bool>()) {
      std::cout << "Dumping AST for " << ctx.file.path() << "\n";
      std::cout << dump(ctx, ast) << "\n";
    }
  }

  return 0;
}
