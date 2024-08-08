//
// Created by ruarq on 01/08/2024.
//

#ifndef IRIS_AST_IDENTIFIER_HPP
#define IRIS_AST_IDENTIFIER_HPP

#include <cstddef>
#include <iris/SourceRange.hpp>

namespace iris::ast {
  /**
   * @brief Represents an identifier in the ast.
   */
  class Identifier {
  public:
    /**
     * @brief Construct a new identifier object.
     * @param range The source range of the identifier
     * @param id The intered string id for the identifier
     */
    explicit Identifier(SourceRange const &range, std::size_t id);

  public:
    /**
     * @brief Get the identifier as string.
     * @note Essentially just for convenience, calls `context.get(identifier.id_)`.
     * @param context The context the identifier belongs to
     * @return The string of the identifier
     */
    [[nodiscard]] auto string(Context const &context) const -> std::string_view;

    /**
     * @brief Check if two identifiers equal each other.
     * @note Does not check if the ranges are equal.
     * @param other The other identifier
     * @return true if both identifiers are equal
     * @return false otherwise
     */
    [[nodiscard]] auto operator==(Identifier const &other) const -> bool;

    /**
     * @brief Test two identifiers for inequality.
     * @note Does not check if the ranges are not equal.
     * @param other The other identifier
     * @return true if the identifiers don't equal each other
     * @return false otherwise
     */
    [[nodiscard]] auto operator!=(Identifier const &other) const -> bool;

  public:
    SourceRange range;  ///< The source range of the identifier.

  private:
    std::size_t id_;  ///< The id of the identifier.
  };
}  // namespace iris::ast

#endif  // IRIS_AST_IDENTIFIER_HPP
