//
// Created by ruarq on 08/08/2024.
//

#ifndef IRIS_AST_DUMP_HPP
#define IRIS_AST_DUMP_HPP

#include <iris/Context.hpp>
#include <ostream>

#include "ast.hpp"

namespace iris::ast {
  /**
   * @brief Dump an AST to review it.
   */
  class Dump {
  public:
    /**
     * @brief Construct a new dump object.
     * @param ctx The context
     * @param out The stream to write the AST to
     */
    explicit Dump(Context const &ctx, std::ostream &out);

  public:
    /**
     * @brief Dump a module.
     * @param module The module to dump
     */
    auto dump(Module const &module) -> void;

    /**
     * @brief Dump a decl.
     * @param decl The decl to dump
     */
    auto dump(Decl const &decl) -> void;

    /**
     * @brief Dump a block.
     * @param block The block to dump
     */
    auto dump(Block const &block) -> void;

    /**
     * @brief Dump a statement.
     * @param stmt The statement to dump
     */
    auto dump(Stmt const &stmt) -> void;

    /**
     * @brief Dump an expression.
     * @param expr The expression to dump
     */
    auto dump(Expr const &expr) -> void;

  private:
    /**
     * @brief Add a label in front of a node.
     * @note Should only be used before calling `start_node` or `dump`
     * @param label The label
     */
    auto label(std::string_view label) -> void;

    /**
     * @brief Start dumping a new node.
     * @param name The name of the node
     */
    auto start_node(std::string_view name) -> void;

    /**
     * @brief Add a field to a node being dumped.
     * @note Should only be used inbetween start_node and end_node.
     * @param what What the field should say
     */
    auto field(std::string_view what) -> void;

    /**
     * @brief End dumping the node.
     */
    auto end_node() -> void;

    /**
     * @brief Write the current indentation to @a out_.
     */
    auto write_indent() -> void;

    /**
     * @brief Indent by one.
     */
    auto indent() -> void;

    /**
     * @brief Dedent by one.
     */
    auto dedent() -> void;

  private:
    Context const &ctx_;     ///< The context which all nodes being dumped belong to.
    std::ostream &out_;      ///< The std::ostream to write to.
    std::size_t indent_{0};  ///< The current indentation level.
    std::string label_;      ///< The label to write in front of a sub-node.
  };

  /**
   * @brief Dump a module to it's string representation.
   * @param ctx The context the module belongs to
   * @param module The module
   * @return A string representing the module
   */
  auto dump(Context const &ctx, Module const &module) -> std::string;
}  // namespace iris::ast

#endif  // IRIS_AST_DUMP_HPP
