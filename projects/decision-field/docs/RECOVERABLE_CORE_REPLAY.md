# Recoverable core replay — Tasks 5–7 continuation

## Frozen scope

Every frame binds run ID, obligation-set ID/version, projection ID, metric-policy ID and axis version. A changed scope produces UNRESOLVED rather than transferring a previous result into a new domain.

`make_baseline_frame(engine, field, context, projection, assessment)` accepts the actual v0.3 engine/field through structural templates. It validates 16 first-ring nodes, 12 addressed probes for each, unique source identities, direct declared parent links, non-stale sources and replay-stable finite projections/assessments. It snapshots projected values and preserves all ten raw probe metrics separately from the six component-scoring inputs.

The caller must supply evidence strength and residual-cost interpretation explicitly. Those quantities are not present as authoritative measurements in the predecessor's ProbeEvaluation and are not invented by the adapter.

## Selection versus preservation

Primary components guide attention. They do not authorize forgetting an observation. Common-coordinate analysis now uses every observation in the input frame, including zero-quality and component-cap-excluded rows.

A shared value is an observed anchor whose complete source range lies within tolerance. It is not a newly invented average. This avoids overflow when multiple equal, large finite values are joined.

## A summary is not a recovery payload

A minimum/maximum range cannot reconstruct each original row. The packet therefore contains:

```text
frozen context
+ shared-coordinate anchors
+ variable-coordinate ranges
+ ordered source state IDs
+ exact per-row coordinate overrides
```

A coordinate that differs from its anchor even by one bit is stored as an override, including signed zero. No arithmetic delta is used, avoiding subtraction/addition rounding as a recovery dependency.

`reconstruct(packet)` has no source-frame argument. It rejects missing coordinates, duplicate overrides, inconsistent identities/counts, non-finite payloads, tampered ranges and unsupported common-coordinate claims. The recovered numeric projection is exact by default; native Object fields outside that projection remain referenced through the source graph.

## Replay and negative evidence

`replay_core(original, packet, oracle, mode)` checks decoded rows against the preserved originals under a named, versioned oracle with an explicit obligation list. Each oracle evaluation is repeated to detect observed instability.

- KEEP: required exact projection or declared obligation outcomes agree for all tested rows.
- REPAIR: coverage, identity, recovery payload or required behavior changed.
- UNRESOLVED: context changed, evidence is absent, the oracle is missing/incomplete/unstable, or a required result is unknown.

A retained counterexample must remain a counterexample. Therefore a packet can receive KEEP while `all_obligations_pass` is false. That is successful preservation of a negative instance, not a false claim that the instance was solved.

Exact numeric-projection replay is the default. Obligation-only replay is explicitly selectable and limited to the provided oracle; it is not a proof of semantic equivalence beyond that oracle.

## Surface as an input Object

`capture_surface` creates an owned `SurfaceObject` from a completed v0.3 triadic search. It carries both poles, midpoint reference, all explored records, the 16 representatives, all definitions, declared criteria, qualification counts, observed statistics and per-record verification outcomes.

The adapter verifies complete whole-wave coverage as defined by the v0.3 engine: 192 observations initially, then twelve successors per previous frontier state. This is a version-specific adapter contract, not a requirement that all future adaptive schedulers expand every state.

Qualification counts and representative statistics are recomputed from retained observations and declared criteria. Partial waves, duplicate representatives and inconsistent counts are rejected. No representative can stand in for an omitted deep branch.

`analyze_surface` checks declared lateral/normal axes under the current Euclidean numeric metric and feeds the complete observation frame into component analysis. `next_region` returns a strictly greater radial lower bound with a predecessor-surface reference. Neither function proves a global manifold or universal search coverage.

## Current interfaces

- `component_adapter.hpp`: scope, source snapshots and complete baseline projection.
- `core_replay.hpp`: recoverable packets, decoder, oracle and finite replay receipts.
- `surface_component_analysis.hpp`: scoped surface capture, declared local basis and next-region relation.

The older component-analysis header remains standalone. Its common-core calculation is repaired to use the complete input frame; component ranking and recursive decomposition remain bounded heuristics.

## Remainder

Native-state serialization, durable content-addressed storage, cooperative live-worker scheduling, first-class structural transform unification and RMAL execution integration remain separate obligations. This slice does not claim to implement them through naming or documentation alone.
