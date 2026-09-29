# v0.5 review

Review performed by the implementing assistant; no independent human or subagent review is claimed. GCC/Clang are separate toolchain checks, not independent reviewers or independent scientific evidence.

## Defects reproduced and repaired

1. A zero-quality or ranking-cap-excluded observation could be absent from the common-core calculation. The two-row counterexample failed before repair. Preservation now uses the complete frame.
2. Summing equal maximum finite values overflowed while producing a supposed shared coordinate. The regression failed before repair. The shared value is now an observed anchor.
3. A recovery packet could reconstruct rows yet carry false range/stability metadata. A regression failed; reconstruction now verifies those claims against all recovered rows.
4. Invalid enum values from an oracle could be treated as matching answers. They now remain unresolved.
5. Surface basis construction read a vector's size in the same call that moved the vector. The integration test exposed argument-evaluation-order dependence. Size is captured before moving.

## Additional adversarial checks

Partial baseline, duplicated IDs, stale source state, incorrect quadrant address, wrong parent, unstable projection, missing residual values, corrupted values, altered scope, incomplete/unknown oracle output, duplicate representatives, qualification-count tampering and nonorthogonal declared axes.

## Scope not promoted

- Stance-only numeric projection tests do not prove native-state reconstruction.
- Six common mandatory coordinates were specified by the fixture.
- Row-entry savings exclude source graph, metadata and container overhead.
- Source callbacks and semantic oracles remain domain-supplied; replay checks are observed checks, not proofs of purity.
- The recovered v0.3 implementation is unmodified and retains its previously documented limitations.
