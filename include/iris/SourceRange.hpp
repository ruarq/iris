//
// Created by ruarq on 01/08/2024.
//

#ifndef IRIS_SOURCE_RANGE_HPP
#define IRIS_SOURCE_RANGE_HPP

#include <cstddef>
#include <string_view>

#include "Context.hpp"

namespace iris {
  /**
   * @brief Represents a range of characters in a file
   */
  class SourceRange {
  public:
    /**
     * @brief Join or "add" two source ranges together.
     * @param a A source range
     * @param b The other source range
     * @return The result of joining the two source ranges together
     */
    [[nodiscard]] static auto join(SourceRange const &a, SourceRange const &b) -> SourceRange;

  public:
    SourceRange() = default;
    SourceRange(SourceRange const &) = default;
    SourceRange(SourceRange &&) = default;
    auto operator=(SourceRange const &) -> SourceRange & = default;
    auto operator=(SourceRange &&) -> SourceRange & = default;

    /**
     * @brief Construct a new SourceRange object.
     * @param offset The offset/position where the range starts
     * @param size The size of the range
     * @param line The line where the range starts
     * @param column The column where the range starts
     */
    SourceRange(std::size_t offset, std::size_t size, std::size_t line, std::size_t column);

  public:
    /**
     * @brief Get the end offset of the source range.
     * @return The offset where the source range ends
     */
    [[nodiscard]] auto end() const -> std::size_t;

    /**
     * @brief Get the literal of the source range.
     * @param buffer The buffer the source range belongs to
     * @return A string view to the part of the buffer which is represented by the range
     */
    [[nodiscard]] auto literal(std::string_view buffer) const -> std::string_view;

    /**
     * @brief Get the literal of the source range.
     * @param context The context the source range belongs to
     * @return A string view representing the literal of the source range
     */
    [[nodiscard]] auto literal(Context const &context) const -> std::string_view;

    /**
     * @brief "Add" or join two source ranges together.
     * @param other The other source range
     * @return The result of joining both source ranges together
     */
    [[nodiscard]] auto operator+(SourceRange const &other) const -> SourceRange;

    /**
     * @brief "Add" or join another source range into this one.
     * @param other The other source range
     * @return SourceRange &
     */
    auto operator+=(SourceRange const &other) -> SourceRange &;

    /**
     * @brief Compare two source ranges for equality
     * @param other The other source range
     * @return true If both source ranges are equal
     * @return false If one source range is not equal to the other
     */
    [[nodiscard]] auto operator==(SourceRange const &other) const -> bool;

    /**
     * @brief Compare two source range for inequality
     * @param other The other source range
     * @return true If both source ranges are not equal
     * @return false If both source range are equal
     */
    [[nodiscard]] auto operator!=(SourceRange const &other) const -> bool;

  public:
    std::size_t offset = 0;  ///< The offset/position where the source range starts
    std::size_t size = 1;    ///< The size of the source range
    std::size_t line = 1;    ///< The line where the source range starts
    std::size_t column = 1;  ///< The column where the source range starts
  };
}  // namespace iris

#endif  // IRIS_SOURCE_RANGE_HPP
