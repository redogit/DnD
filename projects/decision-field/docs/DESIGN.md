# Directional Parametric Primary Component-of-Component Analysis

## Goal

Analyze explored Decision Field states from multiple signed directions, identify the strongest obligation-relevant components, recursively inspect structure inside those components, and join only validated shared structure into a simpler core.

## Why this exists

The 16 x 12 field generates many locally meaningful states.

A single global rank is insufficient because:

- opposite directions can both be informative;
- diagonals can expose interactions hidden on cardinal axes;
- lateral and orthogonal movement mean different things at a discovered surface;
- parameter directions may not align with geometric directions;
- a deep branch can contain a local structure that should not be averaged away.

The component layer therefore acts between exploration and simplification.

## Analysis frame

```text
ComponentFrame
    core[]
    observations[]
```

Each observation carries:

```text
state_id
values[]
obligation_progress
information_gain
evidence_strength
residual_cost
recovery_cost
decay_risk
```

These values are explicit inputs. Hidden judgment is not allowed to appear as an unexplained score.

## Basis

```text
horizontal  : RIGHT / LEFT
vertical    : UP / DOWN
diagonal    : UR / UL / DR / DL
lateral     : + / -
orthogonal  : + / -
parametric  : named axis + / -
```

Additional domain bases may be added later.

The current engine requires at least two dimensions for the canonical cardinal/diagonal basis.

## Projection

For observation `x`, core `c`, and normalized basis `b`:

```text
projection = dot(x - c, b)
```

Only positive projection contributes to that signed direction; the opposite direction has its own basis entry.

## Obligation-relative weighting

Current executable carrier:

```text
quality =
    obligation_progress
  + information_gain
  + evidence_strength
  - residual_cost
  - recovery_cost
  - decay_risk

score = positive_projection * max(0, quality)
```

Future work may replace the linear carrier, but any replacement must retain the individual evidence/cost dimensions and replayability.

## Primary grouping

Each observation is assigned to the admissible basis direction with highest current score.

Direction buckets accumulate:

- members;
- aggregate score;
- highest-scoring representative.

Buckets are sorted by aggregate score and bounded by `max_primary_components`.

## Component-of-component recursion

If a component has enough members and the configured recursive depth remains:

1. preserve the component and member identities;
2. exclude the parent's basis;
3. analyze the same members against the remaining basis;
4. create child components;
5. continue until the depth/member stop condition.

This prevents repeatedly rediscovering the same parent direction.

## Join to simpler core

Take the representative state of each retained primary component.

For each parameter dimension:

```text
if max(value) - min(value) <= tolerance:
    admit mean(value) as stable core coordinate
else:
    preserve {dimension, min, max} as residual
```

This is intentionally conservative.

## Relation to the 16 x 12 field

Planned direct adapter:

```text
Core C
-> 16 first-ring states
-> 12 outward probes each
-> 192 baseline observations
-> component analysis
-> convergence cohorts / counterprobe cohorts
-> recursive component analysis
-> simpler core candidate
-> replay against obligations
-> KEEP | REPAIR | UNRESOLVED
```

## Up / down relation

Adjacent RMAL/POLYMETA work distinguishes:

```text
DOWN -> realize obligations
UP   -> reconstruct claims from observations
```

Decision Field uses this structurally:

- down-analysis decomposes a core into executable directional/parameter components;
- up-analysis joins validated component results back into a reconstructible simpler core.

## Surface relation

When a triadic exploration discovers a SurfaceDefinition:

- lateral axes operate along the surface;
- orthogonal axes test departure from the surface;
- parametric axes test declared transform degrees;
- diagonals test combined/cross-direction movement;
- component recursion tests whether the surface has internal structure.

## Admission

A component/core result remains bounded by:

```text
GENERATE != VERIFY != ADMIT
FINITE FIELD != UNIVERSAL RESULT
SCORE != TRUTH
PRIMARY != NECESSARY
RESIDUAL != FAILURE
```
