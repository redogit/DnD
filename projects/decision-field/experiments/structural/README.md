# Synthetic structural proposal/checker

This is the first code increment from the [2026-09-29 design crosswalk](../../docs/ANYFUNCTOR_STRUCTURAL_RECONCILIATION_2026-09-29.md). It runs independently of the C23 VM and native Mirror engine. Python 3.10+ and the standard library suffice.

From the repository root:

```sh
python3 -m unittest discover -s projects/decision-field/experiments/structural -p 'test_*.py' -v
python3 projects/decision-field/experiments/structural/probe.py --output /tmp/structural-receipt.json
python3 projects/decision-field/experiments/structural/probe.py --verify /tmp/structural-receipt.json
```

Choose an unused output path; the writer refuses overwrite. `--source` explicitly selects the trusted canonical synthetic source. The reader rechecks recorded proposals without rerunning generators. Fresh generation and receipt replay are separate observations.

The scoped `.gitattributes` rules retain LF bytes for this experiment and its evidence on Windows as well as Linux. CRLF conversion changes the frozen fixture and receipt identities and is correctly rejected by the canonical-byte checks.

## Object and authority

`source_graph.json` is an invented frozen fixture, not recovered Work observations or a mathematical theorem. Its 11 ordered source occurrences include a wrapper, two adapters, three identical integer payloads with distinct occurrence/evidence/object/source IDs, a same-payload occurrence across an authority boundary, a differently represented equal value, two apparent cycle nodes, and a wide state reached by a dependency tagged irrelevant. All source occurrences remain mandatory. Depth, cycle and relevance tags are synthetic declarations; no computation is executed to substantiate them.

`model.py` provides frozen `StructuralProposal`, `RecoveryReceipt`, `HistoryEvent`, `K`, `DuplicateEvaluator`, and `Session`. `Session.H` is an immutable tuple of immutable event bytes; each append preserves its prefix and links to the preceding event. `Session.A` is immutable projection bytes. Source and predecessor bytes remain addressable in H. Event/proposal IDs are run-local monotone IDs, qualified by the receipt's schedule. Source occurrence, semantic object, source, evidence and authority IDs remain unchanged across schedules. Initial body IDs derive from source occurrence IDs; surviving IDs cannot acquire different payload bytes.

Projection occurrences retain their complete metadata and a body binding. DuplicateEvaluator shares only byte-identical payloads within the same authority and kind, including all bindings of a selected body when scopes overlap. Sharing payload storage never shares event identity. This does not prove duplicate executions interchangeable.

History labels generation, checker and admission authorities separately. They are declared synthetic roles, not authenticated principals. H is append-only through this API in one process. Hostile mutation of Python internals, external edits and crash-durable authenticated custody are outside this increment.

## Verification and admission

`checker.py` never calls DuplicateEvaluator. Caller-owned frozen source and current A are its anchors. It checks exact ordered occurrence metadata, edges, authorities and surviving body identities; forbids sharing across authority/kind boundaries; expands A to the complete source byte for byte; checks exact source/predecessor recovery bytes and hashes; and independently observes K and binding mutation radius.

The certificate reference binds the contract/source/predecessor/candidate digests. It is a reference, not a proof or trusted success flag. A source copy in the receipt cannot rescue a lossy A. GENERATE appends a proposal; VERIFY appends a check; neither changes A. ADMIT requires an actual recorded successful check for that exact proposal and still-current predecessor. Stale, mismatched, replayed or rejected checks fail. Failed proposal bytes and reasons remain in H.

## Declared structural vector

All nine K components are protected against increase; admission requires at least one strict decrease.

| Component, in order | Synthetic definition |
|---|---|
| active_nodes | occurrences + stored payload bodies + authority records |
| active_edges | explicit relation edges + occurrence-to-body bindings |
| call_depth | maximum declared occurrence call-depth tag |
| adapter_depth | maximum declared occurrence adapter-depth tag |
| duplicate_payloads | excess stored identical bodies within an authority/kind owner class |
| recurring_cycles | number of explicit edges tagged `cycle` |
| dependency_volume | number of explicit edges tagged `dependency`, including irrelevant ones |
| projection_width | sum of stored payload coordinate-list lengths |
| wrapper_layers | sum of declared occurrence wrapper-layer tags |

The fixture moves from `(24,20,3,2,2,2,1,8,1)` to `(22,20,3,2,0,2,1,8,1)`. These are finite counts, not Big-O bounds. Cost hints and checker observations account for deterministic input/recovery byte volume; they do not measure timing, peak memory or lifecycle costs. Mutation radius counts changed occurrence records/bindings.

Canonical ASCII JSON graphs are bounded by 65,536 bytes, 64 occurrences/bodies, 128 edges, 16 authorities and nesting depth 16. Initial A must also fit. Malformed proposals up to 131,072 bytes may be retained but cannot pass. The panel report has a separate 2 MiB envelope.

The CLI now limits bytes returned by file reads to each source/report budget plus one overflow byte before parsing. Buffered I/O may fetch additional filesystem bytes; this is not a total-memory or timing bound. Malformed blob containers and receipt JSON that exceeds parser stack capacity return explicit rejection. The [ingress counterprobes](../../evidence/structural_receipt_boundary_2026_09_30/VERIFICATION.md) retain failures, controls and fresh 28-test results. The earlier 24-test evidence and process receipt remain unchanged at their original revision; fresh generation still produces that exact receipt.

## Finite panel and recovery

`probe.py` permutes two scoped instances of the same DuplicateEvaluator (overlapping pairs 1/2 and 2/3), an occurrence alias with redirected edges, and authority removal with relabeling. Both negative proposals are smaller in bytes and active-node count. Every attempt uses freshly observed A and has separate generation/check/admission events.

All 24 schedules retain two admissions and two rejections, with exact recovery at every step. The report retains 312 events and 96 proposals through content-addressed bytes, preserving every event occurrence and chronology. The reader expands references, recomputes check/admission events, verifies panel coverage and compares exact history. Each negative must reconstruct exactly its declared altered source and preserve surviving body identities. Report integrity is checked against caller-owned source; authorship is not authenticated. One final A digest occurred in this panel; general order independence is not inferred.

## Claim ceiling and chronology

`GENERATE != VERIFY != ADMIT`; `ACTIVE_GRAPH != FULL_HISTORY`; `STRUCTURAL_EQ != BEHAVIORAL_EQ`.

Only finite synthetic payload-sharing proposals/checks/admission are implemented. The general evaluator family, Pareto planner, runtime seam and recursive/universal evaluator remain unimplemented. No behavioral equivalence, universality, optimality, termination, polynomial bound or P-versus-NP result follows. The [evidence](../../evidence/structural_probe_2026_09_30/VERIFICATION.md) separates current reruns, prior conversation-reported results and the lost original commit.
