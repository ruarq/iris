#include <iris/iris.hpp>
#include <iris/version.hpp>

#include <iostream>

#include <cxxopts.hpp>

auto main(int argc, char** argv) -> int {
  cxxopts::Options options("irisc", "Compiler for the iris programming language.");
  options.add_options()
    ("t,test", "testing options");
  auto result = options.parse(argc, argv);
  if (result["test"].as<bool>()) {
    std::cout << iris::hello_world() << "\n";
  }
  return 0;
}
