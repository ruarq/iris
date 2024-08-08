//
// Created by ruarq on 01/08/2024.
//

#include <fstream>
#include <iris/Context.hpp>
#include <iterator>

using namespace iris;

auto Context::from(std::filesystem::path path) -> Context {
  std::ifstream in{path};
  std::string content{std::istreambuf_iterator{in}, {}};
  return Context{File{std::move(path), std::move(content)}};
}

Context::Context(File file) : file(std::move(file)) {}

auto Context::intern(std::string string) -> std::size_t {
  if (auto const result = string_to_id_map_.find(string); result != string_to_id_map_.end()) {
    return result->second;
  }

  auto const new_id = interned_strings_.size();
  string_to_id_map_[string] = new_id;
  interned_strings_.push_back(std::move(string));
  return new_id;
}

auto Context::get(std::size_t const id) const -> std::string_view { return interned_strings_[id]; }
