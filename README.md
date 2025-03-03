# Compiler for PL/0 written in C

- [x] Lexer (needs better error handling).
- [_] Parser.

The concrete grammar of the language.

```ts
Program   ::= Block "." .
Block     ::= [ConstDecl] [VarDecl] [ProcDecl] Statement.
ConstDecl ::= "const" Ident "=" Number {"," Ident "=" Number} ";" .
VarDecl   ::= "var" Ident {"," Ident} ";" .
ProcDecl  ::= "procedure" Ident ";" Block ";" .
Statement ::= Ident ":=" Expression
            | "call" Ident
            | "begin" Statement {";" Statement} "end"
            | "if" Condition "then" Statement
            | "while" Condition "do" Statement .
Condition ::= "odd" Expression | Expression ("="|"#"|"<"|">"|"<="|">=") Expression.
Expression ::= [ "+"|"-" ] Term { ("+"|"-") Term }.
Term      ::= Factor { ("*"|"/") Factor }.
Factor    ::= Ident | Number | "(" Expression ")" .
```

Code snippet.

```c
const max = 100;
var x, y;
procedure fact;
    var n, f;
    begin
        n := max;
        f := 1;
        while n > 0 do
        begin
            f := f * n;
            n := n - 1;
        end
    end;
begin
    call fact;
end.
```
