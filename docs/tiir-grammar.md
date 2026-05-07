# tiir grammar

I was thinking about code that looked like:

```tiir
[version: 1.0]

[!0]
symbol s : [14 i8] "Hello, World!\0";

[!1]
symbol puts: (ptr) -> ();

[!2]
Symbol main : () -> i32:
    mv %0, 7
    mv %1, A
    call %2, F, (%0, %1)
    ret %2

tag !file = (file: source.c)
tag !0 = [!file, line: 10, column: 1]
tag !1 = [!file, line: 12, column: 1]
tag !2 = [!file, line: 15, column: 1]
```
