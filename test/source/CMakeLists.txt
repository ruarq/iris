add_subdirectory(lexer)

target_sources(iris-test
  PRIVATE
  iris.cpp
  main.cpp
  SourceRange.cpp
)