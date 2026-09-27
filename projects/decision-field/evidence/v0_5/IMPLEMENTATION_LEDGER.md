# Execution ledger — continuation of Tasks 5–7

Remote base: redogit/DnD@4282676da043cca93b1622532d94ae419d9795ab.
Local workspace is an isolated worktree of the exact executable subproject snapshot, not a full repository clone.
Header, existing tests and CMake match the remote Git blob IDs.

- Baseline: merged component test target PASS (four internal assertions suites).
- Recovery: original v0.3 ZIP SHA-256 matches 91a25de44269e6f54d45c5744b38af09eef2daf25b8fedbdab79ca0b4a018292; unmodified archive builds and passes all three CTest targets.
- Ruling: preserve recovered archive unchanged; test the actual predecessor through an optional local include path. Do not fabricate missing Git source.
- Ruling: primary ranking is not a preservation permission. The complete input frame, including unselected and zero-quality observations, governs common-core stability.
- Ruling: range summaries cannot reconstruct individual observations. Store exact per-row overrides plus source IDs; the decoder does not receive the source frame.
- Ruling: KEEP means replay equivalence in the frozen projection/obligation/oracle scope, not that every problem instance succeeds. Negative instances must remain negative.
- Ruling: Tasks 5–7 share a frozen run/obligation/projection/metric/axis context. Stale or incomplete records fail closed. Source fingerprints and all raw evaluation metrics remain attached.
- Ruling: Task 8 stays deferred; a few successful local tests do not establish sufficiently stable semantics to promote the RMAL carrier as an execution primitive.

## Completed work

- Task 5: complete baseline adapter; actual v0.3 data tested, 192 records retained.
- Task 6: scoped SurfaceObject capture and component analysis; nearest 192 and farther 30,144 records retained; full-wave and count validation tested.
- Task 7: packet decoding and versioned obligation replay; exact projection, retained negative results and malformed evidence gates tested.
- Review repairs: complete-frame preservation, overflow-safe anchors, summary-integrity validation, unknown invalid verdicts and move/evaluation ordering.
- GCC Debug 9/9; GCC Release with -Werror 9/9; Clang ASan/UBSan with -Werror 9/9.
- One combined build command exhausted its tool time budget during Clang compilation. The interrupted build was resumed explicitly; the completed sanitizer run passed. The interruption remains in the build log.
- Root RMAL/RMALC C23 was not modified or rerun in this continuation.
