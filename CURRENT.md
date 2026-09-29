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

Exact implementation + packaging witness:

```text
verified source head:
a58fb0de292f1dd3fb7abf4660e96822ad270fab

workflow:
RMAL C23 Toolchain
run 36244043491

Linux / Clang / C23:            PASS
Windows x64 / Clang / C23:      PASS
Windows staged install:         PASS
Windows staged rmalc.exe smoke: PASS
Windows ZIP package:            PASS
Windows artifact upload:        PASS
```

Published workflow artifact:

```text
name: RMAL-3.1.0-windows-x64-c23
artifact id: 10906986534
size: 640713 bytes
```

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


## September 29 continuation — explicit native host / Mirror bridge

PR #3 extends the VM with opt-in native callback bindings and a C-compatible API
for C++ consumers. No function is bound by default; script shadowing, reentry,
duplicate bindings and invalid argument shapes are rejected.

`projects/decision-field/` now supplies one immutable FunctionObject / AnyFunctor
Mirror adapter, executed from RMAL rather than treated as directive metadata.
Its separate native checkpoint/restart verification and limitations are recorded
in `projects/decision-field/evidence/v0_7/VERIFICATION.md`.

Root compiler/ABI CI and the full native integration suite are separate evidence
scopes. The full native suite requires the exact original v0.3 source; the supplied
companion bundle includes it. No Windows durable checkpoint implementation or
universal AnyFunctor/compiler proof is implied.
