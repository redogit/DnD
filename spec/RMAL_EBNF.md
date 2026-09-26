# RMAL 3 Core Grammar

This grammar describes the **current native C++ frontend**, not the full historical/semantic RMAL universe.

```ebnf
program      = { statement } EOF ;

statement    = module
             | const_decl
             | binding_decl
             | function
             | if_stmt
             | while_stmt
             | return_stmt
             | print_stmt
             | require_stmt
             | stop_stmt
             | directive
             | expr_stmt ;

module       = "MODULE" identifier [";"] ;

const_decl   = ("const" | "CONST") identifier "=" expression [";"] ;

binding_decl = ("let" | "LET" | "STATE" | "SET")
               identifier "=" expression [";"] ;

function     = "fn" identifier "(" [ identifier {"," identifier} ] ")" block ;

if_stmt      = "if" expression block ["else" block] ;
while_stmt   = "while" expression block ;
return_stmt  = "return" expression [";"] ;
print_stmt   = "print" expression [";"] ;

require_stmt = ("assert" | "ASSERT" | "REQUIRE") expression [";"] ;
stop_stmt    = "STOP" [";"] ;

block        = "{" { statement } "}" ;

expression   = literal
             | identifier
             | call
             | unary
             | binary
             | "(" expression ")" ;

call         = identifier "(" [ expression {"," expression} ] ")" ;

directive    = directive_name { token-on-same-line } [";"] ;

directive_name =
               "EXPORT" | "IMPORT" | "EXTERN" | "SURFACE" | "TARGET"
             | "TYPE" | "TRAIT" | "CLASS" | "RELATION" | "RELATE"
             | "OPERATOR" | "CONFIG" | "PROFILE" | "CONTEXT"
             | "CLAIM" | "EVIDENCE" | "RULE" | "LANGUAGE"
             | "ENTITY" | "ATTRIBUTE" | "BOUNDARY"
             | "PRESERVE" | "ALLOW" | "DENY" | "FORBID"
             | "OBLIGATION" | "INVARIANT" | "REMAINDER"
             | "COUNTERPROBE" | "VERIFY" | "CONTINUE" | "TRACE"
             | "FIND" | "PATH" | "LENS" | "SURVIVE"
             | "TAKE" | "GOAL" | "SCOPE" | "BOUND"
             | "GENERATE" | "FIT" | "CONSTRAIN" | "MUTATE"
             | "ROTATE" | "COMPOSE" | "CLASSIFY"
             | "BUILD" | "ARCHIVE" | "REPEAT" ;
```

## Expression precedence

High to low:

1. unary `!`, `-`
2. `*`, `/`, `%`
3. `+`, `-`
4. `<`, `<=`, `>`, `>=`
5. `==`, `!=`
6. `&&`
7. `||`

## Current semantic status

`CONST / LET / STATE / SET / fn / if / while / return / print / assert / REQUIRE / STOP`
are lowered to current VM behavior.

Directive forms are preserved as line-oriented carriers in RMALBC1 but do not yet imply typed semantic execution.

```text
PARSED_DIRECTIVE != EXECUTED_SEMANTICS
```

Legacy RMAL line-oriented statements remain accepted without semicolons where the current parser can determine statement boundaries by source line.

See `RMAL_LANGUAGE_SPEC_3.md` and `RMAL_COMPATIBILITY_MATRIX.md`.
