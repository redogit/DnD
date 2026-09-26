# CURRENT — RMAL 3.1 / RMALC 3.1 C23

**Date:** 2026-09-26

## Current implementation authority

The current implementation is **ISO C23**, with Windows x64 + Clang as a first-class target.

```text
RMAL source
 -> lexer
 -> parser
 -> AST
 -> RMALBC1
 -> VM
 -> manifest / trace / audit
```

The C++20 implementation is a preserved predecessor under `history/cpp20/`.

## Current semantic improvements over the C++20 predecessor

- `CONST` is enforced as immutable at runtime.
- `STATE` declaration and `SET` mutation are distinct operations.
- `SET` rejects undefined bindings.
- equality is typed: `1 != "1"` and `true != 1`.
- source line/column coordinates are carried into bytecode and traces.
- directive records preserve source position.
- manifest output is available as text or JSON.

## Current executable surface

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

Recovered declarative and RMALKDVMLLL-adjacent forms remain parsed carrier metadata unless separately implemented.

`PARSED_CARRIER_CURRENT != EXECUTABLE_CURRENT`

## Native build targets

### Windows

```powershell
.\scripts\build-windows-c23.ps1
```

Uses Clang, CMake, and Ninja.

### Portable / Linux

```bash
cmake -S . -B build -G Ninja -DCMAKE_C_COMPILER=clang -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

## Verification state

Local strict C23 verification has passed with:

```text
clang -std=c23 -Wall -Wextra -Wpedantic -Werror
7 / 7 CTest gates PASS
```

Repository-level Windows and Linux CI is defined in `.github/workflows/rmal-c23.yml`.

The current head is not considered **Windows-verified** until the exact-head `windows-latest` job succeeds.

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

## Claim ceiling

Until exact-head CI succeeds:

`NATIVE_C23_LOCAL_STRICT_BUILD_VERIFIED_WINDOWS_CI_PENDING`

Not universal compiler correctness. Not full 2.1.x parity. Not scientific validation.
