
[![Ubuntu CI](https://github.com/fabiosvm/rak/actions/workflows/ubuntu.yml/badge.svg)](https://github.com/fabiosvm/rak/actions/workflows/ubuntu.yml) [![macOS CI](https://github.com/fabiosvm/rak/actions/workflows/macos.yml/badge.svg)](https://github.com/fabiosvm/rak/actions/workflows/macos.yml) [![Windows CI](https://github.com/fabiosvm/rak/actions/workflows/windows.yml/badge.svg)](https://github.com/fabiosvm/rak/actions/workflows/windows.yml)

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

Example:

```
echo "Hello, world!";
```

> **Note:** You can use the flag `-d` to dump the bytecode.

## Cleaning

To clean the build files, run the clean script:

```
./clean.sh
```

## License

Rak is licensed under the MIT License. See the [LICENSE](LICENSE) file for more information.
