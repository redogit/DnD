# RMAL Tool Chain — LLVM-Style Current Architecture

**RMAL** — Ryan McMillan April Language  
**RMALC** — 2.1.0  
**Snapshot** — `current-clean-20260918`  
**Semantic link layer** — RMALKDVMLLL v2

## Compiler pipeline

```text
RMAL source
   |
   v
surface frontends / parser
   |
   v
semantic analysis + ABI + contracts
   |
   v
DATA-DRIVEN DOMAIN SPECIALIZATION
   |  dependency closure / capability union / provenance / conflicts
   |
   +--> RMAL core IR
   |
   +--> RMAL-SIR2 proof/contract envelope  [partial]
   |
   v
verified optimizer (-O0..-O3)
   |
   v
RMALBC1
   |
   v
RMALOBJ1
   |
   v
linker
   |
   v
RMALEXE1
   |
   +--> VM/runtime
   +--> debugger
   +--> disassembler/manifest
   +--> audit/repro
```

## Backend path

```text
RMAL semantic program
   -> target capability preflight
   -> Python | C++23 | JavaScript | SQL | JSON | human surface
```

## LLVM-family correspondence

| RMAL | LLVM-family analogy |
|---|---|
| `rmalc` | `clang`-style compiler driver |
| RMAL core IR | LLVM IR-like core representation |
| Domain specialization | data-driven target/domain configuration layer |
| RMAL-SIR2 | MLIR / typed proof-contract envelope |
| `-O0..-O3` optimizer | `opt`/pass pipeline |
| RMALBC1 | portable compiled carrier / bitcode-like layer |
| RMALOBJ1 | relocatable object layer |
| `rmalc link` | `lld`-like linker |
| RMALEXE1 | linked executable bundle |
| `rmalc run` | `lli`-like runtime |
| `rmalc disasm` | `llvm-dis`/inspection |
| `rmalc debug` | structural/semantic `lldb`-like debugger |
| `rmalc transpile` | backend/code generation |
| `audit`, `repro`, `doctor` | integrity and reproducibility utilities |

This is an architectural analogy, not feature-parity with LLVM.

## Current command surface

```text
rmal
rmalc targets
rmalc check
rmalc compile
rmalc link
rmalc build
rmalc run-object
rmalc run
rmalc run-source
rmalc disasm
rmalc manifest
rmalc repro
rmalc close-remainders
rmalc library-list
rmalc library-text
rmalc library-build
rmalc book-of-books
rmalc debug
rmalc transpile
rmalc repl / rpl
rmalc doctor
rmalc audit
rmalc meta-solve
rmalc meta-verify
```

## Current limits

- RMAL-SIR2 exists, but full lowering and proof-state integration remain incomplete.
- Full PolyMeta proof-toolchain acceptance is not complete.
- The debugger is structural/semantic; interactive proof rewind is not complete.
- Backend lowering is fail-closed where semantics cannot be preserved.
- Finite verification does not establish universal correctness.
- P versus NP remains open.