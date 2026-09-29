# Decision Field v0.6 — native engine persistence

This project extends the recoverable Decision Field inside `redogit/DnD` without changing
root RMAL/RMALC C23 authority. v0.5 connects exploration to component/core/surface replay.
v0.6 persists the engine-owned native data graph and restores it in another process.

## Current execution

```text
actual v0.3 engine -> synchronized snapshot -> canonical DFNAT001 archive
-> create-only POSIX file -> independent process -> checked native reconstruction
-> explicit callback rebinding -> memoization / dependency / ID-preserving continuation
```

The snapshot includes full native values, all state links, depths and provenance, transform
edges and recovery receipts, successful and failed invocation records, cache indexes,
obligations, counters, dependency indexes, scoped fixes, concessions and residual notes.

The concrete SelfState codec preserves stance, orientation and marker sets. Other domains
must provide an explicit exact native codec. This is not a dump of pointers/object memory.

## Build the complete source bundle

Requirements: C++23 compiler, CMake >= 3.25, Python 3, OpenSSL >= 3 development library.
Native durable-file tests are enabled on POSIX; Windows file I/O is not implemented.

```sh
cmake -S projects/decision-field -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel 2
ctest --test-dir build --output-on-failure
```

Linux with the exact predecessor source enables 13 tests. The original source is unchanged;
`scripts/prepare_native_engine.py` verifies its SHA-256 and generates a snapshot-capable
successor header only for the native targets. Older tests still use the exact original.

## Git-only recovery

The repository contains the new implementations, tests and reconstruction instructions.
The companion bundle includes the original 13-file predecessor. For a Git-only checkout,
use `scripts/recover_predecessor.py` with the exact original v0.3 archive. Without it,
CMake explicitly enables only the four standalone v0.5 component/bridge tests.
Use `-DDF_ENABLE_NATIVE_PERSISTENCE=OFF` to run the nine predecessor/bridge tests without
requiring OpenSSL. That configuration does not test persistence.

## Files to read

- `CURRENT.md`: observed state and boundaries.
- `docs/NATIVE_PERSISTENCE.md`: checkpoint specification.
- `docs/plans/2026-09-27-native-persistence.md`: implementation plan.
- `evidence/v0_6/VERIFICATION.md`: fresh commands and results.
- `evidence/v0_6/REVIEW.md`: defect, counterprobe and remaining limits.
- `evidence/CONVERSATION_STRATEGIC_INPUTS.md`: original user direction.
- `instructions/OPERATING_PROTOCOL.md`: previously accepted design rules.

## Boundaries

Capture is quiescent, not a concurrent process snapshot. Policy/callback code and result
objects owned outside the engine are not serialized. A matching external policy, codec
and transform registry must be supplied at restore. SHA-256 is integrity relative to a
trusted digest, not authentication, encryption or scientific admission. An altered archive
with a recomputed checksum can still describe a structurally valid different state.

No state is discarded because it failed, became stale or recursed deeply.

```text
SIMPLER CORE != ERASED DIFFERENCES
CHECKPOINT RESTORED != CLAIM PROVEN
METHOD TRANSFER != EVIDENCE TRANSFER
SUCCESSOR != REWRITTEN PREDECESSOR
```
