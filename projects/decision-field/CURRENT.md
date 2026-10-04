# CURRENT — v0.7 Mirror / RMAL native integration

Date: 2026-09-29. Continues merged PR #2 at
`141a60f58bdfe6d36774e0fba5e8350cb6ef5dcf` through PR #3.

## Implemented and locally tested

- Immutable FunctionObject; exact native source-occurrence/context-bound AnyFunctor.
- Mirror uses the existing invocation/cache/edge path, not a parallel engine.
- Computation and admission remain separate; rejected native candidates survive
  in versioned receipts without becoming admitted state nodes.
- Exact involution, replay and Homeward recovery.
- Actual RMAL C23 VM execution through an explicitly bound native capability.
- Separate-process checkpoint/replay with exact RMAL source binding.

Fresh local integration: Release 25/25; Debug ASan+UBSan 25/25. The 6,487-byte
process witness retains three states, three edges/invocations and one rejected
candidate. Its archive re-encodes byte-identically before and after replay.
See `evidence/v0_7/VERIFICATION.md`, `PROCESS_RECEIPT.json` and `SOURCE_MANIFEST.json`.
Root C23/ABI Actions checks are a separate scope from the full native local suite.

## Recovery and limits

Git-only native builds require the exact v0.3 archive and existing recovery script;
the companion bundle includes all original source. `DF_REQUIRE_FULL_NATIVE=ON`
rejects an incomplete setup rather than silently passing standalone tests.

Remaining: Meet4/Overlap unification; live cohorts; externally owned result-object
persistence; authenticated callbacks/source custody; native Windows durable I/O;
complete four-ID semantic cataloging; and generic request classes. The original
VM trace is process-local; source digest and native invocation trace persist.

## AnyFunctor research alignment

The later Work research specifies a singular partial evaluator and ten optional
structural evaluators. Their exact relationship to this bounded Mirror path,
proposed certificate checks and counterprobes are documented in
[`docs/ANYFUNCTOR_STRUCTURAL_RECONCILIATION_2026-09-29.md`](docs/ANYFUNCTOR_STRUCTURAL_RECONCILIATION_2026-09-29.md).
The structural planner and general evaluator are research targets, not v0.7
runtime features or results.

## Separate synthetic structural increment — 2026-09-30

The [standalone structural experiment](experiments/structural/README.md) implements
occurrence-preserving DuplicateEvaluator proposals, an independent synthetic
checker, append-only history, exact source/predecessor receipts and separate
admission. Its 24-schedule panel retains smaller occurrence-alias and
authority-removal failures. [Evidence and recovery chronology](evidence/structural_probe_2026_09_30/VERIFICATION.md)
remain separate from v0.7. The dated crosswalk above remains the historical
design input. No runtime binding, general evaluator family, Pareto planner,
behavioral equivalence, universality or P-versus-NP result is added.

The [receipt-ingress follow-up](evidence/structural_receipt_boundary_2026_09_30/VERIFICATION.md)
checks file read budgets and rejects malformed blob containers and excessive
JSON nesting explicitly. It preserves the original finite-panel receipt and
adds four counterprobe tests; history/admission semantics and K are unchanged.

The [bounded AdapterEvaluator follow-up](evidence/structural_adapter_2026_09_30/VERIFICATION.md)
adds a separate synthetic lift/lower fixture and a recoverable two-occurrence
structural macro. Independent checks require matching ownership, inverse type
contracts and empty boundary effects. Admission protects every K component and
strictly decreases adapter depth. Mixed DuplicateEvaluator orderings preserve exact
source recovery; smaller boundary-crossing failures remain in history. The old
fixture, historical receipts and frozen v0.7 source identities remain unchanged.
This extends only the standalone experiment, with no runtime or planner binding.
The [checkout follow-up](evidence/structural_adapter_2026_09_30/CHECKOUT_FIX.md)
retains the first Windows CI manifest failure and the scoped LF repair for the
14 frozen native source witnesses; their committed source bytes are unchanged.

## Way back

v0.6 evidence remains unchanged under `evidence/v0_6/`; v0.5 under `evidence/v0_5/`.
The earlier CURRENT/readme are recoverable at the exact PR #2 merge above.
The v0.6 large checkpoint digest remains
`8ab9f2b15524ac08d3d8cdf8959e7da344a301cca9a35a51530f86868309670a`.
The frozen v0.3 engine and DFNAT001 wire schema are unchanged.


## October 3 reconciliation — verified additive successors

The repository now carries the bounded synthetic structural checker recovered from
closed PR #7. It implements only finite DuplicateEvaluator and AdapterEvaluator
proposal/check/admission experiments with exact source reconstruction, immutable
history and retained negative receipts. It is not wired into the native Mirror engine
and does not claim behavioral equivalence, universality, optimality, termination or a
complexity result.

The exact Dynamic RMAL character-contract fixture required by the already-present
`tests/verify_dynamic_rmal.py` is restored from Git blob
`c6b86d1c81479b742134ee08ca9e83b087afcf9d`; this is a recovery repair, not a
compiled Dynamic RMAL runtime.

Cross-lineage decisions and withheld peer implementations are recorded in
`provenance/CODE_CROSS_CORRELATION_2026-10-03.md`.
