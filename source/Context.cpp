//
// Created by ruarq on 01/08/2024.
//

#include <fstream>
#include <iris/Context.hpp>
#include <iterator>

using namespace iris;

auto Context::create(std::filesystem::path path) -> Context {
  std::ifstream in{path};
  std::string content{std::istreambuf_iterator{in}, {}};
  return {.file = File{std::move(path), std::move(content)}};
}
