# Synthetic structural probe verification — 2026-09-30

Base: `2f9fbe3b1964194d8f7b525c6fc90a22b3ff7df9` on `master`.
Scope: the standalone Python experiment under `experiments/structural/`, its
synthetic fixture, receipt and CI. No runtime or native Mirror source changes.

## Current checkout observations

| Check | Observed result | Retained evidence |
|---|---|---|
| Synthetic contracts, Python 3.12.14 | 24/24 | `TESTS.txt` |
| Same contracts with `python3 -O` | 24/24 | `OPTIMIZED_TESTS.txt` |
| Finite panel generation | 24 schedules; 96 proposals; 48 admissions; 48 rejections retained; 96 exact source reconstructions | `PROCESS_RECEIPT.json` |
| Separate-process receipt replay, without generators | PASS | `RECEIPT_REPLAY.txt` |
| v0.7 source identity check | All 14 unchanged; identity match is not a runtime test | `V0_7_SOURCE_IDENTITY.txt` |
| Root Python regression tests | 3/3 | `ROOT_PYTHON_TESTS.txt` |
| Independent read-only review of reconstructed implementation | No material issues; 24/24 contracts observed | Conversation review; no invented review transcript |

The canonical process receipt is 366,026 bytes, SHA-256
`9e8565d2fef18eb0f3098898c34b0242d1a3316e1c7684378bf607c30cbbdce7`.
It contains nine content-addressed byte blobs and 312 separate history events.
Initial K is `(24,20,3,2,2,2,1,8,1)`; final K is
`(22,20,3,2,0,2,1,8,1)`. Every schedule has the same final A digest in this
finite panel. General order independence is not established.

The two scoped DuplicateEvaluator proposals preserve all ordered occurrences,
authorities, edges and exact source reconstruction while sharing payload bodies.
Each deliberately smaller negative is well-formed: one removes an occurrence
and redirects its edges, and one removes an authority and relabels its node.
Both fail verification/admission, retain their candidate bytes and reasons in H,
and leave A unchanged. Replay validates these particular alterations independently
of action labels. The supplied source and predecessor bytes are recovery anchors,
not permission to admit a projection that cannot itself reconstruct the source.

## Commands

Run from the repository root. Use an unused output path; generation refuses
overwrite.

```sh
python3 -m unittest discover -s projects/decision-field/experiments/structural -p 'test_*.py' -v
python3 -O -m unittest discover -s projects/decision-field/experiments/structural -p 'test_*.py' -v
python3 projects/decision-field/experiments/structural/probe.py --output /tmp/structural-receipt.json
python3 projects/decision-field/experiments/structural/probe.py --verify /tmp/structural-receipt.json
python3 projects/decision-field/experiments/structural/probe.py --verify projects/decision-field/evidence/structural_probe_2026_09_30/PROCESS_RECEIPT.json
python3 projects/decision-field/scripts/check_mirror_manifest.py
python3 -m unittest discover -s tests -p 'test_*.py' -v
```

The Linux/Windows workflow repeats normal and optimized contracts, generates a
fresh receipt, replays fresh and checked-in receipts, and retains the exact
checked-out revision. CI observations remain separate from these local results.
`SOURCE_MANIFEST.json` binds this checkout's code and evidence file bytes; it
does not certify correctness or record a future commit's identity.

## Recovery chronology and native regression limit

Workspace maintenance removed the earlier unpushed checkout, patch, native
dependencies and local logs. Its reported commit
`6d08e1b169132d061bee1a3403da52cfba1e7c83` was not on the remote and could not
be recovered. This checkout was rebuilt from preserved conversation patches
against the same master. The synthetic contract suite and prose were
reconstructed; byte identity of the old complete implementation is not claimed.
The newly generated process receipt exactly matches the SHA-256 reported before
the loss. That is a bounded receipt identity observation, not recovery of the
old commit or its missing logs.

The previous conversation reported standalone native GCC 14/14, full Clang
Release 25/25, and ASan/UBSan 25/25 with leak detection disabled. Leak-enabled
execution was blocked by the environment's process inspection restriction.
It also reported native writer/reader equality with the existing v0.7 receipt
and the v0.6 large-checkpoint anchors. Those logs and generated native archives
were lost, so these are prior conversation-reported results, not freshly
reproduced evidence in this checkout. Native suites were not rerun during
reconstruction. The historical v0.7 evidence and all 14 source identities remain
unchanged; their original verification scope remains dated in `../v0_7/`.

`GENERATE != VERIFY != ADMIT`; `ACTIVE_GRAPH != FULL_HISTORY`;
`STRUCTURAL_EQ != BEHAVIORAL_EQ`. This finite synthetic increment establishes
no behavioral equivalence, universal evaluator, Pareto planner, runtime binding,
complexity bound or P-versus-NP result. In-process append-only history is not
authenticated, crash-durable storage.
