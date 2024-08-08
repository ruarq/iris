add_subdirectory(lexer)
add_subdirectory(parser)

target_sources(iris-test
  PRIVATE
  iris.cpp
  main.cpp
  SourceRange.cpp
)