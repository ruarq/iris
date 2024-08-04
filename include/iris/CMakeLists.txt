add_subdirectory(lexer)
add_subdirectory(ast)
add_subdirectory(parser)

target_sources(iris
  PRIVATE
  iris.hpp
  version.hpp
  SourceRange.hpp
  Context.hpp
  File.hpp
)