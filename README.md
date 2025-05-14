
[![Ubuntu CI](https://github.com/fabiosvm/rak/actions/workflows/ubuntu.yml/badge.svg)](https://github.com/fabiosvm/rak/actions/workflows/ubuntu.yml) [![macOS CI](https://github.com/fabiosvm/rak/actions/workflows/macos.yml/badge.svg)](https://github.com/fabiosvm/rak/actions/workflows/macos.yml) [![Windows CI](https://github.com/fabiosvm/rak/actions/workflows/windows.yml/badge.svg)](https://github.com/fabiosvm/rak/actions/workflows/windows.yml) [![Codacy Badge](https://app.codacy.com/project/badge/Grade/d85735ffbdd44a7797df2160bed7621d)](https://app.codacy.com/gh/fabiosvm/rak/dashboard?utm_source=gh&utm_medium=referral&utm_content=&utm_campaign=Badge_grade) [![Codacy Badge](https://app.codacy.com/project/badge/Coverage/d85735ffbdd44a7797df2160bed7621d)](https://app.codacy.com/gh/fabiosvm/rak/dashboard?utm_source=gh&utm_medium=referral&utm_content=&utm_campaign=Badge_coverage)

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

The following is an example of a Fibonacci function:

```rs
fn fib(n) {
  if n < 2 {
    return n;
  }
  return fib(n - 1) + fib(n - 2);
}

println(fib(8)); // 21
```

## Building

This project uses [CMake](https://cmake.org) and includes a build script to simplify the proces. To build, simply run:

```
./build.sh
```

## Running a script

Use `rak` to run a script by reading it from standard input.

```
./build/rak examples/fib.rak
```

You can use the `-c` flag to compile a script into bytecode and display it without executing.

```
./build/rak -c examples/hello.rak
```

Output:

```
; main
; 0 parameter(s), 1 constant(s), 6 instruction(s), 0 function(s)
[0000] LOAD_GLOBAL     33
[0001] LOAD_CONST      0
[0002] CALL            1
[0003] POP
[0004] PUSH_NIL
[0005] RETURN
```

## Testing

Check the dependencies before running the tests.

- **Python 3.9 or later** must be installed.

```
pip install -r tests/requirements.txt
```

> **Note:** that currently the only dependency is `pyYAML`, which is commonly pre-installed in most Linux OS.

Now you can run the tests:

```
./tests.sh
```

To generate a test coverage report in Linux, run the `test-coverage.sh` file. You'll need at least one of the coverage tools: 'lcov' or 'gcovr'. After running the script, it will display the location of the generated HTML report

## Cleaning

To clean the build files, run the clean script:

```
./clean.sh
```

> **Note:** Windows users may need to run `.bat` files instead of `.sh` files.

## Documentation

Documentation is available at [docs/README.md](docs/README.md).

## License

Rak is licensed under the MIT License. See the [LICENSE](LICENSE) file for more information.
