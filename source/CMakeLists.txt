add_subdirectory(lexer)

target_sources(iris
  PRIVATE
  iris.cpp
  SourceRange.cpp
  File.cpp
  Context.cpp
)