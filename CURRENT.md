# CURRENT — RMAL Native C++ Formalization

**Date:** 2026-09-26

## Current verified implementation head

`b9c07ac3fe8d15828dedf190d740565fb9be1baf`

GitHub Actions run `36207543711` completed successfully.

Verified at that head:

- C++20 configure/build;
- native self-check;
- `rmalc language-spec --format json`;
- legacy syntax check;
- recovered-surface compatibility check;
- preserved historical `RMALC.rmal` parser check;
- example execution;
- compiled directive manifest;
- audit;
- CTest.

## Current canonical executable surface

```text
MODULE
CONST
LET / let
STATE
SET
fn
if / else
while
return
print
assert / ASSERT
REQUIRE
STOP
```

## Current canonical parsed-carrier surface

Declarative, evidence, relation, configuration, query, and RMALKDVMLLL-adjacent forms are accepted as structured directive carriers where listed in `spec/RMAL_LANGUAGE_SPEC_3.md`.

`PARSED_CARRIER_CURRENT != EXECUTABLE_CURRENT`

## Current pipeline

```text
RMAL source
 -> lexer
 -> parser
 -> AST
 -> RMALBC1
 -> VM
 -> trace / audit
```

Compiled declarative directives are retained in bytecode metadata and exposed through `rmalc manifest`.

## Formal target

```text
semantic object
 -> RMAL IR
 -> RMAL-SIR2
 -> typed transform / verification layer
 -> RMALKDVMLLL reconstruction/admission discipline
 -> RMALBC1
 -> RMALOBJ1
 -> RMALEXE1
```

The unimplemented target stages remain explicit.

## Next implementation frontier

1. source-location/provenance-bearing directive records;
2. typed semantic-object IR;
3. lexical locals and true `CONST` immutability;
4. typed equality;
5. typed evidence/claim/context/relationship records;
6. RMAL-TFEML trace objects;
7. deterministic RMAL-SIR2 serialization;
8. RMALOBJ1 object emitter and linker boundary.

## Claim ceiling

`NATIVE_CPP_BUILD_AND_CURRENT_COMPATIBILITY_SURFACE_VERIFIED`

Not universal correctness. Not full 2.1.x parity. Not scientific validation.
