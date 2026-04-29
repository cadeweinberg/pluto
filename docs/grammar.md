
# EBNF Grammar (Draft)

```ebnf
module = { decl } ;

decl = "def" identifier [meta] [type] ["=" def] ";" ;

def  = function_body | literal | type | meta ;

function_body = "{" { block } "}" ;
block         = [identifier ":\n"] { instruction } ;
instruction   = [operand "="] opcode [meta] [operand {"," operand }] "\n" ;
operand       = identifier | literal ;

literal        = literal_tuple | array_literal | string | scalar_literal ;
tuple_literal  = "(" [literal {"," literal}] ")" ;
array_literal  = "[" {type literal} "]" ;
scalar_literal = integer | float | "true" | "false" ;

type          = function_type | tuple_type | array_type | pointer_type | scalar_type ;
function_type = type tuple_type ;
tuple_type    = "(" [type {"," type}] ")" ;
array_type    = "[" integer type "]" ;
pointer_type  = "ptr";
scalar_type   = "i8"   | "i16" | "i32" | "i64"
              | "f32"  | "f64"
              | "bool" | "nil" ;

meta        = tuple_meta | scalar_meta ;
tuple_meta  = "(" meta {"," meta} ")" ;
scalar_meta = identifier [type] [literal] ;
```

```regex
identifier = [!@%][-a-zA-Z$._][-a-zA-Z$._0-9]* ;
string     = '"' ([^"\\] | '\\' .)* '"' ;
integer    = [0-9][0-9]*;
fractional = [0-9]* "." [0-9]+ | [0-9]+ ".";
exponent   = 'e' [+-]? [0-9]+;
float      = fractional exponent? | [0-9]+ exponent;
```
