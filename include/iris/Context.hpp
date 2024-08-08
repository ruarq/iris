//
// Created by ruarq on 01/08/2024.
//

#ifndef IRIS_CONTEXT_HPP
#define IRIS_CONTEXT_HPP

#include <unordered_map>
#include <vector>

#include "File.hpp"

namespace iris {
  /**
   * @brief The context of the current file
   */
  class Context {
  public:
    /**
     * @brief Create a context for a file.
     * @param path The filepath.
     * @return The created context
     */
    [[nodiscard]] static auto from(std::filesystem::path path) -> Context;

  public:
    /**
     * @brief Construct a new context object.
     * @param file The file used for the context
     */
    explicit Context(File file);

  public:
    /**
     * @brief Intern a string in the context.
     * @note Used mostly for identifiers.
     * @param string The string to intern
     * @return A id reference to the interned string
     */
    [[nodiscard]] auto intern(std::string string) -> std::size_t;

    /**
     * @brief Get a interned string.
     * @param id The id of the interned string
     * @return The string related to the id
     */
    [[nodiscard]] auto get(std::size_t id) const -> std::string_view;

  public:
    File file;  ///< The file of the context.

  private:
    std::vector<std::string>
        interned_strings_;  ///< The vector in which interned strings are stored.
    std::unordered_map<std::string, std::size_t>
        string_to_id_map_;  ///< The hash map used to generate ids for the interned strings.
  };
}  // namespace iris

#endif  // IRIS_CONTEXT_HPP
