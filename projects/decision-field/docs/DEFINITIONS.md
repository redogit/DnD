# Decision Field definitions

## Decision Field

An obligation-relative executable field of states, transforms, branch relations, evidence, residuals, repairs and recoverable lineage.

## Core

The current shared state from which a bounded directional field is evaluated.

A Core is not assumed to be globally final.

## Direction

A typed relation from an anchor state toward a candidate region of state/parameter space.

Direction may be geometric, semantic, transform-relative or domain-defined.

## Directional basis

A set of explicit signed basis relations used to project observations.

Current first executable basis families:

- Horizontal: `RIGHT`, `LEFT`
- Vertical: `UP`, `DOWN`
- Diagonal: `UP_RIGHT`, `UP_LEFT`, `DOWN_RIGHT`, `DOWN_LEFT`
- Lateral: tangent-like `+` / `-`
- Orthogonal: normal-like `+` / `-`
- Parametric: declared parameter `+` / `-`

## Lateral

A direction that runs along the locally declared surface/tangent relation.

It is supplied by the domain; it is not inferred from a word such as "sideways".

## Orthogonal

A direction declared normal/orthogonal to the local surface relation.

The implementation normalizes the supplied vector but does not pretend to prove domain orthogonality.

## Parametric direction

A signed movement along one declared independent or conditionally independent parameter direction.

## Diagonal component

A combined movement across the first two declared cardinal axes.

A diagonal is a cross-direction probe, not evidence that the underlying variables are statistically independent.

## Primary component

The currently strongest obligation-relevant explanatory direction for one or more observations under the declared scoring inputs.

```text
PRIMARY != PRINCIPAL_VARIANCE_BY_DEFAULT
```

## Component-of-component analysis

A primary component with sufficient members is treated as a new local problem.

The parent basis is excluded and the component's members are analyzed again against the remaining basis.

This recursively asks what structure exists inside an already identified component.

## Simpler core

The shared stable parameter subset across retained representative primary components.

A coordinate enters the simpler core only when its representative values agree within tolerance.

## Residual dimension

A coordinate that cannot be collapsed into the simpler core.

It retains the observed minimum and maximum and remains available for later analysis.

```text
SIMPLIFICATION != DELETION
```

## SurfaceDefinition

A discovered bounded relation spanning required directions after a complete exploration wave.

A surface may become a first-class Object and seed the next exploration region.

## Mirror

A domain-declared reflected counterpart that preserves mandatory invariants and reflects declared relations.

```text
MIRROR != BOOLEAN_NOT
MIRROR != INVERSE_TRANSFORM
```

## Overlap

The greatest currently supported structure shared by paired states under the active obligations.

## Concession

A Pareto-safe re-expression or scoped adjustment that increases shared structure or reduces conflict without violating either side's mandatory obligations.

## Evidence boundary

Structural transfer from adjacent projects may suggest an operator or test.

It does not transfer the source project's evidentiary conclusion.

```text
METHOD_TRANSFER = ALLOWED
EVIDENCE_TRANSFER = DENY_UNLESS_EARNED
```
