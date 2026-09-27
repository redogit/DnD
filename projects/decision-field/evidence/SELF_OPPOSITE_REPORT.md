# Decision Field self-opposite run

## Purpose

Run the prototype's current architectural commitments against a semantic mirror of those same commitments. This is a self-model test, not a literal source-code negation: the commitments implemented by the engine are represented as typed state, mirrored through the engine's own quartet/mirror/overlap machinery, and then expanded through the 16 x 12 directional baseline.

## Self model

Six commitments are treated as mandatory architectural invariants:

1. preserve lineage
2. preserve mandatory obligations
3. preserve explicit branch identity
4. require reversible transforms
5. preserve evidence/provenance
6. retain negative evidence

Ten implemented design choices are treated as non-mandatory/mirrorable for this experiment:

- distributed execution
- adaptive convergence cohorts
- four-round scoped fixes
- 16 x 12 baseline exploration
- mirror-first re-expression
- cooperative concessions
- preservation of deep excursions
- exact + obligation reversal levels
- content-addressed memoization
- adaptive directional scheduling

## Scenario A — lawful mirror

The mirror preserves the six mandatory invariants and reverses the ten mirrorable design choices.

Result:

- Direct self/opposite overlap: **6 / 16** commitments.
- Mirror re-expression required: **no**.
- Synchronized center: exactly the **six mandatory invariants**.
- Opposed non-mandatory choices retained explicitly: **10**.
- Mandatory directional baseline: **192 executions**.
- Default-threshold convergence cohorts after baseline: **24**.
- Retained states: **222**.

The engine therefore does not average itself with its opposite. It extracts the shared obligated core and keeps the remaining disagreement outside the core.

## Scenario B — absolute mirror

The mirror initially reverses all sixteen commitments, including the mandatory ones.

Initial result:

- Direct self/opposite overlap: **0 / 16** commitments.

The engine then follows the designed mirror-first repair path. It holds the primary side fixed and re-expresses the mirror side only enough to restore the mandatory invariants.

Final result:

- Mirror re-expression required: **yes**.
- Recovered synchronized center: exactly the **six mandatory invariants**.
- Non-mandatory opposition retained: **10**.
- Mandatory directional baseline after repair: **192 executions**.
- Default-threshold convergence cohorts after baseline: **24**.
- Retained states: **226**.

The absolute opposite therefore does not force the primary to move. The mirror is repaired by the smallest obligation-required distinction, while its remaining opposition survives.

## Defect discovered by the self-run

The first self-run grouped all 192 baseline probes into one convergence cohort. The cause was single-link/transitive connected-component clustering:

`A ~ B` and `B ~ C` could place `A` and `C` in the same cohort even when `A` and `C` did not independently meet the affinity threshold.

That contradicted the intended rule: separately converging areas should begin separately and only merge after synchronization evidence supports it.

The cohort implementation was replaced with a stricter rule:

- probes are seeded in descending convergence-potential order;
- a probe may join a cohort only when its affinity meets the threshold against **every current member**;
- later synchronization may still reconfigure or merge cohorts.

After the repair, the self-test produces 24 cohorts at the default threshold rather than one global cohort.

### Threshold sweep after repair

| Affinity threshold | Cohorts | Largest cohort |
|---:|---:|---:|
| 0.70 | 23 | 13 |
| 0.75 | 32 | 15 |
| 0.80 | 43 | 10 |
| 0.85 | 115 | 4 |
| 0.90 | 130 | 3 |
| 0.95 | 171 | 2 |

The non-monotonic largest-cohort sizes at low thresholds are a consequence of deterministic greedy complete-affinity assignment; cohort count remains monotonic as the threshold tightens. This is acceptable for the prototype but should eventually become an explicitly synchronization-driven cohort optimizer rather than a one-pass greedy partition.

## Self-consistency enforcement added

The self-run also exposed that structural domain-policy operations were not receiving the same replay-stability checks as registered transforms. The engine now replay-checks:

- `Meet4`
- mirror generation
- overlap
- mirror re-expression
- concession
- 16-direction generation
- 12-way outward generation
- probe evaluation

It also verifies observed mirror involution before re-expression:

`Mirror(Mirror(x)) == x`

for the observed state and axis definition.

## Remaining implementation seam

The structural domain-policy operations are now replay-checked and their state transitions retain exact predecessor links, but they are not yet represented through the same content-addressed `TransformDefinition` / transform-edge machinery as ordinary registered transforms. Therefore they do not yet receive the full invocation receipt, memoization, and inverse-descriptor treatment of generic transforms.

That is the next important unification boundary exposed by the self-test: **structural operations should become first-class transforms rather than privileged callbacks.**
