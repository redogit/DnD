# Decision Field

**Status:** active executable research/tooling project inside `redogit/DnD`.

This folder preserves and develops the Decision Field work without changing the authority of the root RMAL/RMALC C23 toolchain.

## Current object

The project lineage is a distributed, recoverable decision/search system that:

- preserves immutable lineage and obligation-relative distinctions;
- uses pure, replay-stable, reversible transforms;
- builds cooperative quartets, mirror opposites, overlaps and shared cores;
- expands a synchronized core into 16 first-ring directions and 12 outward probes per direction;
- preserves deep excursions and diagnoses repeated depth patterns instead of truncating them;
- allows scoped cohort fixes, concessions and synchronization;
- attacks its own design using lawful-opposite and triadic adversarial tests;
- now adds directional/parametric primary component-of-component analysis to reduce explored structure into a simpler core without deleting residual differences.

## Repository source boundary

The current **browsable executable source** added by this branch is the standalone directional component-analysis slice. The previously verified v0.3 Decision Field engine is retained here through exact source hashes, run reports, and strategic-input lineage rather than being silently reconstructed from partial text.

`BROWSABLE CURRENT SLICE != COMPLETE PREDECESSOR SOURCE TREE`

## New analysis layer

The project-specific component analysis is **not ordinary statistical PCA**.

It treats a direction as a typed basis relation and ranks components by obligation-relative explanatory value rather than variance alone.

Current basis families:

```text
UP / DOWN
LEFT / RIGHT
DIAGONAL combinations
LATERAL / tangent
ORTHOGONAL / normal
PARAMETRIC +/- declared degrees
```

Process:

```text
explored states
-> typed directional projections
-> primary components
-> component-local recursive decomposition
-> representative components
-> stable shared parameters
-> simpler core
+ preserved residual dimensions
```

A discarded dimension never silently disappears: it becomes a residual with its observed range.

## Relation to RMAL

RMAL/RMALC remain the repository's canonical language/toolchain authority.

Decision Field is a target-local project and potential RMAL execution consumer.

```text
METHOD_TRANSFER != EVIDENCE_TRANSFER
DECISION_FIELD_RESULT != RMAL_LANGUAGE_TRUTH
COMPONENT_ANALYSIS != STATISTICAL_PCA_UNLESS_AN_ADAPTER_EXPLICITLY_SAYS_SO
```

## Build

This subproject is C++23 and intentionally isolated from the root C23 build.

```bash
cmake -S projects/decision-field -B build/decision-field -DCMAKE_BUILD_TYPE=Release
cmake --build build/decision-field --parallel
ctest --test-dir build/decision-field --output-on-failure
# current GitHub slice: decision_field_component_analysis_tests
```

## Read order

1. `CURRENT.md`
2. `docs/DEFINITIONS.md`
3. `docs/DESIGN.md`
4. `docs/plans/2026-09-27-directional-component-analysis.md`
5. `evidence/ADJACENT_IDEAS.md`
6. `evidence/BUILD_REPORT.md`
7. source/tests

## Governing boundaries

```text
OBJECT IDENTITY IS INVARIANT; COORDINATES ARE NEGOTIABLE
GENERATE != VERIFY != ADMIT
SUCCESS != OPTIMAL
REPETITION != PROOF
RELATED != SUPPORTS
SHARED METHOD != SHARED EVIDENCE
FAILED PATH != USELESS PATH
SIMPLER CORE != ERASED RESIDUALS
```
