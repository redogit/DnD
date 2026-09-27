# Directional Component Analysis Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Build an obligation-relative up/down/left/right/diagonal/lateral/orthogonal/parametric component-of-component analyzer that reduces explored Decision Field states to a simpler recoverable core without erasing residual distinctions.

**Architecture:** Keep the existing Decision Field exploration semantics separate from the new component layer. Convert explored states into explicit `ComponentObservation` records, project them onto typed signed bases, recursively decompose strong components, then join only representative coordinates that remain stable within tolerance. Preserve every non-collapsible coordinate as a residual and replay the simpler-core candidate against the original obligations before admission.

**Tech Stack:** ISO C++23, CMake >= 3.25, CTest. RMAL/RMALC remain a separate root C23 toolchain and are not modified by this subproject.

**Spec:** `projects/decision-field/docs/DESIGN.md`

## Global Constraints

- Object identity remains recoverable while coordinates/representations may change.
- `GENERATE != VERIFY != ADMIT`.
- `SUCCESS != OPTIMAL`.
- `REPETITION != PROOF`.
- `RELATED != SUPPORTS`.
- `METHOD_TRANSFER != EVIDENCE_TRANSFER`.
- Directional components are obligation-relative; `PRIMARY != PRINCIPAL_VARIANCE_BY_DEFAULT`.
- Simplification must retain every non-collapsible dimension as a residual.
- Existing v0.3 self-opposite and triadic evidence is predecessor evidence and may not be retroactively rewritten.
- Root RMAL/RMALC C23 build authority remains unchanged.

## Review Focus

1. Zero or dimension-mismatched basis vectors must fail explicitly rather than silently normalize.
2. Opposite signed directions must remain distinct even when they share an underlying parameter.
3. Recursive component analysis must not rediscover the excluded parent basis as its own child.
4. A varying representative dimension must remain residual rather than being averaged into the simpler core.
5. High geometric projection with negative obligation/evidence quality must not outrank obligation-relevant components.

---

### Task 1: Directional basis and observation model

**Files:**
- Create: `projects/decision-field/include/decision_field/component_analysis.hpp`
- Test: `projects/decision-field/tests/component_analysis_tests.cpp`

**Interfaces:**
- Consumes: explicit numeric observation coordinates and explicit obligation/evidence/cost values.
- Produces: `BasisDirection`, `ComponentObservation`, `ComponentFrame`, `AnalysisConfig`.

- [x] **Step 1: Write the failing basis coverage test**
- [x] **Step 2: Run it and verify RED because `component_analysis.hpp` does not exist**
- [x] **Step 3: Implement signed horizontal, vertical, diagonal, lateral, orthogonal and parametric basis construction**
- [x] **Step 4: Run the test and verify GREEN**
- [x] **Step 5: Preserve the TDD witness in `evidence/BUILD_REPORT.md`**

### Task 2: Obligation-relative component selection

**Files:**
- Modify: `projects/decision-field/include/decision_field/component_analysis.hpp`
- Test: `projects/decision-field/tests/component_analysis_tests.cpp`

**Interfaces:**
- Consumes: `ComponentFrame`, `BasisDirection[]`, `AnalysisConfig`.
- Produces: ranked `ComponentNode[]`.

- [x] **Step 1: Test that observations are assigned to their strongest admissible signed basis**
- [x] **Step 2: Implement projection and explicit quality weighting**
- [x] **Step 3: Preserve aggregate score and representative observation**
- [x] **Step 4: Verify test and full local suite**

### Task 3: Component-of-component recursion

**Files:**
- Modify: `projects/decision-field/include/decision_field/component_analysis.hpp`
- Test: `projects/decision-field/tests/component_analysis_tests.cpp`

**Interfaces:**
- Consumes: one retained component and its member observation indexes.
- Produces: child `ComponentNode[]` under remaining bases.

- [x] **Step 1: Write a test with multiple observations sharing a parent component but separable by another axis**
- [x] **Step 2: Exclude the current parent basis during child decomposition**
- [x] **Step 3: Stop at configured depth/member bounds**
- [x] **Step 4: Verify children preserve their member indexes and depth**

### Task 4: Join representative components to a simpler core

**Files:**
- Modify: `projects/decision-field/include/decision_field/component_analysis.hpp`
- Test: `projects/decision-field/tests/component_analysis_tests.cpp`

**Interfaces:**
- Consumes: representative observations from retained primary components.
- Produces: `SimplerCore{stable_parameters,residual_dimensions}`.

- [x] **Step 1: Test one stable coordinate plus two varying coordinates**
- [x] **Step 2: Admit a coordinate only when representative range <= `core_tolerance`**
- [x] **Step 3: Preserve every non-stable coordinate as `CoreResidualDimension{dimension,minimum,maximum}`**
- [x] **Step 4: Verify no varying coordinate disappears**

### Task 5: Adapter from 16 x 12 Decision Field observations

**Files:**
- Create: `projects/decision-field/include/decision_field/component_adapter.hpp`
- Test: `projects/decision-field/tests/component_adapter_tests.cpp`

**Interfaces:**
- Consumes: core state, 16 first-ring states, 192 baseline probe records, domain parameter projection.
- Produces: `ComponentFrame` with one explicit observation per selected field state.

- [ ] **Step 1: Write failing adapter tests for all 192 baseline records and provenance retention**
- [ ] **Step 2: Implement explicit state-to-coordinate projection callback**
- [ ] **Step 3: Map probe evaluation dimensions without collapsing their individual values**
- [ ] **Step 4: Verify adapter round-trip references source state IDs**

### Task 6: First-class SurfaceDefinition analysis

**Files:**
- Create: `projects/decision-field/include/decision_field/surface_component_analysis.hpp`
- Test: `projects/decision-field/tests/surface_component_analysis_tests.cpp`

**Interfaces:**
- Consumes: discovered `SurfaceDefinition`, representative states, local tangent/lateral and orthogonal/normal definitions.
- Produces: surface-local component analysis and next-region parameterization.

- [ ] **Step 1: Test lateral vs orthogonal separation on a synthetic surface**
- [ ] **Step 2: Treat the surface itself as an input Object rather than only a parameter bundle**
- [ ] **Step 3: Project diagonal/parametric components relative to surface-local basis**
- [ ] **Step 4: Preserve surface definitions and residuals as predecessor lineage**

### Task 7: Replay simpler core against obligations

**Files:**
- Create: `projects/decision-field/include/decision_field/core_replay.hpp`
- Test: `projects/decision-field/tests/core_replay_tests.cpp`

**Interfaces:**
- Consumes: original obligation set, simpler-core candidate, residual dimensions, reconstruction callback.
- Produces: `KEEP | REPAIR | UNRESOLVED` disposition plus validation receipt.

- [ ] **Step 1: Test a candidate core that loses an obligation-relevant residual and must fail**
- [ ] **Step 2: Reconstruct candidate states from core + retained residuals**
- [ ] **Step 3: Replay positive and negative evidence**
- [ ] **Step 4: Admit only obligation-equivalent simplification**

### Task 8: RMAL carrier after semantic stabilization

**Files:**
- Create: `projects/decision-field/rmal/directional_component_analysis.rmal`
- Create: `projects/decision-field/evidence/RMAL_COMPONENT_CARRIER.json`

**Interfaces:**
- Consumes: independently stable Decision Field component semantics.
- Produces: RMAL carrier and RMALC check/compile/audit receipts.

- [ ] **Step 1: Encode the stable interface without changing root RMAL grammar**
- [ ] **Step 2: Run current root RMALC against the carrier**
- [ ] **Step 3: Record parser/compiler/audit results separately from semantic validation**
- [ ] **Step 4: Preserve `COMPILED != SCIENTIFICALLY_VALID`**

## Current verification checkpoint

Local full predecessor + component suite:

```text
decision_field_tests                       PASS
decision_field_self_opposite               PASS
decision_field_triadic_self_attack         PASS
decision_field_component_analysis_tests    PASS
```

The repository subfolder intentionally exposes the new component layer as an isolated build while preserving predecessor hashes/reports separately.
