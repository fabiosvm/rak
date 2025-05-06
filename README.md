
[![Ubuntu CI](https://github.com/fabiosvm/rak/actions/workflows/ubuntu.yml/badge.svg)](https://github.com/fabiosvm/rak/actions/workflows/ubuntu.yml) [![macOS CI](https://github.com/fabiosvm/rak/actions/workflows/macos.yml/badge.svg)](https://github.com/fabiosvm/rak/actions/workflows/macos.yml) [![Windows CI](https://github.com/fabiosvm/rak/actions/workflows/windows.yml/badge.svg)](https://github.com/fabiosvm/rak/actions/workflows/windows.yml)

# The Rak Programming Language

Rak is a simple, cross-platform, dynamically typed scripting language designed with a strict **Mutable Value Semantics** model.

> **Note:** This project is in the early stages of development and is not yet ready for use.

## Features

Rak offers the following key features:

- **Imperative and functional**: Combines imperative and functional programming paradigms.
- **Familiar syntax**: Inspired by C, making it easy to learn.
- **Cross-platform**: Runs on Linux, macOS, and Windows.
- **Dynamic typing**: Variables do not have types; only values do.
- **Mutable value semantics**: Enables safe and efficient memory management by allowing mutable values to be shared, while ensuring that mutations do not produce unintended side effects.
- **Embeddable**: Can be embedded into C applications.
- **Extensible**: Easily extended with native functions.

## What does Rak look like?

The following is an example of a simple script that prints a hailstone sequence:

```rs
let n = 7;

let seq = [n];
while n != 1 {
  if n % 2 == 0 {
    &n /= 2;
  } else {
    &n = n * 3 + 1;
  }
  &seq += [n];
}

println(seq);
```

## Building

This project uses [CMake](https://cmake.org) and includes a build script to simplify the proces. To build, simply run:

```
./build.sh
```

## Running a script

Use `rak` to run a script by reading it from standard input.

```
./build/rak examples/hailstone.rak
```

You can use the `-c` flag to compile a script into bytecode and display it without executing.

```
./build/rak -c examples/hello.rak
```

Output:

```
1 constant(s)
5 instruction(s)
[0000] LOAD_GLOBAL     33
[0001] LOAD_CONST      0
[0002] CALL            1
[0003] POP
[0004] HALT
```

## Cleaning

To clean the build files, run the clean script:

```
./clean.sh
```

## Running Test Cases

### Dependencies
- **Python 3.9 or later** must be installed.
- Install required Python packages `pip install -r requirements.txt`. Note that currently the only dependency is pyYAML, which is commonly pre-installed in most Linux OS.

### Running Tests
- Execute the appropriate script for your OS:
  - Linux/macOS: `./tests.sh`
  - Windows: `test.bat`

- Available command line options:
  - `-q` or `--quiet`: Suppress detailed output, showing only test summary.
  - `<filter_string>`: Run only tests containing this string. Helpful when debugging specific cases.

## Documentation

Documentation is available at [docs/README.md](docs/README.md).

## License

Rak is licensed under the MIT License. See the [LICENSE](LICENSE) file for more information.
