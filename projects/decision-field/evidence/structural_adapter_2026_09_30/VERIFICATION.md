# Bounded AdapterEvaluator — verification, 2026-09-30

Continues PR #7 at `4fc3883a1c3389063abb72926d63e85166c99542`, base master
`2f9fbe3b1964194d8f7b525c6fc90a22b3ff7df9`. Date uses America/New_York;
execution crossed into October 1 UTC. Python 3.12.14 on Linux.

## Object, obligation and deliberate boundary

The original `source_graph.json` has opaque adapter payloads, not certified
lift/lower contracts. Its bytes and earlier evidence remain unchanged. The new
`adapter_source_graph.json` changes only graph ID and adds two explicit synthetic
adapter declarations. All 11 ordered occurrence identities, both authority IDs,
source/object/evidence IDs, chronology, original payload fields and edges remain.
These declarations are test inputs; they are not recovered user observations or
proof that executable functions are mutual inverses.

The bounded evaluator packs exactly two adjacent depth-(1,2) lift/lower occurrence
records into one top-level structural macro. Both complete records remain inside
it, with original payload bodies and edges outside it. Source recovery expands
that representation directly, without substituting the receipt's source copy.
No runtime operation is removed or executed. No arbitrary-length adapter-chain
planner, general evaluator family, Pareto planner or runtime binding is added.

The independent checker establishes matching ownership/authority, inverse declared
type endpoints and inverse-pair IDs, no declared boundary effects and the exact
three-edge linear incident topology. It checks compact pairs regardless of the
generator label. The ordered identity records, authority list, evidence, source
edges and retained body identities remain protected. All nine K components must
be nonincreasing; a pair-structure change or AdapterEvaluator-labelled proposal
also requires strict global adapter-depth decrease.

For this fixture K goes from `(24,20,3,2,2,2,1,8,1)` to
`(23,20,3,1,2,2,1,8,1)` for the adapter alone and
`(21,20,3,1,0,2,1,8,1)` after both duplicate scopes. Active-node counting treats the
macro as one top-level entry; the two retained identity records have not vanished.
Adapter depth is a declared representation measure, not measured execution depth.
The macro adds JSON bytes; source/candidate/recovery byte costs remain recorded
separately. No timing, peak-memory, lifecycle or asymptotic improvement follows.

## Fresh observations

| Check | Result | Record |
|---|---|---|
| Baseline before edits | 28/28 | `BASELINE_TESTS.txt` |
| New evaluator tests before implementation | 7 failures, 1 error, 1 existing rejection passed | `RED_TESTS.txt` |
| New receipt tests before panel support | 2 errors, 9 passed | `RED_RECEIPT_TESTS.txt` |
| Full synthetic suite | 39/39 | `TESTS.txt` |
| Same suite under `python -O` | 39/39 | `OPTIMIZED_TESTS.txt` |
| Root Python regressions | 3/3 | `ROOT_PYTHON_TESTS.txt` |
| Frozen v0.7 source identities | 14/14 | `V0_7_SOURCE_IDENTITY.txt` |
| Existing and new receipts, fresh and saved, separate processes | Exact replay; generators not called by reader | `RECEIPT_REPLAY.txt` |

The initial RED error came from the unsupported v2 candidate having no observed
K, while its test required depth 1. It is retained rather than rewritten. The two
receipt RED errors identify the then-absent `adapter` panel argument. Tests use
the real Session/checker; the generator-disabling tests replace only generator
methods with exceptions to establish the reader/checker's independence.

The 24 new schedules permute both overlapping DuplicateEvaluator scopes,
AdapterEvaluator and a deliberately smaller compact-pair authority-removal
forgery. Every schedule has 3 admissions and 1 retained rejection: **72 admissions,
24 rejections, 96 source reconstructions, 312 immutable history events**. Generation
and verification leave A unchanged; admission is bound to a recorded successful
check and the still-current predecessor. One final A digest occurs in this finite
panel; this does not establish general order independence.

New process receipt: **401,889 bytes**, SHA-256
`02f6782b977b3783e1068566fe2d987472692fd361f67a425347749220fe406d`.
Fresh generation is byte-identical to the saved new receipt. Historical duplicate
receipt remains **366,026 bytes**, SHA-256
`9e8565d2fef18eb0f3098898c34b0242d1a3316e1c7684378bf607c30cbbdce7`;
fresh generation remains byte-identical to it as well.

## Retained counterprobes

`COUNTERPROBES.json` retains the full H of 12 separate synthetic source variants:
authority, ownership, outer type, inner type, boundary effect, inverse ID,
direction, unknown contract field, boundary edge, fan-out, fan-in and depth.
Each forced proposal also shares one duplicate body, making it smaller in bytes
and active-node count. Each reconstructs **its own frozen variant source exactly**,
but fails `adapter_contract`, is denied admission and remains in H. This separates
recovery correctness from admissibility. Variant sources are explicitly derived
counterexamples, not silently substituted for the accepted panel fixture.

`counterprobes.py` reproduces those records from the new fixture. This is fresh
counterprobe generation; independent replay is separately evidenced by the process
receipt reader. The suite also rejects pair reordering, missing/repeated/nested
occurrences, stale/unverified/repeated admissions, false evaluator labels, unchanged
global adapter depth despite duplicate savings, and protected K increases despite
an adapter reduction.

## Reproduction

From repository root, use new output paths (writers refuse overwrite):

```sh
python -m unittest discover -s projects/decision-field/experiments/structural -p 'test_*.py' -v
python -O -m unittest discover -s projects/decision-field/experiments/structural -p 'test_*.py' -v
python -m unittest discover -s tests -p 'test_*.py' -v
python projects/decision-field/scripts/check_mirror_manifest.py
python projects/decision-field/experiments/structural/probe.py --output /tmp/duplicate-fresh.json
python projects/decision-field/experiments/structural/probe.py --verify /tmp/duplicate-fresh.json
python projects/decision-field/experiments/structural/probe.py --verify projects/decision-field/evidence/structural_probe_2026_09_30/PROCESS_RECEIPT.json
python projects/decision-field/experiments/structural/probe.py --adapter --output /tmp/adapter-fresh.json
python projects/decision-field/experiments/structural/probe.py --adapter --verify /tmp/adapter-fresh.json
python projects/decision-field/experiments/structural/probe.py --adapter --verify projects/decision-field/evidence/structural_adapter_2026_09_30/PROCESS_RECEIPT.json
python projects/decision-field/evidence/structural_adapter_2026_09_30/counterprobes.py --output /tmp/adapter-counterprobes.json
```

The workflow repeats synthetic normal/optimized suites, fresh/saved receipt replay
for both panels, and frozen source identity checks on Linux and Windows. Local
observations above do not claim those remote runs have already passed. The full
native Mirror build/runtime suite was not rerun: identity matches establish
preservation of anchors, not new runtime evidence. The historical crosswalk and
native evidence stay unchanged. See `REVIEW.md` for the independent source review.

`GENERATE != VERIFY != ADMIT`; `STRUCTURAL_EQ != BEHAVIORAL_EQ`.
No behavioral equivalence, universal termination, universality, optimality,
polynomial bound or P-versus-NP result is claimed.
