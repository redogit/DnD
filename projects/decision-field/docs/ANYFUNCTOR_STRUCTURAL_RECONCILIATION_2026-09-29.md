# AnyFunctor structural reconciliation — 2026-09-29

This note connects the verified v0.7 Mirror slice to the later AnyFunctor research. It is a **design/status crosswalk**, not a claim that the structural planner or a universal evaluator runs in RMAL or Decision Field today.

## Authority and current witness

- Repository baseline: `redogit/DnD` master `6d24f4fd1a4464a8e8aee27731a7fecce0831351`, merged PR #3. The current executable state is described in [CURRENT](../CURRENT.md) and [Mirror / RMAL Homeward](MIRROR_RMAL_HOMEWARD.md).
- Executable carrier: [`anyfunctor_mirror.hpp`](../include/decision_field/anyfunctor_mirror.hpp), [unit tests](../tests/anyfunctor_mirror_tests.cpp), and [RMAL fixture](../examples/mirror_homeward.rmal). The C23 VM calls an explicitly bound native C++ host capability; the compiler itself remains C23.
- Bounded evidence: [v0.7 verification](../evidence/v0_7/VERIFICATION.md) reports 25/25 local Release and 25/25 local ASan/UBSan integration tests, plus a distinct root C23/ABI CI scope. It preserves one rejected candidate in a 6,487-byte independent-process checkpoint/replay witness. These are exact fixture results, not a general complexity bound.
- Research inputs held in Work: `ANYFUNCTOR_SINGULAR_FUNCTION_RESEARCH_SYNTHESIS_2026-09-29.md` and `ANYFUNCTOR_WORLD_CONNECTIONS_2026-09-29.md`. Their mathematical anchors and transfers inform this design; they do not supply implementation receipts. The older recovered generic signature `AnyFunctor<O, Ia,Ib,Ic,Id,Ta,Tb,Tc,Td>(Inputs<…>)` is a partial historical input, not silently replaced by the C++ Mirror template.

## What is here and what is proposed

| Relation | Current executable v0.7 | Research target / remaining obligation |
|---|---|---|
| Function representation | Immutable `FunctionObject<T>::mirror` binds one native Mirror policy, declared axes, obligations, codec and policy identity. | General versioned `FunctionObject` with representation, input/output contracts, effects, authority, evidence/recovery policies and claim ceiling. |
| Invocation | `AnyFunctor(engine, AnyInvocation<T>)` accepts exactly one source state in this path and binds exact source occurrence, encoded value, context and execution space. | `AnyFunctor : AnyInvocation ⇀ AnyOutcome` over represented functions, with ordered input occurrences, explicit child frames and resource envelope. Partiality remains essential. |
| Self application | No represented AnyFunctor FunctionObject or explicit recursive child-frame evaluator is present. | A request for itself returns its representation; a distinct invocation frame performs any actual recursive evaluation. No universal termination or semantic deciding claim. |
| Verification and admission | Candidate generation, involution/obligation checks, admission and Homeward are distinct. Rejected candidate bytes remain in a receipt without an admitted state. | General computation, truth and usage statuses; independent evidence/certificate checks and authority for each admission. Current checks cover only the declared Mirror contract. |
| Persistence | DFNAT001 retains native engine data, invocation and recovery receipts; the host rebinds compiled capability and exact RMAL source at restart. | Full provenance/history graph, child-frame trace, general continuation, externally owned result objects and general recovery contracts. VM trace remains process-local today. |
| Structural reduction | No ten-evaluator family, `StructuralProposal`, Pareto planner, or structural-equivalence certificate protocol is implemented. | Optional certified reductions of the active graph with recoverable full history. They must never become semantic requirements for executing a function. |

The name AnyFunctor currently labels a bounded Mirror adapter. The research uses it for one singular partial evaluator. These share an intended lineage, but identical spelling does not make the current adapter universal, a category-theoretic functor, or a proof of arbitrary program equivalence.

## Proposed structural contract

Let `H_t` be append-only provenance/history and `A_t` the active projection used for a bounded invocation. The proposed invariant is:

```text
H_(t+1) ⊇ H_t
A_(t+1) may contain less active structure than A_t
ACTIVE_GRAPH != FULL_HISTORY
SIMPLIFIED != ERASED
```

A `StructuralProposal` records evaluator ID, predecessor IDs, candidate structure, structural-equivalence certificate reference, predicted complexity change, verification and recovery costs, mutation radius, and exact recovery receipt. Its checker must independently establish preservation of every obligation-relevant occurrence, identity, authority and evidence distinction, plus Homeward recovery. A smaller `A_t` is eligible for admission only after a strict reduction in a declared structural measure and successful independent checks; no cost estimate admits anything by itself.

The planner observes a vector, not an unexplained scalar: active nodes/edges, explicit call depth, adapter depth, duplicate subgraphs, recurring cycles, dependency volume, projection width and wrapper layers. It may select a Pareto-frontier proposal. Disjoint proposals may be checked concurrently; a committed reduction requires re-observation before the next choice. Neither the cost vector nor a finite panel proves a polynomial bound for arbitrary NP instances.

## Ten proposed evaluators and their discriminating checks

All rows in this table are **research candidates**, not v0.7 commands or tests.

| Evaluator | Possible structural change | Required negative probe |
|---|---|---|
| CanonicalShapeEvaluator | Canonicalize only certified shape-equivalent forms. | Similar-looking shapes with different occurrence or authority IDs remain distinct. |
| CompositionEvaluator | Replace a verified wrapper/composition chain with a compact representation. | A changed effect, call order or recovery obligation blocks fusion. |
| AdapterEvaluator | Shorten certified lift/lower adapter chains. | Different ownership, type contract or boundary effect blocks elimination. |
| DuplicateEvaluator | Share certified duplicate computation structure. | Equal values from distinct source occurrences retain separate event identity. |
| RecursionEvaluator | Summarize stabilized recurring active structure without discarding frames. | A repeated graph with changed live state or unbounded recursion yields no termination inference. |
| DependencyEvaluator | Remove irrelevant active dependency reachability under the declared obligation. | Changing a dependency that affects a required output invalidates the reduction. |
| ProjectionEvaluator | Narrow a wide active state with an exact recoverable residual. | A mandatory distinction hidden in the projected coordinates forces rejection. |
| RemainderEvaluator | Compact residual indexing while retaining original witnesses. | Negative outcome or lineage loss fails replay and blocks compaction. |
| StructuralPlannerEvaluator | Choose admissible proposals from the Pareto frontier. | A low estimated cost cannot override failed certificates or mandatory obligations. |
| StructuralCertificateEvaluator | Verify structural equality and exact recovery before admission. | A forged certificate for a smaller graph that deletes one mandatory occurrence is rejected and retained as counterevidence. |

`STRUCTURAL_EQ != BEHAVIORAL_EQ`. A structural certificate may justify a narrower active representation under a declared contract. It cannot assert semantic equivalence of arbitrary programs, authenticate compiled callbacks, promote cached output to evidence, or decide mathematical truth.

## Smallest implementation probe

1. Freeze a synthetic input history graph containing wrappers, adapter chains, equal-value/distinct-occurrence nodes, an apparent cycle, an irrelevant dependency and a wide state. Give each node stable source IDs, mandatory obligations and an exact recovery witness.
2. Generate one proposal with one evaluator. Before any state change, run an independent structural certificate checker and compare exact reconstruction against the frozen graph. Store every failed proposal and reason in history.
3. Admit only if the declared `K(A)` vector strictly improves in at least one structural component without worsening protected components or violating recovery/obligations. Keep the unreduced graph and predecessor links addressable. Repeat only after re-observing `A`.
4. Deliberately feed the checker one smaller graph that aliases two occurrences or deletes an authority boundary. Expected result: reject, preserve the failed proposal and original active structure.
5. Vary evaluator order. Each admitted path must reconstruct the same source and preserve mandatory distinctions. Record cost and receipts; do not infer order independence from a single sample.

The first code increment should implement the proposal/checker protocol on synthetic graphs, separately from the C23 VM and the frozen native Mirror engine. Connecting it to the runtime is a later, separately verified seam. The exact v0.7 Mirror tests and checkpoint hashes serve as regression anchors; they do not test this new protocol until an explicit integration test is added.

## Boundaries

```text
REPRESENTABLE != TOTAL
UNIVERSAL EVALUATOR != UNIVERSAL DECIDER
SELF-DESCRIPTION != SELF-AUTHORITY
FIXED POINT != CORRECT RESULT
STRUCTURAL_OPTIMIZATION != SEMANTIC REQUIREMENT
CACHED != VERIFIED
GENERATE != VERIFY != ADMIT
METHOD TRANSFER != EVIDENCE TRANSFER
FINITE PROBE != P = NP PROOF
```

Current v0.7 limitations remain: one concrete SelfState Mirror path, run-local handles, host-trusted callbacks, POSIX durable native I/O, quiescent persistence, process-local VM trace, no generic four-ID catalog, no first-class Meet4/Overlap or live cohort scheduler. The two Work research records remain the authority for the proposed evaluator family; this document makes the gap explicit for the Git repository without claiming it has closed in code.
