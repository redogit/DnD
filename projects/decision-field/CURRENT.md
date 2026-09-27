# CURRENT — Decision Field v0.6 native persistence

Date: 2026-09-27. Continuation of PR #2 at `21e85bd293745b2ff2096fc1000c7a7058f3880b`.
Root RMAL/RMALC C23 is unchanged; this continuation does not merge PR #2.

## Implemented

- Complete engine-owned data checkpoint with full native SelfState codec.
- Canonical versioned binary archive with SHA-256 and allocation guards.
- External version-bound callback registry, no serialized executable callbacks.
- Reciprocal native-link/dependency checks, increasing lineage depth, counter checks,
  invocation/cache/edge consistency and input-derived invocation-key verification.
- Create-only POSIX file publication with private staging, file fsync and directory fsync.
- Independent process load and continuation preserving cache, IDs, stale/failure evidence,
  repeated input bindings, zero-output receipts and zero-input generation.
- Original v0.3 source stays unchanged; the build generates an explicitly versioned successor.

## Measured fixture

Writer and independent reader agree on 30,376 native states, 5 transform edges and 5
invocations. Archive size is 8,755,589 bytes. The fixture includes the actual nearest and
farther triadic explorations, then five explicit transform invocations. This is not the
same fixture/count as the previous triadic report.

```text
SHA-256: 8ab9f2b15524ac08d3d8cdf8959e7da344a301cca9a35a51530f86868309670a
reader re-encoding equals writer bytes: YES
new invocation and next-ID continuation: YES
failed invocation / scoped fix / negative residual preserved: YES
existing checkpoint overwrite refused: YES
```

## Verification

GCC 14.2 Debug and Release with `-Werror`: 13/13 CTest targets each.
Clang 17 ASan+UBSan with `-Werror`: 13/13, no sanitizer findings in this run.
Standalone without predecessor: 4/4; native persistence is explicitly unavailable there.

Altered-body probes with recomputed checksums: 239 rejected, 17 structurally valid changes
accepted, out of 256 deterministic probes. This is not authentication or exhaustive fuzzing.

## Corrections found

An internally self-consistent but forged cache key could previously pass restore checks.
A failing regression test reproduced it. Restore now recomputes keys from transform
identity/version, ordered source fingerprints and captured context. Clang also required
an explicit unsigned-byte conversion in digest formatting; the repair did not change bytes.

## Remaining boundaries

- Quiescent engine data only: no thread/OS memory/process checkpoint.
- External quartet/field/SurfaceObject results are not owned by the engine and need their
  own persistence layer; their underlying engine state nodes are included.
- Complete native recovery is demonstrated for SelfState, not arbitrary codec implementations.
- The legacy context retains entropy/configuration fingerprints, not unavailable original bytes.
- File durability is implemented for a trusted POSIX directory/filesystem, not power-loss-tested.
  Windows durable file I/O is explicitly not implemented; no Windows verification is claimed.
- SHA-256 does not authenticate source or grant evidence authority.
- Structural operations still use predecessor callbacks, not unified first-class transforms.
- Live cohort scheduling, RMAL execution integration and independent review remain outstanding.
- Root C23 tests and GitHub Actions were not rerun/verified in this local continuation.

See `evidence/v0_6` for new evidence. Prior v0.5 reports are preserved as historical records.
