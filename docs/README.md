
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

## Destructuring

Rak allows destructuring in let statements. This means you can extract values from compound values into separate variables.

```rs
let [x, y] = [1, 2];
println(x); // 1
println(y); // 2

```

If the number of variables is greater than the number of values, the remaining variables are assigned `nil`.

```rs
let [x, y, z] = [1, 2];
println(z); // nil
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

Strings are sequences of characters. They are constructed using double quotes `"`.

```rs
let s = "This is a string";
```

Use the `len` built-in function to get the length of a string.

```rs
println(len(s)); // 16
```

Besides the `+` operator add numbers, it can also be used to concatenate strings.

```rs
let s = "Hello, " + "world!";
println(s); // Hello, world!
```

## Arrays

Arrays are ordered collections of values. They are constructed using square brackets `[]` and can contain any type of value.

```rs
let a = [false, 3.14, "foo"];
```

You can access elements in an array using the index operator `[]`.

```rs
println(a[0]); // false
println(a[1]); // 3.14
println(a[2]); // foo
```

> **Note:** Arrays are 0-indexed, meaning that the first element is at index 0.

When you access an element that is out of bounds, Rak will panic.

```rs
println(a[3]); // ERROR: index out of bounds
```

Use the `len` function to get the length of an array.

```rs
println(len(a)); // 3
```

You can also use the `append` to add elements to the end of an array.

```rs
&a = append(a, 4);
println(a); // [false, 3.14, "foo", 4]
```

As strings, the `+` operator can also be used to concatenate arrays.

```rs
let a = [1, 2] + [3, 4];
println(a); // [1, 2, 3, 4]
```

## Ranges

Ranges are a representation of a range of integers. They are constructed using the `..` operator.

```rs
let r = 1..3;
```

You can access elements in a range using the index operator `[]`.

```rs
println(r[0]); // 1
println(r[1]); // 2
```

Ranges are exclusive, meaning that the last element is not included in the range.

```rs
println(r[2]); // ERROR: index out of bounds
```

Also, you can use the `len` to get the length of a range.

```rs
println(len(r)); // 2
```

> **Note:** Ranges can be used for slicing strings and arrays. Look at the Slicing section.

## Slicing

> (Documentation for this section is coming soon.)

## Records

> (Documentation for this section is coming soon.)

## Closures

> (Documentation for this section is coming soon.)
