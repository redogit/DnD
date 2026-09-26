# RMAL 3.1 / RMALC 3.1 — Windows C23

## Target

```text
OS: Windows x64
Language implementation: ISO C23
Compiler: Clang
Build system: CMake + Ninja
Executable: rmalc.exe
Library: rmal
```

## Build

From PowerShell:

```powershell
.\scripts\build-windows-c23.ps1
```

or from cmd.exe:

```bat
scripts\build-windows-c23.bat
```

Equivalent manual build:

```powershell
cmake -S . -B build/windows-c23 -G Ninja -DCMAKE_C_COMPILER=clang -DCMAKE_BUILD_TYPE=Release
cmake --build build/windows-c23 --parallel
ctest --test-dir build/windows-c23 --output-on-failure
```

## Compiler surface

```text
rmalc version
rmalc language-spec --format json
rmalc selfcheck
rmalc check <file.rmal>
rmalc compile <file.rmal>
rmalc manifest <file.rmal> --json
rmalc run <file.rmal>
rmalc trace <file.rmal>
rmalc audit <file.rmal>
```

## Current executable semantics

- immutable CONST;
- mutable LET/STATE;
- SET mutation with existence and immutability checks;
- functions and recursion;
- if/else and while;
- int64, bool, string, nil;
- typed equality;
- arithmetic and comparisons;
- REQUIRE/assert;
- STOP;
- directive metadata with source coordinates;
- RMALBC1 execution;
- trace and audit surfaces.

## Evidence boundary

A successful Windows build verifies this implementation surface at the tested revision.

```text
WINDOWS_BUILD_PASS != UNIVERSAL_CORRECTNESS
COMPILED != SCIENTIFICALLY_VALID
TRACE != PROOF
```
