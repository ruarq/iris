//
// Created by ruarq on 01/08/2024.
//

#ifndef IRIS_FILE_HPP
#define IRIS_FILE_HPP

#include <filesystem>
#include <string>
#include <string_view>

namespace iris {
  /**
   * @brief Represents a source file.
   */
  class File {
  public:
    File(std::filesystem::path path, std::string content);

  public:
    /**
     * @return The path of the file
     */
    [[nodiscard]] auto path() const -> std::filesystem::path;

    /**
     * @return The contents of the file
     */
    [[nodiscard]] auto content() const -> std::string_view;

  private:
    std::filesystem::path path_;
    std::string content_;
  };
}  // namespace iris

#endif  // IRIS_FILE_HPP
