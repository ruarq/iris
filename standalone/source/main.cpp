#include <cxxopts.hpp>
#include <iostream>
#include <iris/iris.hpp>
#include <iris/version.hpp>

auto main(int const argc, char** argv) -> int {
  cxxopts::Options options("irisc", "Compiler for the iris programming language.");
  options.add_options()("h,help", "display this information");
  auto const result = options.parse(argc, argv);
  if (result["help"].as<bool>()) {
    std::cout << options.help();
  }

  for (auto const& filename : result.unmatched()) {
    auto ctx = iris::Context::from(filename);
    dump_tokens(ctx);
    auto ast = iris::parser::parse(ctx);
  }

  return 0;
}
