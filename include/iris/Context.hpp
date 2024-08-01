//
// Created by ruarq on 01/08/2024.
//

#ifndef IRIS_CONTEXT_HPP
#define IRIS_CONTEXT_HPP

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
    [[nodiscard]] static auto create(std::filesystem::path path) -> Context;

  public:
    File file;
  };
}  // namespace iris

#endif  // IRIS_CONTEXT_HPP
