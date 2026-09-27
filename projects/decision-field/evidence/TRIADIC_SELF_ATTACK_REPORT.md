# Decision Field v0.3 — Triadic self-attack report

## Purpose

Extend the prior self/opposite test from a two-branch adversarial relation to a three-branch family:

- **Pole A** — the implemented architecture.
- **Pole B** — its lawful mirror: mandatory commitments preserved, optional commitments reflected.
- **Midpoint M** — the exact supported overlap of A and B, not an arithmetic average.

The attack structure is therefore:

1. A challenges B and B challenges A across their polar differences.
2. A and B independently challenge M for distinctions that M omits.
3. M challenges A and B by refusing pole-only commitments that lack support from the opposite side.

The search does not predeclare a semantic recursion depth. It explores complete directional waves and asks whether the observed states define a sector-spanning separating surface.

## Initial triad

The self-domain has 16 architectural commitments:

- 6 mandatory invariants;
- 10 optional/current implementation commitments.

Measured attack relation:

- A vs B disagreements: **10**
- A challenges midpoint omissions: **10**
- B challenges midpoint omissions: **10**
- midpoint rejects unsupported A-only commitments: **10**
- midpoint rejects unsupported B-only commitments: **10**
- midpoint retained commitments: **6 mandatory invariants**

The midpoint therefore acts as an epistemic adversary to both poles: it does not assert either pole's optional claims until additional structure supports them.

## Surface admission rule

A candidate surface is admitted only after an entire directional wave completes and all of the following hold:

1. every representative validates the mandatory obligations;
2. all 16 first-ring directions are represented;
3. left/right polar affinity is balanced within the configured tolerance;
4. representatives have non-zero validated departure from the midpoint;
5. the surface decision is made only after the full wave, never from an early partial direction.

The engine records 16 representative states, one for every first-ring direction, while retaining all observations and lineage.

## First complete surface

The nearest surface was found after the mandatory first wave:

- discovered wave: **1**
- state depth: **2**
- directional executions: **192**
- representatives: **16/16 directions**
- mean polar imbalance: **0.000000**
- radial departure mean: **0.306996**
- radial departure range: **0.285714–0.323810**

The important fact is not that it appeared at wave 1. The engine did not accept it until all 192 baseline directions were executed.

## Use the surface to define the next area

The first surface's own maximum radial departure, **0.323810**, was then used as the lower boundary for a second search. The next search therefore had to find a complete surface strictly outside the first one instead of using a guessed depth.

The result:

- wave 1: no admissible farther surface
- wave 2: no complete 16-direction farther surface
- wave 3: complete farther surface found

Second surface:

- discovered wave: **3**
- state depth: **4**
- cumulative directional executions: **30,144**
- representatives: **16/16 directions**
- mean polar imbalance: **0.000000**
- radial departure mean: **0.460319**
- radial departure range: **0.451915–0.467541**

This verifies the requested behavior: **the distance was not known before exploration**. Branches of branches were expanded wave-by-wave until a complete surface existed.

## Definitions exposed by the farther surface

All 10 optional architectural dimensions varied across the second surface:

- adaptive-convergence-cohorts
- adaptive-directional-scheduling
- baseline-16x12
- content-addressed-memoization
- cooperative-concessions
- distributed-execution
- dual-reversal-levels
- four-round-scoped-fixes
- mirror-first-reexpression
- preserve-deep-excursions

The six mandatory invariants remained fixed.

Thus the surface has a usable definition:

> Preserve the mandatory six-invariant core while allowing balanced variation across the ten optional architectural dimensions, with no polar side dominating the complete 16-direction boundary.

That definition can be carried forward as a boundary/axis specification for another area of exploration.

## Reprojection test

One representative from each of the four sectors was selected from the farther surface. The surface-varying dimensions became the next mirror-axis definition, and those four representatives were passed back through the quartet -> mirror -> overlap -> Meet4 machinery.

Result:

- the surface definitions were valid;
- the quartet reconstructed successfully;
- the new synchronized center was obligation-equivalent to the original six-invariant midpoint.

Therefore this prototype self-domain currently exhibits a **self-similar surface** rather than a newly discovered invariant dimension.

This is an evidence boundary, not a failure. The engine found a farther structured boundary, but the current toy domain does not contain additional latent semantics beyond its 16 predefined commitments. Discovering genuinely new dimensions requires a richer domain representation rather than inventing them from the current model.

## Implementation added

The generic engine now contains:

- `TriadProbeEvaluation`
- `TriadProbeRecord<T>`
- `SurfaceDefinition`
- `TriadExplorationResult<T>`
- `DecisionFieldEngine<T>::explore_triad_surface(...)`

`explore_triad_surface`:

1. builds the midpoint's complete 16 x 12 field;
2. evaluates every state against left pole, right pole, and midpoint;
3. waits for a complete wave before surface admission;
4. requires all 16 directional rays to participate;
5. if no surface exists, expands every current frontier state through its 12 outward branches;
6. repeats without a semantic fixed depth;
7. uses only an explicit execution-wave guard as a runtime safety boundary.

The execution guard is not treated as the mathematical or semantic distance of the search.

## Verification

CTest after this change:

```text
decision_field_tests                  PASS
decision_field_self_opposite          PASS
decision_field_triadic_self_attack    PASS

100% tests passed, 0 failed
```

The triadic run retained **30,381 states** while preserving branch lineage.
