# CURRENT — Decision Field v0.5: recoverable core replay

Date: 2026-09-27.
Predecessor Git checkpoint: `4282676da043cca93b1622532d94ae419d9795ab`.

## Executable continuation

```text
actual v0.3 16 x 12 baseline
-> frozen ComponentFrame + source references + uncollapsed metrics
-> signed component analysis
-> common-coordinate summary + exact per-row overrides
-> independent numeric-projection decoding
-> obligation replay
-> KEEP | REPAIR | UNRESOLVED
```

A discovered triadic surface is now capturable as a scoped `SurfaceObject` carrying its definition, poles, representative IDs, all explored waves, source references, validation results and declared criteria. It can be analyzed under a declared lateral/normal frame and provide a next-region bound.

## Measured local results

- Actual predecessor baseline: 192 projected observations.
- Stance projection: 6 shared coordinates, 10 variable coordinates.
- Exact reconstructed projections: 192/192; all six declared mandatory checks pass in this fixture.
- Numeric storage entries: 3,072 source coordinate values versus 1,926 core/override values. Metadata and original graph storage are excluded from this count.
- Surface capture: nearest 192 records; farther 30,144 records; all waves preserved, not only the 16 representatives.
- Synthetic positive/negative fixture: 96 negative checks preserved across 192 rows. KEEP means fidelity, not that every instance succeeds.

The six invariants are specified by the self-architecture fixture, not discovered as universal laws.

## Fresh verification

| Configuration | Result |
|---|---|
| GCC 14.2 Debug, recovered predecessor included | 9/9 CTest targets passed |
| GCC 14.2 Release, warnings as errors | 9/9 passed |
| Clang 17 Debug, AddressSanitizer + UndefinedBehaviorSanitizer | 9/9 passed |

The Git-only standalone slice has four test targets. The complete nine-target run requires the exact recovered v0.3 source dependency. See `scripts/recover_predecessor.py` and the verification report. GitHub Actions results, when available, are separate from these local results.

## Repairs in this continuation

1. Primary ranking no longer determines preservation scope: unselected and zero-scoring observations participate in common-core calculation.
2. Finite observed anchors replace overflow-prone averaging of equal large values.
3. Recovery rejects lost coordinates, malformed ranges and unsupported shared-coordinate claims.
4. Missing, unstable or unknown oracle results cannot silently become KEEP.
5. Surface capture rejects partial waves, duplicated representatives and mismatched qualification counts.

## Remaining boundaries

- Exact reconstruction here means the declared numeric projection; it does not claim to reconstruct every field of an arbitrary native Object.
- Runtime source graphs remain the authority for native values and lineage. Durable graph serialization is not implemented by this slice.
- Lateral/normal vectors are declared and checked under the supplied Euclidean numeric metric; this does not establish a differentiable manifold.
- Structural operations in the predecessor remain callback-based, not unified first-class transforms.
- The predecessor's whole-wave triadic expansion is bounded by resource guards, not a polynomial-time search guarantee.
- RMAL carrier promotion and dynamic cohort scheduling remain future tasks; root RMAL/RMALC C23 files are unchanged.
