# REDOGIT

`redogit` is the working rule for these repositories:

> **Do it. Inspect what happened. Preserve the history. Redo the smallest thing that matters better. Verify the successor. Repeat.**

A redo is not a clean-room claim that the earlier work never existed. Git already gives us the right machinery: history, diffs, branches, commits, and successors.

## The loop

1. **Input** — load the actual code, artifact, failure, or idea that exists.
2. **Boundary** — identify what the project is responsible for and what it is not.
3. **Difference** — find the consequential defect, dependency, ambiguity, or missing behavior.
4. **Redo** — build the smallest coherent successor that addresses that difference.
5. **Check** — make the important behavior observable and executable where possible.
6. **Promote** — call a successor `current` only after its declared check passes. An unverified successor is a candidate, not the current answer.
7. **Retain** — keep predecessor, failure, unresolved remainder, source-native identity, and provenance in history.
8. **Continue** — extend only when the next difference earns the extra complexity.

## Repository contract

A repository may expose its current state in `redogit.json`, governed by `REDOGIT.schema.json`.

The contract states:

- which successor is current;
- where it lives;
- how to build and verify it, when applicable;
- which predecessors remain retained;
- what the repository owns and explicitly does not own;
- whether an external blocker prevents normal verification;
- how claims are bounded by evidence;
- how assumptions, tests, unknowns, and Knowledge Decay are carried;
- which consequential distinctions must not be silently collapsed.

For executable repositories, the shared reusable workflow `.github/workflows/verify-redogit.yml` validates the contract and executes the declared build and verification commands. Callers should pin a tested commit of that workflow rather than follow a moving branch implicitly.

A repository with an external runner or service blocker must record the blocker rather than manufacture a green status. A provenance-only fork may declare `provenance-boundary` and verify attribution/boundary without pretending inherited code has been rebuilt.

## Research policy

The common research surface is `I/R/P/O`:

- `I` — the bounded input actually available;
- `R` — scratch/research state, relations, evidence, and unresolved context used to reason about the task;
- `P` — the selected plan, bounded by the current obligation and evidence;
- `O` — the observable output, including consequence, evidence state, and unresolved remainder.

Every repository contract carries the three checks `assumption`, `test`, and `unknown`, and separates four evidence classes:

1. executed-and-verified results;
2. externally validated findings;
3. formal consequences from accepted premises;
4. hypotheses or open questions.

Those classes are not interchangeable. A claim stops at the strongest evidence actually supporting it.

Selection is task-local and minimal-necessity. Unselected possibilities are not false, and a selected path is not globally optimal merely because it was selected. Knowledge Decay remains active whenever evidence age, accessibility, dependency drift, or changed external facts can affect the current claim.

The required distinctions are machine-checked by the shared validator. Among them:

- `UNKNOWN != ABSENT`
- `INDEX_MISS != ABSENCE`
- `RELATED != SUPPORTS`
- `SEMANTIC_SIMILARITY != IDENTITY`
- `SOURCE != RECONSTRUCTION`
- `BYTE_IDENTITY != SEMANTIC_TRUTH`
- `OBSERVATION != INTERPRETATION`
- `VIEWPOINT_CHANGE != TASK_CHANGE`
- `SELECTION != GLOBAL_OPTIMALITY`
- `FINITE_VERIFICATION != UNIVERSALITY`
- `LOSS_ACKNOWLEDGED != LOSS_CONCEALED`
- `EVALUATION_COMPLETE != PROMOTION_APPROVED`
- `PERSON != RECORDED_MODEL`
- `USER_GOAL != SYSTEM_GOAL`
- `PREDECESSOR != SUCCESSOR`
- `INTERNAL_CONSISTENCY != EXTERNAL_VALIDATION`
- `CLAIM != EVIDENCE`

Repositories may add domain-specific distinctions, but they may not remove the common ones.

## Promotion invariant

The current pointer moves forward only after evidence exists:

`predecessor -> bounded difference -> candidate successor -> declared check -> PASS -> current successor`

If the check fails:

`candidate successor -> failure evidence -> next bounded redo`

The predecessor is not rewritten to hide the failure, a failed candidate is not promoted by documentation alone, and newer lessons do not retroactively rewrite a verified historical checkpoint.

## What REDOGIT rejects

- pretending old mistakes never happened;
- copying accidental architecture forward because it is already there;
- mixing unrelated projects because their names overlap;
- opaque generated dumps when a smaller reconstructible source exists;
- calling something complete without an observable completion condition;
- declaring a successor current before its own declared verification passes;
- hiding external blockers behind false or unrelated green checks;
- treating missing retrieval as absence;
- treating similarity as identity;
- treating a passing finite test as a universal proof;
- silently concealing semantic loss or unresolved remainder.

## What REDOGIT keeps

- useful ideas;
- authorship and upstream attribution;
- failures as evidence;
- unresolved remainder;
- source-native identity;
- small testable cores;
- explicit successors;
- machine-readable current-state contracts;
- claim ceilings and evidence classes;
- freedom to redo the successor again.

Git is not only where the finished answer goes. It is the lineage of learning how to make the answer better.
