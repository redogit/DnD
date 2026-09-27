# Native-state persistence — v0.6 design

## Objective
Persist and restore every data field of the actual DecisionFieldEngine v0.3 state,
not only the v0.5 numeric projection. Preserve complete native values, ordered parent/
child lists, transform edges and recovery receipts, successful and failed invocations,
memoization entries, dependency indexes, counters, obligations, scoped fixes, concessions,
evidence and negative residual notes. New work continues on PR #2; master is not changed.

## Chosen implementation
Use an explicit EngineCheckpoint<T> export/restore API, a canonical versioned binary
archive, SHA-256 integrity through OpenSSL EVP, and create-only durable file storage.
A hash-checked build step injects the snapshot API into a generated successor header;
the exact v0.3 source remains unchanged. Export/import does not reinterpret or repair
captured evidence. Imported callable behavior is never supplied by archive bytes:
a matching externally compiled policy, native value codec, and transform registry are
required. Archive identity binds the engine profile, policy and codec versions and run.

A projection-only save is insufficient. Raw object-memory dumps are rejected because
pointers and container memory are not a portable encoding. A new database/runtime is
unnecessary for this bounded checkpoint slice.

## Contract
- Quiescent capture: callers synchronize before exporting; this engine is not concurrent.
- Canonical ordering applies to unordered maps/sets; vector order and multiplicity survive.
- Every native T has an explicit versioned codec and exact equality check. Current concrete
  codec covers SelfState's stance, markers and orientation, not stance alone.
- Unknown schema/profile/codec/policy, malformed or excessive lengths, duplicate keys,
  orphaned links, cyclic lineage, invalid counters, invalid addresses and broken reciprocal
  dependencies reject loading before any live engine is replaced.
- Old failed invocations and stale states remain facts. Valid structure is not evidence admission.
- Exact numerical bits in metadata are preserved. Fingerprints are checked using the supplied
  policy but are not a substitute for the full native value encoding.
- File persistence is immutable: a second save to the same path never overwrites its predecessor.
  POSIX implementation uses private same-directory staging, file fsync, no-replace hard-link
  publication, and directory fsync. No claim of power-loss simulation or hostile directory safety.
- SHA-256 detects corruption relative to trusted bytes/digest; it is neither authentication
  nor encryption. Archive files contain evidence and must be access controlled.
- Resource limits bound bytes, strings and aggregate collection entries before allocation.
- A fresh process loads without an original engine, then resumes a known transform and checks
  memoization, ID allocation and dependency invalidation. Snapshot retains all depth.

## Scope boundaries
The native engine does not own external quartet/field/surface result objects or executable
callback bodies. Those external result objects and an OS/process checkpoint are not claimed
as part of this engine-data checkpoint. Domain codecs are supplied explicitly. Legacy captured
entropy contains fingerprints only: missing entropy bytes are not invented. Generic structural
callbacks remain a separate unification obligation. Windows durable I/O is not admitted by a
Linux-only test; archive encoding itself is platform-neutral for the specified primitive widths.

## Verification
Existing nine tests first. Then native round-trip, full 30k-state surface lineage, failed and
zero-output transforms, repeated input bindings, generator receipts, missing callback/version
rejection, stale-state retention, canonical order, damaged/truncated archives, malformed graphs,
resource-limit attacks, create-only storage and process-separated read/resume. Run GCC Debug,
Release and Clang ASan/UBSan. Record actual commands, sizes, counts and scope, no invented CI.
