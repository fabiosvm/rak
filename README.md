
# The Rak Programming Language

Rak is a dynamically typed scripting language that adopts a mutable value semantics approach.

> **Note:** This project is in the early stages of development and is not yet ready for use.

## Building

This project uses [CMake](https://cmake.org) to build, and to facilitate this, a build script is provided. So, to build, simply run the build script:

```
./build.sh
```

## Executing the REPL

Use `rak` to run the REPL:

```
./build/rak
```

> **Note:** The REPL just checks the syntax of the input code. It does not execute it.

## Cleaning

To clean the build files, run the clean script:

```
./clean.sh
```

## License

Rak is licensed under the MIT License. See the [LICENSE](LICENSE) file for more information.
