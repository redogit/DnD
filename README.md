# RMAL

**RMAL — Ryan McMillan April Language**

This repository is the canonical Git home for RMAL and RMALC.

The former DnD working tree remains recoverable in Git history at `bf1a2fc39957351a779091d748366996e80e0573`.

## Canonical implementation

**RMAL 3.1 / RMALC 3.1 are implemented in ISO C23.**

Primary native target:

```text
Windows x64
Clang
ISO C23
CMake + Ninja
```

The former C++20 implementation is preserved under `history/cpp20/` as predecessor evidence.

Current native source:

- `include/rmal/rmal.h`
- `src/rmal.c`
- `src/c23/`
- `src/main.c`
- `spec/RMAL_EBNF.md`
- `spec/RMAL_LANGUAGE_SPEC_3.md`

Build on Windows:

```powershell
.\scripts\build-windows-c23.ps1
```

or:

```bat
scripts\build-windows-c23.bat
```

Build and package the Windows x64 SDK:

```powershell
.\scripts\package-windows-c23.ps1
```

Output:

```text
dist/RMAL-3.1.0-windows-x64-c23.zip
```

Portable CMake build:

```bash
cmake -S . -B build -G Ninja -DCMAKE_C_COMPILER=clang -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

## Current executable path

```text
RMAL source
  -> C23 lexer
  -> parser
  -> AST
  -> RMALBC1 lowering
  -> VM
  -> manifest / trace / audit
```

Current executable behavior includes:

- `MODULE`
- immutable `CONST`
- `LET`
- `STATE` / `SET`
- functions and recursion
- `if / else`
- `while`
- `return`
- integer, boolean, string, and nil values
- typed equality
- arithmetic and comparisons
- `print`
- `assert / REQUIRE`
- `STOP`
- recovered RMAL declarative forms preserved as compiled directive metadata
- source line/column provenance in bytecode and trace records

## Canonical distinctions

```text
RMAL        = programming language
RMALC       = Ryan McMillan April Language Compiler / native toolchain
RMAL-SIR2   = typed semantic intermediate representation target
RMALBC1     = executable bytecode carrier
RMALOBJ1    = relocatable/linkable object carrier target
RMALEXE1    = linked executable bundle target
RMALKDVMLLL = Ryan McMillan April Knowledge Decay Virtual Machine Link Layer Language
RMAL-TFEML  = Trace-First Execution Memory Layer
```

## Evidence boundaries

```text
SURFACE != SEMANTICS
PARSED != EXECUTED
GENERATE != VERIFY != ADMIT
TRACE != PROOF
LINKED != PROVED
COMPILED != SCIENTIFICALLY_VALID
AUDIT_PASS != SEMANTIC_TRUTH
RELATED != SUPPORTS
SUCCESSOR != REWRITTEN_PREDECESSOR
```

## Corpus

Historical RMAL programs, inputs, outputs, Library artifacts, Git-native artifacts, and chat-recovered definitions are preserved under `examples/`, `provenance/`, `docs/`, and `rmal/`.

See `CURRENT.md` for the current verified implementation state.
