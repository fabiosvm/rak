
[![Ubuntu CI](https://github.com/fabiosvm/rak/actions/workflows/ubuntu.yml/badge.svg)](https://github.com/fabiosvm/rak/actions/workflows/ubuntu.yml) [![macOS CI](https://github.com/fabiosvm/rak/actions/workflows/macos.yml/badge.svg)](https://github.com/fabiosvm/rak/actions/workflows/macos.yml) [![Windows CI](https://github.com/fabiosvm/rak/actions/workflows/windows.yml/badge.svg)](https://github.com/fabiosvm/rak/actions/workflows/windows.yml)

# The Rak Programming Language

Rak is a dynamically typed scripting language that adopts a mutable value semantics approach.

> **Note:** This project is in the early stages of development and is not yet ready for use.

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

This project uses [CMake](https://cmake.org) to build, and to facilitate this, a build script is provided. So, to build, simply run the build script:

```
./build.sh
```

## Executing a script

Use `rak` to execute a script. Example:

```
./build/rak < examples/hailstone.rak
```

You can use the flag `-c` to just compile and display the bytecode, without executing it.

```
./build/rak -c < examples/hello.rak
```

Output:

```
1 constant(s)
5 instruction(s)
[0000] LOAD_GLOBAL     31
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

## License

Rak is licensed under the MIT License. See the [LICENSE](LICENSE) file for more information.
