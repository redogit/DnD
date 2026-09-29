# Native Persistence Implementation Plan

> For agentic workers: use superpowers:executing-plans task-by-task.

**Goal:** Independently restore the actual engine's native data graph and resume equivalent work.
**Architecture:** Hash-bound generated snapshot API, canonical codec/archive, immutable durable files.
**Tech Stack:** C++23, Python 3 build generator, CMake 3.25+, OpenSSL 3 EVP, POSIX file operations.
**Spec:** docs/NATIVE_PERSISTENCE.md

## Global constraints
Preserve predecessor source bytes and failure lineage. No source engine on decoder input.
No serialized callbacks. Check exact identity/version binding. PR #2 is open; no implicit merge.

## Review focus
1. Failure edges do not appear in legacy parent child-edge lists; preserve that distinction.
2. Repeated input bindings are ordered and may repeat; do not deduplicate them accidentally.
3. Memoization and dependencies survive restore, including zero-output and failed invocations.
4. Valid digest is not valid graph, and a valid graph is not an admitted scientific conclusion.
5. Publishing the new file must not overwrite, truncate or unlink an existing checkpoint.

### Task 1 — Native snapshot API
Files: detail/checkpoint_types.inc; detail/checkpoint_methods.inc; scripts/prepare_native_engine.py;
        tests/native_snapshot_tests.cpp
- [x] Write/run missing-feature and field-preservation tests (RED).
- [x] Add export_checkpoint() and static restore_checkpoint(policy, checkpoint, registry).
- [x] Check full ID/link/dependency/counter/registry validity before import.
- [x] Run new test and all prior tests (GREEN); commit/ledger.

### Task 2 — Canonical archive and explicit native codec
Files: native_archive.hpp; self_state_codec.hpp; tests/native_archive_tests.cpp
- [x] Write exact native-field, canonical-order, identity/version, corruption and bounds tests (RED).
- [x] Implement pack_native/unpack_native with explicit codec, SHA-256 and resource bounds.
- [x] Preserve ordered bindings, NUL-containing strings, integer ranges and metadata float bits.
- [x] Run focused/full tests; commit/ledger.

### Task 3 — Durable create-only file handoff
Files: checkpoint_file.hpp; tests/checkpoint_process.cpp
- [x] Write absent-file, collision, truncation and separate writer/reader process tests (RED).
- [x] Implement save_new_checkpoint/read_checkpoint with declared POSIX guarantees.
- [x] Writer builds actual field/triadic native graph; reader restores only archive + code.
- [x] Resume with memoization/counter/dependency checks; run full test matrix.

### Task 4 — Review, evidence and GitHub successor
- [x] Re-read specification; run adversarial malformed-input probes and sanitizers.
- [x] Preserve observed failures and caveats in evidence/v0_6.
- [x] Update local CURRENT/README and source manifest; package exact tested source.
- [ ] Create atomic successor commit on current PR branch; verify remote blob hashes and PR state.
