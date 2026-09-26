# RMAL Compatibility Matrix

| Surface | Recovered status | C23 parser | VM semantics | Current classification |
|---|---|---:|---:|---|
| `CONST` | 2.1.x implemented | yes | yes | EXECUTABLE_CURRENT |
| `LET` / `let` | 2.1.x implemented | yes | yes | EXECUTABLE_CURRENT |
| `STATE` | 2.1.x implemented | yes | yes (binding) | EXECUTABLE_CURRENT |
| `SET` | 2.1.x implemented | yes | yes (binding update) | EXECUTABLE_CURRENT |
| `REQUIRE` | 2.1.x implemented | yes | yes (assert-like) | EXECUTABLE_CURRENT |
| `STOP` | 2.1.x implemented | yes | yes (halt) | EXECUTABLE_CURRENT |
| functions/recursion | C++ successor | yes | yes | EXECUTABLE_CURRENT |
| if/else/while | C++ successor | yes | yes | EXECUTABLE_CURRENT |
| `TYPE/TRAIT/CLASS` | recovered declaration family | yes | no typed semantics yet | PARSED_CARRIER_CURRENT |
| `RELATION/RELATE` | recovered declaration family | yes | no relation engine yet | PARSED_CARRIER_CURRENT |
| `ENTITY/ATTRIBUTE` | recovered executable-universe surface | yes | no entity store yet | PARSED_CARRIER_CURRENT |
| `CLAIM/EVIDENCE/BOUNDARY` | recovered evidence surface | yes | no evidence engine yet | PARSED_CARRIER_CURRENT |
| `CONFIG/PROFILE/LANGUAGE` | recovered compiler surface | yes | no config lowering yet | PARSED_CARRIER_CURRENT |
| `FIND/PATH/LENS/SURVIVE` | recovered universe queries | yes | no query engine yet | PARSED_CARRIER_CURRENT |
| RMALKDVMLLL compact surface | specified | carrier parse | no | SPECIFIED_TARGET |
| RMAL-SIR2 | partial predecessor implementation | not first-class | no | SPECIFIED_TARGET |
| RMALOBJ1/RMALEXE1 | predecessor architecture | no current emitter/linker | no | SPECIFIED_TARGET |
| S-expression frontend | predecessor architecture | no | no | SPECIFIED_TARGET |
| external backends | predecessor architecture | no | no | SPECIFIED_TARGET |

## Known semantic simplifications in current native C23 compiler

- `CONST` is not yet runtime-enforced as immutable.
- `STATE` and `SET` currently lower to the same global store mechanism.
- `REQUIRE` currently lowers to the same VM check as `assert`.
- directives are carried as bytecode metadata but not interpreted by a typed semantic engine.
- function locals are parameter-local; general lexical local scopes are not complete.
- current equality compares rendered values and should be replaced by typed equality.
- current trace records opcode execution, not full RMAL-TFEML trace objects.

These are explicit implementation debts, not hidden equivalences.


## RMAL 3.1 C23 delta

| Behavior | C++20 predecessor | C23 current |
|---|---|---|
| CONST immutability | not enforced | enforced |
| STATE vs SET | same store path | distinct declaration/mutation |
| SET undefined binding | implicit create possible | rejected |
| Equality | rendered-value comparison | typed comparison |
| Source coordinates | limited | bytecode/directive/trace |
| Windows target | not exact-head verified | CI-gated with Clang/C23 |
