
# The Rak Programming Language

Rak is a simple, cross-platform, dynamically typed scripting language designed with a strict **Mutable Value Semantics** model.

> **Note:** This is a work in progress. The language is not yet complete and the documentation may change.

## Hello, world!

When learning a new programming language, the first thing you usually do is to print "Hello, world!" to the console. In Rak, you can do this using the `println` function.

```rs
println("Hello, world!");
```

## Comments

Comments are used to add notes to your code. They are ignored by the compiler and are not executed.

```rs
// This is a single-line comment
let x; // This is also a single-line comment
```

To write a multi-line comment, simply use multiple single-line comments:

```rs
// This is a
// multi-line
// comment
```

## Semicolons

Semicolons are important in Rak. They are used to separate statements and are not optional.

```rs
let x = 42;
```

Empty statements are not allowed; a semicolon by itself is not a valid statement.

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
- `fn`
- `if`
- `let`
- `loop`
- `nil`
- `return`
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

Rak allows destructuring in `let` statements. This means you can extract values from compound values into separate variables.

Look at the following example where we destructure an array:

```rs
let [x, y] = [1, 2, 3];
println(x); // 1
println(y); // 2

```

If the number of variables is greater than the number of values, the remaining variables are assigned `nil`.

```rs
let [x, y, z] = [1, 2];
println(z); // nil
```

You can use the placeholder `_` to ignore values you don't need.

```rs
let [_, x] = [1, 2, 3];
println(x); // 2
```

Now, let's destructure a record:

```rs
let {y, z} = {x: 1, y: 2, z: 3};
println(y); // 2
println(z); // 3
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

Examples:

```rs
println(1 + 2);  // 3
println(5 * 2);  // 10
println(10 / 2); // 5
println(5 % 2);  // 1
println(-42);    // -42
```

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

Examples:

```rs
println(1 == 1); // true
println(1 != 2); // true
println(2 >= 2); // true
println(3 > 2);  // true
println(2 <= 2); // true
println(1 < 2);  // true
```

## Logical operators

The following logical operators are supported:

| Operator | Description |
|---|---|
| `\|\|` | Logical OR |
| `&&` | Logical AND |
| `!` | Logical NOT |

Examples:

```rs
println(true && false); // false
println(true || false); // true
println(!false);        // true
```

## If statements

If statements are used to execute a block of code conditionally.

```rs
if x > 0 {
  println("x is positive");
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

Besides adding numbers, the `+` operator can also be used to concatenate strings.

```rs
let s = "Hello, " + "world!";
println(s); // Hello, world!
```

Strings can be sliced using the `[]` operator and a range.

```rs
let s = "Hello, world!";
println(s[0..5]); // Hello
println(s[7..12]); // world
```

> **Note:** The range is exclusive, meaning that the last character is not included in the slice.

## Arrays

Arrays are ordered collections of values, enclosed in square brackets `[]`. Each element can be of any type, and values are accessed by their zero-based index.

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

Rak allows you to use the `&` operator to mutate arrays. This means you can modify individual elements directly.

```rs
let a = [1, 2, 3];
&a[0] = 4;
println(a); // [4, 2, 3]
```

Rak allows mutation of compound values and sharing of references for performance reasons, but ensures that mutation effects are never shared between references.

```rs
let a = [1, 2, 3];
let b = a;
&b[0] = 4;
println(a); // [1, 2, 3]
println(b); // [4, 2, 3]
```

As strings, the `+` operator can also be used to concatenate arrays.

```rs
let a = [1, 2] + [3, 4];
println(a); // [1, 2, 3, 4]
```

Arrays can be sliced too.

```rs
let a = [1, 2, 3, 4, 5];
println(a[0..3]); // [1, 2, 3]
println(a[2..5]); // [3, 4, 5]
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

Remember that ranges are exclusive. So...

```rs
println(r[2]); // ERROR: index out of bounds
```

Also, you can use the `len` to get the length of a range.

```rs
println(len(r)); // 2
```

## Records

Records are ordered collections of key-value pairs, enclosed in curly braces `{}`. Each key is a string, and each value can be of any type. They are commonly used to represent structured data, similar to objects or dictionaries in other languages.

```rs
let jedi = {
  name: "Yoda",
  age: 900,
  species: "Unknown"
};
```

To access the values in a record, use the `[]` or `.` operator.

```rs
println(jedi["name"]); // Yoda
println(jedi.age); // 900
```

If you try to access a field that does not exist, Rak will panic.

```rs
println(jedi.level); // ERROR: record has no field named 'level'
```

Use `len` to get the number of fields in a record.

```rs
println(len(jedi)); // 3
```

You can also use the `&` operator with `[]` or `.` to mutate records.

```rs
&jedi["age"] = 990;
println(jedi.age); // 990
&jedi.age = 1000;
println(jedi.age); // 1000
```

New fields can be added to a record.

```rs
&jedi.level = "master";
println(jedi); // {name: Yoda, age: 1000, species: Unknown, level: master}
```

## Closures

Every function in Rak is a closure. This means that they are first-class citizens, can be passed as arguments, returned from other functions, and can capture values from their surrounding scope. To define a closure, use the `fn` keyword followed by the name, parameters, and the statement block.

```rs
fn add(x, y) {
  return x + y;
}
```

> **Note:** The `return` keyword is used to return a value from a closure. If you omit it, `nil` is returned by default.

You can call a closure by using its name followed by parentheses `()` and passing the arguments inside.

```rs
println(add(1, 2)); // 3
```

> **Note:** The return value of `println` is discarded because it is called in a statement.

Like other values, closures can be assigned to variables.

```rs
let sub = fn (x, y) {
  return x - y;
};
println(sub(5, 2)); // 3
```

You can also pass closures as arguments to other closures.

```rs
fn apply(f, x, y) {
  return f(x, y);
}
println(apply(add, 1, 2)); // 3
println(apply(sub, 7, 3)); // 4
```

Returning a closure from another closure.

```rs
fn adder() {
  return fn (x, y) {
    return x + y;
  };
}
let add = adder();
println(add(5, 2)); // 7
```

Recursion is also possible in Rak. A closure can call itself.

```rs
fn factorial(n) {
  if n == 0 {
    return 1;
  }
  return n * factorial(n - 1);
}
println(factorial(5)); // 120
```

Be careful with recursion, as it can lead to stack overflow if the recursion depth is too high.

```rs
println(factorial(130)); // ERROR: too many nested calls
```

To avoid this, maybe you can use a tail-recursive approach.

```rs
fn factorial(n, acc) {
  if n == 0 {
    return acc;
  }
  return factorial(n - 1, n * acc);
}
println(factorial(130, 1)); // 6.46686e+219
```

## Built-in functions

> (Documentation for this section is coming soon.)
