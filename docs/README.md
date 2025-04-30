
# The Rak Programming Language

Rak is a simple, cross-platform, dynamically typed scripting language designed with a strict **Mutable Value Semantics** model.

> **Note:** This is a work in progress. The language is not yet complete and the documentation may change.

## Hello, world!

When learning a new programming language, the first thing you usually do is print "Hello, world!" to the console. In Rak, you can do this using the `println` function.

```rs
println("Hello, world!");
```

## Semicolons

Semicolons are important in Rak. They are used to separate statements and are not optional.

```rs
let x;
```

There is no empty statement. A semicolon by itself is not a valid statement.

```rs
; // ERROR: unexpected token ';' at 1:1
```

## Identifiers

Identifiers are used to name variables, functions, and other entities. They must start with a letter or an underscore, followed by letters, digits, or underscores. Also, identifiers are case-sensitive.

```rs
x
_temp
PI
```

> **Note:** Keywords cannot be used as identifiers.

## Keywords

The following keywords are reserved and cannot be used as identifiers:

- `break`
- `continue`
- `do`
- `else`
- `false`
- `if`
- `let`
- `loop`
- `nil`
- `true`
- `while`

## Types

Rak is a dynamically typed language. This means that type information is associated with values, not with variables.

The following types are supported:

| Type | Description |
|---|---|
| `Nil` | The type of the `nil` value. |
| `Bool` | `false` or `true`. |
| `Number` | 64-bit floating point number. |
| `String` | A sequence of characters. |
| `Array` | An ordered collection of values. |
| `Range` | A representation of a range of integers. |
| `Record` | A collection of fields. |
| `Closure` | Function or native function. |

## Falsy values

Only two values are considered falsy in Rak:

- `nil`
- `false`

## Literals

Literals are used to represent values in the source code. The following literals are supported:

- `nil` represents the absence of a value.
- `false` represents the boolean false.
- `true` represents the boolean true.
- Numbers are written as sequences of digits.
- Strings are written as sequences of characters enclosed in double quotes.

Examples:

```rs
nil
false
true
42
3.14
"Hello, world!"
```

## Variables

Variables are declared using the `let` keyword followed by an identifier.

```rs
let x = 1;
```

After a variable is declared, it can be used in expressions.

```rs
println(x + 2); // 3
```

Variables can be declared without an initial value, in which case they are initialized to `nil`.

```rs
let x;
println(x); // nil
```

## Scope

Variables are block-scoped and Rak allows shadowing declarations of variables in inner scopes.

```rs
let x = 1;
{
  let x = 2;
  println(x); // 2
}
println(x); // 1
```

## Assignment

In Rak, assignment is explicit. The `&` symbol indicates that you intend to mutate a value. This strictness helps prevent accidental side effects.

```rs
&x = "foo";
&x = [1, 2, 3];
&x += 1;
```

The following assignment operators are supported:

| Operator | Description |
|---|---|
| `=` | Assigns the value to the left-hand side. |
| `+=` | Adds the value to the left-hand side. |
| `-=` | Subtracts the value from the left-hand side. |
| `*=` | Multiplies the left-hand side by the value. |
| `/=` | Divides the left-hand side by the value. |
| `%=` | Takes the remainder of the left-hand side by the value. |

## Expressions

Expressions are used to compute values.

```rs
1 + 2
x + 1
x * (y + 1)
```

## Arithmetic operators

The following arithmetic operators are supported:

| Operator | Description |
|---|---|
| `+` | Addition |
| `-` | Subtraction |
| `*` | Multiplication |
| `/` | Division |
| `%` | Remainder |
| `-` (unary) | Negation |

> (Examples for this section are coming soon.)

## Equality and comparison operators

The following equality and comparison operators are supported:

| Operator | Description |
|---|---|
| `==` | Equal to |
| `!=` | Not equal to |
| `>=` | Greater than or equal to |
| `>` | Greater than |
| `<=` | Less than or equal to |
| `<` | Less than |

> (Examples for this section are coming soon.)

## Logical operators

The following logical operators are supported:

| Operator | Description |
|---|---|
| `\|\|` | Logical OR |
| `&&` | Logical AND |
| `!` | Logical NOT |

> (Examples for this section are coming soon.)

## If statements

If statements are used to execute a block of code conditionally.

```rs
if x > 0 {
  println("x is zero");
}
```

It is also possible to use `else` and `else if` to create more complex conditional statements.

```rs
if x > 0 {
  println("x is positive");
} else if x < 0 {
  println("x is negative");
} else {
  println("x is zero");
}
```

Rak allows a variable before the expression in an if statement.

```rs
if let x = 1; x > 0 {
  println(x);
}
```

> **Note:** `x` is declared in the scope of the block.

## If expressions

It is also possible to use if in expressions. This is useful when you want to assign a value based on a condition.

```rs
let x = 1;
let y = if x > 0 { "positive" } else if x < 0 { "negative" } else { "zero" };
println(y); // positive
```

If you omit the `else` branch, the expression will return `nil` if the condition is false.

```rs
&x = 0;
let y = if x > 0 { "positive" };
println(y); // nil
```

## Loop statements

Loop statements are used to execute a block of code repeatedly.

```rs
loop {
  println("Press Ctrl+C to stop");
}
```

In loops, you can use `break` to exit the loop and `continue` to skip the current iteration.

```rs
let i = 0;
loop {
  &i += 1;
  if i < 5 {
    continue;
  }
  if i > 10 {
    break;
  }
  println(i);
}
```

## While statements

While statements are used to execute a block of code repeatedly while a condition is true.

```rs
let i = 0;
while i < 10 {
  println(i);
  &i += 1;
}
```

There is also a `while let` statement that allows you to declare a variable before the expression.

```rs
while let i = 0; i < 10 {
  println(i);
  &i += 1;
}
```

## Do-while statements

Do-while statements are used to execute a block of code at least once and then repeatedly while a condition is true.

```rs
let i = 0;
do {
  println(i);
  &i += 1;
} while i < 10;
```

## Strings

> (Documentation for this section is coming soon.)

## Arrays

> (Documentation for this section is coming soon.)

## Ranges

> (Documentation for this section is coming soon.)

## Records

> (Documentation for this section is coming soon.)

## Closures

> (Documentation for this section is coming soon.)
