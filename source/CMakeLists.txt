add_subdirectory(lexer)
add_subdirectory(ast)
add_subdirectory(parser)

target_sources(iris
  PRIVATE
  iris.cpp
  SourceRange.cpp
  File.cpp
  Context.cpp
)