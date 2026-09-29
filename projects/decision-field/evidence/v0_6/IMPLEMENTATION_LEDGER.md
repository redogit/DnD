# Ledger — plan: docs/plans/2026-09-27-native-persistence.md

Ruling: proceed without additional design questions per the user's standing instruction.
Ruling: current PR #2 remains open; add a forward-only successor rather than merging implicitly.
Ruling: preserve source v0.3 bytes; generate an explicit snapshot-enabled successor at build time.
Ruling: engine-owned state is complete within the native checkpoint; external result objects and
callback code must not be falsely advertised as serialized runtime state.
Pre-flight: Task 2 consumes EngineCheckpoint<T> from Task 1; Task 3 consumes canonical bytes from
Task 2. All data import validates before installing a live engine. Source snapshot baseline is
companion archive SHA-256 89508eaeea3fd62945e176fb4a2d6439989d9a711320a53413e589ed4f4403cb.
Fresh baseline: GCC 14.2 Debug — 9/9 CTest targets passed.

Task 1: complete. Expected missing-feature test failed, then snapshot API passed; entire 10-target
suite green. The first incremental build reused a pre-header test object (__has_include does
not track a file that did not exist); forced test recompile verified the actual implementation.
Snapshot includes both success and failure invocations, all native maps, counter state, full
SelfState and research. Restore validates links, versions, dependencies and memoization.

Task 2: complete. Expected absent-archive RED; after implementation 11/11 tests pass.
Explicit SelfState codec includes all native fields. Binary encoding is canonical over unordered
maps/sets, preserves ordered vectors and NUL/IEEE bits, checks bounds and SHA-256, binds policy/
codec/version, rejects malformed data and requires exact native codec round-trip at capture.

Task 4: Clang strict compilation caught implicit char-to-unsigned-char conversion in hexadecimal digest formatting. Preserved compiler RED log; made byte interpretation explicit with static_cast<unsigned char>. No wire-format change.

Task 3: complete. Missing file/process feature test failed before implementation. POSIX create-only publication, bounded reads, separate-process restart and native continuation now pass. Writer/reader: 30376 states; 5 edges/invocations; 8755589 bytes.
Task 4: local review and matrix complete. Forged cache-key RED repaired by input-derived key validation. GCC Debug/Release 13/13; Clang ASan/UBSan 13/13; standalone absent-predecessor 4/4. Body mutation probe: 239 rejection / 17 structurally valid changes, not authentication. Remote source-hash verification and PR update are recorded at publication.
