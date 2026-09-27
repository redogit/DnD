# CURRENT — Decision Field directional component analysis

**Date:** 2026-09-27
**Status:** executable local slice verified before repository import

## Preserved predecessor

The preserved predecessor lineage is the v0.3 Decision Field prototype developed through:

```text
quartet
-> lawful mirror
-> overlap
-> shared center
-> 16 x 12 baseline field
-> self-opposite test
-> triadic pole/pole/midpoint attack
-> unknown-distance surface discovery
```

The predecessor remains identifiable through exact source hashes, evidence reports, and the source manifest. The current GitHub branch intentionally exposes the new component-analysis code as a standalone buildable slice instead of pretending that a partial reconstruction is the full v0.3 source tree.

## New slice

Added `include/decision_field/component_analysis.hpp`.

Implemented:

- signed horizontal directions: left/right;
- signed vertical directions: up/down;
- four planar diagonal directions;
- explicit lateral +/- basis;
- explicit orthogonal +/- basis;
- arbitrary named parametric +/- axes;
- obligation/evidence/cost weighted directional projection;
- primary component grouping;
- recursive component-of-component decomposition;
- simpler-core joining across all retained members of retained primary components;
- residual preservation for every non-stable dimension.

## Meaning of primary

`PRIMARY` means highest current obligation-relevant explanatory component under the declared scoring inputs.

It does **not** mean highest covariance variance by definition.

## Current scoring carrier

For an observation and a positive signed basis direction:

```text
quality =
    obligation_progress
  + information_gain
  + evidence_strength
  - residual_cost
  - recovery_cost
  - decay_risk

component_score =
    positive_projection * max(0, quality)
```

This is a first executable carrier, not the final universal scoring law.

## Core join

All retained members of retained primary components are compared coordinate-by-coordinate; representative states are not sufficient for a stability claim.

If the observed range is within the declared tolerance, the coordinate is admitted into the simpler core.

Otherwise it is preserved as a residual dimension with minimum/maximum extent.

## Verification

Fresh local verification before import:

```text
decision_field_tests                       PASS
decision_field_self_opposite               PASS
decision_field_triadic_self_attack         PASS
decision_field_component_analysis_tests    PASS

4 / 4 PASS
```

See `evidence/BUILD_REPORT.md`.

## Next development boundary

Connect the component analyzer directly to:

1. the 192 baseline probe records;
2. discovered `SurfaceDefinition` objects;
3. synchronization-driven cohort reconfiguration;
4. first-class SurfaceDefinition input/output;
5. RMAL carriers once the semantics are independently stable.

```text
CURRENT EXECUTABLE SLICE != FINAL COMPONENT THEORY
```
