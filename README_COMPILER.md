# RMALC 3.1 — Native ISO C23 Compiler

RMALC is the native compiler/runtime toolchain for **RMAL — Ryan McMillan April Language**.

## Current implementation

```text
Implementation language: ISO C23
Primary target: Windows x64
Primary compiler: Clang
Build: CMake + Ninja
```

The former C++20 implementation is preserved under `history/cpp20/`.

## Pipeline

```text
source
 -> lexer
 -> parser
 -> AST
 -> RMALBC1
 -> VM
 -> manifest
 -> trace
 -> audit
```

## Commands

```text
rmalc version
rmalc language-spec --format json
rmalc selfcheck
rmalc check file.rmal
rmalc compile file.rmal
rmalc manifest file.rmal --json
rmalc run file.rmal
rmalc trace file.rmal
rmalc audit file.rmal
```

## Windows

Build and verify:

```powershell
.\scripts\build-windows-c23.ps1
```

Build, stage, verify, and package:

```powershell
.\scripts\package-windows-c23.ps1
```

Package:

```text
dist/RMAL-3.1.0-windows-x64-c23.zip
```

## Semantic guarantees currently implemented

- immutable `CONST`;
- explicit `STATE` and `SET`;
- type-sensitive equality;
- source-coordinate-bearing bytecode/directives/traces;
- deterministic parser/compiler behavior for tested inputs;
- fail-closed runtime errors for invalid mutation, type use, division/modulo by zero, undefined names, and arity mismatch.

## Boundary

```text
COMPILED != SCIENTIFICALLY_VALID
TRACE != PROOF
FINITE_VERIFICATION != UNIVERSALITY
```
