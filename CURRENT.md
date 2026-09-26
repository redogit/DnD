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

Native C23 has passed both local strict verification and GitHub Actions on Linux and Windows.

Exact implementation witness:

```text
verified source head:
b6b9c6819b13017efdeac0047d4ef366b067449a

workflow:
RMAL C23 Toolchain
run 36231758598

Linux / Clang / C23:  PASS
Windows x64 / Clang / C23: PASS
```

The Windows witness used Clang 20.1.8 targeting `x86_64-pc-windows-msvc` on Windows Server 2025. Configure, compile, RMALC self-check, language-spec introspection, recovered-surface checking, historical RMALC carrier checking, example execution, manifest generation, and the CTest semantic regression suite all passed.

Evidence: `evidence/RMAL_C23_WINDOWS_VALIDATION_2026-09-26.json`.

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

`NATIVE_C23_WINDOWS_AND_LINUX_EXACT_HEAD_VERIFIED`

Not universal compiler correctness. Not full 2.1.x parity. Not scientific validation.
