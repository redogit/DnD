# RMAL / RMALKDVMLLL — Syntax and Expressiveness Specification

**Status:** integrated language specification for RMAL Tool Chain 2.1.1  
**Machine-readable source:** `src/rmalc/language_spec.json`  
**Introspection:** `rmalc language-spec`

## 1. Identity and authority boundary

- **RMAL — Ryan McMillan April Language** is the executable multi-surface programming language.
- **RMALKDVMLLL — Ryan McMillan April Knowledge Decay Virtual Machine Link Layer Language** is the semantic VM/link/reconstruction/admission layer.
- The compact RMALKDVMLLL surface is specified in `docs/RMALKDVMLLL_COMPACT_SURFACE.ebnf`; this integration does **not** silently claim that surface is a core RMALC parser frontend.

```text
RMAL source/surface
  -> typed semantic object
  -> RMAL IR / RMAL-SIR2
  -> transforms / solvers / verification
  -> RMALKDVMLLL link/reconstruction/admission discipline
  -> RMALBC1 -> RMALOBJ1 -> RMALEXE1
```

The core rule is `SURFACE != SEMANTICS`.

## 2. Semantic object

The governed semantic unit is modeled by the fields:

```text
subject surface semantics context state history obligations authority
 evidence carrier links cost remainder
```

Representation may change only within its declared transform contract. Evidence and authority never strengthen merely because an object was parsed, linked, compressed, translated, optimized or transported.

## 3. Layer model

```text
L0 SOURCE / CUSTODY
L1 SURFACE
L2 SEMANTIC OBJECT
L3 LINK
L4 EXECUTION
L5 VERIFICATION
L6 ADMISSION
L7 KD / RECOVERY
```

## 4. First-use specification gate

Before shorthand, consequential work may bind:

```text
TARGET GOAL SCOPE DIRECTION
PRESERVE ALLOW FORBID
PRIORITY SUCCESS FAILURE
EVIDENCE OUTPUT RESOURCE_BOUNDS
AUTHORITY CONTEXT TIME
```

These values define the problem contract inherited by subsequent shorthand until explicitly changed.

## 5. Implemented RMAL syntax

The implemented canonical grammar remains in `docs/EBNF.md`. Core binding forms are:

```rmal
LET x = expression
CONST limit = expression
STATE counter = expression
SET counter = expression
```

Core control/contract forms include:

```rmal
REQUIRE condition
PRESERVE meaning provenance
ALLOW representation_change
COUNTERPROBE "independent check"
VERIFY "declared verification"
LET x = IF condition THEN a ELSE b
CONTINUE
STOP
```

Declarative families are:

```text
TYPE TRAIT CLASS RELATION OPERATOR CONFIG PROFILE CONTEXT
CLAIM EVIDENCE RULE LANGUAGE
```

Compiler/module directives are:

```text
MODULE IMPORT EXPORT EXTERN SURFACE TARGET
```

The S-expression adapter additionally provides `let`, `const`, `state`, `set`, `if`, `begin`, `lambda`, `apply`, `compose`, and `require`.

## 6. Multiple surfaces, one semantic target

RMAL supports canonical, user-native and S-expression executable frontends, with functional forms inside supported surfaces. SQL/JSON/human and external language representations are capability-gated carriers/backends. All mappings are subordinate to semantic preservation; unsupported lowering fails closed instead of inventing approximate behavior.

## 7. Executable-universe forms

```rmal
FIND "Hodge" KIND OPEN_PROBLEM LIMIT 10
PATH Language.RMAL TO Problem.Hodge MAX_DEPTH 8
LENS "P vs NP" DEPTH 2 MAX_ENTITIES 100
SURVIVE KINDS OPEN_PROBLEM,PROJECT EXCLUDE_STATUS REJECTED LIMIT 100
```

The associated hard-problem planning sequence is:

```text
DISTINGUISH -> GROUND -> GENERATE -> ROTATE -> SPLIT -> FIT -> SELECT
-> COUNTERPROBE -> VERIFY -> REPAIR -> RECONSTRUCT -> CONTINUE
```

`PATH != PROOF`, `RETRIEVAL != EVIDENCE_TRANSFER`, and `PLAN != SOLUTION` remain hard boundaries.

## 8. RMALKDVMLLL compact semantic/meta surface

The compact surface supports:

```text
TAKE GOAL SCOPE PRESERVE ALLOW FORBID BOUND
GENERATE FIT CONSTRAIN MUTATE ROTATE COMPOSE
VERIFY COUNTERPROBE CLASSIFY BUILD ARCHIVE REPEAT
```

Representative forms:

```text
GENERATE expression
FIT expression FOR expression
MUTATE expression BY operator
ROTATE expression ON axis
COMPOSE op1 >> op2 >> op3
VERIFY expression BY { checks }
COUNTERPROBE expression WITH probe
CLASSIFY expression AS VERIFIED_WITHIN_SCOPE
REPEAT UNTIL condition
```

This surface expresses search, transformation, validation and evidence-state transitions. Its grammar is specified, but parser implementation status remains explicit rather than inferred from documentation.

## 9. Evidence states

The integrated evidence vocabulary is:

```text
UNEXPLORED FORMULATED INTERNALLY_CONSISTENT PLAUSIBLE TESTED
COUNTERPROBED REPLICATED BOUNDED_CLAIM VERIFIED_WITHIN_SCOPE
CONTRADICTED FALSIFIED INVARIANT_FAILURE HIDDEN_COST ORACLE_SHIFT
UNRESOLVED
```

A computed value and the evidence status of that value are distinct objects.

## 10. Typed transform contract

Every verified transform declares:

```text
source_type target_type domain
preconditions postconditions
preserved_invariants allowed_changes
cost_bound inverse_or_reconstruction
claim_ceiling provenance
```

The reversible carrier algebra contains:

```text
IDENTITY COMPOSE INVERSE ROTATE FOLD FLIP UNFOLD
RECONSTRUCT COMPARE_INVARIANTS
```

A transform cannot enter verified IR merely because its description is metaphorically plausible.

## 11. BBF and semantic rotation

The Bidirectional Bitfolding Flip audit is:

```text
FOLD -> FLIP -> UNFOLD -> COMPARE_INVARIANTS -> MEASURE_LOSS_INTRODUCTION
```

A claimed semantic rotation is admitted only when required invariants reconstruct. Otherwise the result is recorded as mutation, loss, introduction, aliasing, leakage, cost shift, oracle shift, unbounded behavior or unresolved remainder as applicable.

## 12. Decision fields

A DecisionField is the currently consequential distinction set. Its solver vocabulary includes:

```text
RESOLVE
PROVE_IRRELEVANT
MERGE_EQUIVALENT
SPLIT_ALIAS
EXPOSE_DEPENDENCY
REPLACE_WITH_SUFFICIENT_STATISTIC
ROTATE_FIELD
REJECT_LOSSY_MERGE
```

The objective is not unconditional `|D| -> 0`; it is removal of only those distinctions removable under the active obligations.

## 13. Meta-solving and POLYMETA

Meta-solving searches solver plans rather than only answers:

```text
Problem -> Formulations -> Constraint indexes -> Solver-plan generation
-> Semantic rotations -> Candidate solver execution -> Pareto comparison
-> Counterprobe -> Independent verification -> Admission
```

Result states remain distinct:

```text
FOUND_CANDIDATE PROVED_UNSAT PROVED_OPTIMAL
BOUNDED_BEST_FOUND BUDGET_EXHAUSTED UNRESOLVED
```

`BUDGET_EXHAUSTED != OPTIMAL` is a semantic barrier.

Higher-order levels are:

```text
M0 data / semantic objects
M1 transforms / solvers / compiler passes
M2 selectors over M1
M3 meta-solvers / optimizer-of-optimizers
M4 POLYMETA compositions over typed M2/M3 families
```

Meta-level composition does not grant additional evidence authority.

## 14. Knowledge Decay and reconstruction

Knowledge Decay is represented as loss of reconstructibility across time, carrier, context, people or tools. Reconstruction packets preserve, as applicable:

```text
source assumptions chronology operator chain failures counterexamples
learned distinctions evidence claim ceiling authority state
reproduction path privacy/access constraints remainder
```

A compact representation that exports consequential cost into reconstruction is not automatically superior.

## 15. Expressiveness

The integrated language model can represent:

- ordinary value, state and conditional computation;
- immutable and mutable bindings;
- higher-order functional composition;
- typed semantic declarations and relations;
- constraints, invariants, effects and authority;
- claims, evidence, provenance and counterexamples;
- semantic rotation and explicit mutation;
- reversible carrier transformations;
- survivor/query and executable-universe operations;
- decision-field solving;
- bounded meta-solving and meta-optimization;
- counterprobe/verification/admission workflows;
- Knowledge Decay recovery;
- multiple source/output carriers with fail-closed capability negotiation.

This is broader semantic expressiveness than the textual grammar alone. The specification therefore distinguishes parser syntax from typed IR and higher-order semantic machinery.

## 16. Constitutional non-equivalences

```text
SURFACE != SEMANTICS
RAW_SURFACE != NORMALIZED_SURFACE != SEMANTIC_IR
PARSE != TRUTH
GENERATE != VERIFY != ADMIT
PLAN != SOLUTION
ROTATION != MUTATION
FIT != TRUTH
UNKNOWN != ABSENT
UNRESOLVED != FALSE
RELATED != SUPPORTS
FINITE_VERIFICATION != UNIVERSALITY
CARRIER_AGREEMENT != INDEPENDENT_VERIFICATION
BUDGET_EXHAUSTED != OPTIMAL
COMPILED != SCIENTIFICALLY_VALID
LINKED != PROVED
TRACE != PROOF
```

## 17. Introspection and verification

```bash
rmalc language-spec --format summary
rmalc language-spec --format json
rmalc language-spec --format canonical-ebnf
rmalc language-spec --format compact-ebnf
rmalc language-spec --section transform_contract_fields
rmalc language-spec --verify
```

The machine-readable specification is content-digested. `--verify` checks required boundary distinctions, core surface declarations, transform fields, solve-state distinctions and the non-promotion status of the compact RMALKDVMLLL surface.

## 18. Non-claims

This integration does not claim Turing completeness solely from the published grammar. That property requires a separate formal demonstration.

Likewise, compilation, linking, optimization, carrier agreement, or finite verification does not establish scientific truth or universal correctness.