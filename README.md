[![Actions Status](https://github.com/ruarq/iris/actions/workflows/macos.yml/badge.svg)](https://github.com/ruarq/iris/actions/workflows/macos.yml)
[![Actions Status](https://github.com/ruarq/iris/actions/workflows/windows.yml/badge.svg)](https://github.com/ruarq/iris/actions/workflows/windows.yml)
[![Actions Status](https://github.com/ruarq/iris/actions/workflows/ubuntu.yml/badge.svg)](https://github.com/ruarq/iris/actions/workflows/ubuntu.yml)
[![Actions Status](https://github.com/ruarq/iris/actions/workflows/style.yml/badge.svg)](https://github.com/ruarq/iris/actions/workflows/style.yml)
[![Actions Status](https://github.com/ruarq/iris/actions/workflows/install.yml/badge.svg)](https://github.com/ruarq/iris/actions/workflows/install.yml)
<!-- [![codecov](https://codecov.io/gh/TheLartians/ModernCppStarter/branch/master/graph/badge.svg)](https://codecov.io/gh/TheLartians/ModernCppStarter) -->

# The iris programming language
Modern, minimalist programming language with rust-like syntax.

## Hello, World!
```iris
import std

fn world(): str {
  ret "World!"
}

fn main() {
  print("Hello, {}\n", world())
}
```