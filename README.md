# arjon-compiler

### The general idea:
The whole language follows a structure of `identifier: type = value` where line endings acts as an end of statement.
For example:
```
x: i32 = 5
y: i32 = x + 5
```
Functions follow the same structure:
```
add: (i32, i32) -> i32 = fn(a, b) {
    return a + b
}
sum: i32 = add(10, 15)
```
In this example `add` is the identifier, `(i32, i32) -> i32` is the type, and `fn(a, b) { ... }` is the value.

Functions can also be invoked like lambdas:
```
sum: i32 = fn(a: i32, b: i32) -> i32 {
    return a + b
}(10, 15)
```
But you will have to supply your own types for the parameters and return value.

It is also possible to evaluate blocks as expressions using the `<-` operator:
```
a: i32 = 10
b: i32 = {
    if(a > 5) {
        <- 5
    }
    <- a
}
```
Here `b` will become `min(a, 5)` which in this case is `5`.

Here is how to declare main:
```
main: () -> i32 = fn() {
    // We can declare a function inside a function
    foo: () -> i32 = fn() {
        return 0
    }
    return foo()
}
```
