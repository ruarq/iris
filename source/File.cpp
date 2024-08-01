//
// Created by ruarq on 01/08/2024.
//

#include <iris/File.hpp>

using namespace iris;

File::File(std::filesystem::path path, std::string content)
    : path_(std::move(path)), content_(std::move(content)) {}

auto File::path() const -> std::filesystem::path { return path_; }

auto File::content() const -> std::string_view { return content_; }
