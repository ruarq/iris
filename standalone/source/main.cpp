#include <cxxopts.hpp>
#include <iostream>
#include <iris/iris.hpp>
#include <iris/version.hpp>

auto main(int argc, char** argv) -> int {
  cxxopts::Options options("irisc", "Compiler for the iris programming language.");
  options.add_options()("h,help", "display this information");
  auto result = options.parse(argc, argv);
  if (result["help"].as<bool>()) {
    std::cout << options.help();
  }

  for (auto const& filename : result.unmatched()) {
    auto ctx = iris::Context::create(filename);
    dump_tokens(ctx);
  }

  return 0;
}
