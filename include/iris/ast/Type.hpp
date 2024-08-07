//
// Created by ruarq on 06/08/2024.
//

#ifndef IRIS_AST_TYPE_HPP
#define IRIS_AST_TYPE_HPP

#include <memory>
#include <variant>

namespace iris::ast {
  struct BasicType;
  struct NameType;
  struct ArrayType;

  using Type = std::variant<BasicType, NameType, ArrayType>;
  using TypePtr = std::unique_ptr<Type>;

  enum class BasicTypeKind {
    Int8,
    Int16,
    Int32,
    Int64,
    UInt8,
    UInt16,
    UInt32,
    UInt64,
    ISize,
    USize,
    Float32,
    Float64,
    Bool,
    Str,
    Unit,
  };

  struct BasicType {
    BasicTypeKind kind;
  };

  struct NameType {
    Identifier identifier;
  };

  struct ArrayType {
    TypePtr element_type;
    std::size_t element_count;
  };
}  // namespace iris::ast

#endif  // IRIS_AST_TYPE_HPP
