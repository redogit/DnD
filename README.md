# RMAL

**RMAL — Ryan McMillan April Language**

This repository is now the canonical Git home for RMAL.

The former DnD working tree was retired by user direction on 2026-09-26. Its exact predecessor remains recoverable in Git history at:

`bf1a2fc39957351a779091d748366996e80e0573`

## Canonical implementation

RMAL is a real programming language implemented in C++20.

```text
RMAL source
  -> lexer
  -> parser
  -> AST
  -> semantic/compiler stages
  -> RMAL-SIR / RMAL-SIR2
  -> RMALBC1
  -> VM
  -> trace / audit / reconstruction
  -> RMALOBJ1
  -> RMALEXE1
```

Current native source:

- `include/rmal/rmal.hpp`
- `src/rmal.cpp`
- `src/main.cpp`
- `spec/RMAL_EBNF.md`

Build:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

## Canonical distinctions

```text
RMAL        = programming language
RMALC       = Ryan McMillan April Language Compiler / native toolchain
RMAL-SIR2   = typed semantic intermediate representation
RMALBC1     = executable bytecode carrier
RMALOBJ1    = relocatable/linkable object carrier
RMALEXE1    = linked executable bundle
RMALKDVMLLL = Ryan McMillan April Knowledge Decay Virtual Machine Link Layer Language
RMAL-TFEML  = Trace-First Execution Memory Layer
```

RMALKDVMLLL is the semantic VM/link/reconstruction/admission layer. It is related to RMAL but is not silently collapsed into the RMAL source grammar.

## Evidence boundaries

```text
SURFACE != SEMANTICS
GENERATE != VERIFY != ADMIT
TRACE != PROOF
LINKED != PROVED
COMPILED != SCIENTIFICALLY_VALID
AUDIT_PASS != SEMANTIC_TRUTH
HASH_INTEGRITY != CLAIM_VALIDITY
RELATED != SUPPORTS
SUCCESSOR != REWRITTEN_PREDECESSOR
```

## Centralized RMAL material

The `provenance/` tree contains copied RMAL artifacts from their source repositories with source identity preserved. Originals remain in those repositories so centralization does not destroy lineage.

See:

- `docs/RMAL_DEFINITIONS.md`
- `docs/RMAL_LINEAGE.md`
- `docs/RMAL_CHAT_RECOVERY_2026-09-26.md`
- `docs/RMAL_TFEML.md`
- `rmal/`
- `provenance/`
