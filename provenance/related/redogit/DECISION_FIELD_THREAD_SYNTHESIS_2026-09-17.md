# Decision Field Thread Synthesis — 2026-09-17

**Status:** forward-only synthesis of the current thread and relevant recovered predecessors. This is not a rewrite of older projects and does not transfer evidence between domains.

## 1. Starting calibration — DEAN-4

The thread began with the exact problem:

```text
400 students
select exactly 100
never exceed dorm capacity
never violate exclusions
return exactly four valid results to the Dean
```

The scalable pairwise-exclusion form is Independent Set with a fixed four-result wrapper.

A reduction from Independent Set `(G,k)` to DEAN-4 uses `G disjoint-union K4`, target `q=k+1`, and capacity `k+1`:

- if `G` has an independent k-set `I`, then `I+{a}`, `I+{b}`, `I+{c}`, `I+{d}` are four valid cohorts;
- any valid `(k+1)`-cohort in `G union K4` contains at most one K4 vertex, hence at least k independent vertices from G.

Thus the unbounded pairwise-exclusion DEAN-4 family is NP-complete. This does not resolve `P ?= NP`; it gives a concrete NP-complete calibration.

## 2. Exact rotations developed in the thread

The same residual problem was repeatedly rotated through exact carriers:

```text
Independent Set
<-> Clique in complement
<-> Vertex Cover by omission
<-> Boolean SAT/cardinality
<-> 0/1 ILP
<-> SQL/factor graph
<-> polynomial/Macaulay relations
<-> contextual frontier
```

The governing rule became:

```text
ROTATION MAY CHANGE REPRESENTATION
ROTATION MAY NOT CHANGE THE OBLIGATION
```

## 3. Repair dynamics

For residual candidate count `m`, target `t`, define omission budget:

```text
ell = m - t
```

In the exclusion graph, if `deg(v) > ell`, `v` must be omitted from every feasible cohort.

Define repair pressure:

```text
rho(v) = deg(v) - ell
```

If a forced omission `v` is applied, the new pressure on surviving `u` is:

```text
rho'(u) = rho(u) + 1 - 1[(u,v) is an exclusion]
```

Equivalently, for forced set `F`:

```text
rho_F(u) = rho_0(u) + |F \ N(u)|
```

so pressure is monotone under forced repairs. Repair therefore became a fixed-point system:

```text
repair
-> budget change
-> degree/pressure change
-> LP change
-> components/modules/crowns
-> learned relations
-> repair again
```

Obstructions are not stopping conditions; they generate stronger attacks.

## 4. Gyroscopic architecture

The solver was generalized to rotate among carriers and apply every exact consequence to closure.

```text
Normalize
-> Detect consequential distinctions
-> Generate admissible operators
-> Probe certified gain/cost
-> Apply
-> Propagate
-> Learn conflicts/consensus
-> Rotate learned structure
-> Repair to fixed point
-> Repeat
```

"Infinity-dense gravity" was formalized as priority/closure, not infinite computation. Higher-order certified progress dominates lower-order heuristic gain.

Typical priority:

```text
terminal proof
>> irreversible fact
>> exact factorization
>> certified rank/nullity contraction
>> kernel/width reduction
>> heuristic steering
```

## 5. Maximal PCA / certified algebra

A critical correction was made: true semantic covariance over unknown solutions is useful for analysis but cannot be an operational oracle.

The executable carrier instead keeps a polynomial-size feature dictionary and linearizes only proved polynomial consequences:

```text
A y = b
```

with `y = Phi_M(x)`.

Certified nullity:

```text
d = |M| - rank(A)
M_affine = 2^d
```

For new exact relations `B y = c`, with `N` spanning `ker(A)`:

```text
g = rank(B N)
d' = d - g
M_affine' = M_affine / 2^g
```

SVD/PCA may steer toward resistant directions; exact rational/modular rank supplies proof.

The operational rule became:

> Attack the certified nullspace; if the current carrier cannot gain useful exact rank, rotate the refusal into another carrier.

## 6. Frontier and mass result

A decreasing measure down one branch is insufficient because binary branching can still create `2^n` states.

For live unresolved frontier `Q`, a sufficient theorem uses total semantic mass:

```text
Psi(Q) = sum_{S in Q} M(S)
```

If every expanded state admits an exact action with canonical unresolved children `T` satisfying:

```text
1 + sum M(T) <= M(S)
```

and initial mass is polynomial, then the total number of expansions is polynomial.

This is a sufficient proof template, not a proved universal property of NP instances.

## 7. Counterexamples retained

Several stronger universal claims were deliberately rejected:

- degree/pressure repair does not solve all regular hard cores;
- Nemhauser-Trotter/crown repair does not solve every connected non-bipartite regular core;
- one fixed exact frontier can contain exponentially many nondominated contexts;
- ordinary linear rank cannot compress every bad cut;
- universal small graph representatives can require exponential boundary size;
- one static compilation language can require exponential size;
- exact linear PCA on original variables can remain high-dimensional even for trivial instances (e.g. empty exclusion graph with fixed cardinality).

Failures are retained as constraints on the next design.

## 8. Quantum-collapse discussion

The thread then observed that quantum measurement also has a possibility -> outcome -> conditioned-update shape.

For a quantum instrument `{M_i}` and density operator `rho`:

```text
p_i = Tr(M_i rho M_i^dagger)
rho_i' = M_i rho M_i^dagger / p_i
```

This can instantiate the generic Decision Field structure:

```text
current state/context
-> possible outcomes and relations
-> physical measurement/evolution operator
-> observed outcome
-> conditioned next state
```

But the boundary is strict:

```text
SHARED_FIELD_STRUCTURE != SHARED_PHYSICAL_MECHANISM
QUANTUM_COLLAPSE != CLASSICAL_BOOLEAN_BRANCHING
```

Quantum mechanics supplies its own amplitudes, operator algebra and transition law.

## 9. Backward Library reconstruction

The Library search then showed that the same structural mechanism predated the phrase "Decision Field".

Recovered formulations included:

```text
Difference compressed down to distinctions
Operators are everywhere the carriers carry
```

and the empirical reset:

```text
conditions -> measurement -> observations -> derived difference -> interpretation
```

with an earned distinction only when collapsing/removing a difference changed a protected consequence in the declared scope.

This supports a chronology of:

```text
mechanism first
-> repeated rediscovery in different projects
-> later naming/generalization
```

rather than retrofitting all projects to a recent label.

## 10. Deeper invariant

The broader object is not necessarily decision. It is:

> **the dynamics of consequential distinctions under transformation.**

A domain-neutral field is parameterized by:

```text
DF = (X, O, D, R, F, E, G, U)
```

where:

- `X`: current possibilities/conditions;
- `O`: observer/task/obligation;
- `D`: task-local consequential distinctions (`X / ~_O`);
- `R`: typed relations;
- `F`: admitted functions/operators;
- `E`: evidence/provenance/uncertainty;
- `G`: desired bounded result;
- `U`: unresolved remainder.

Execution seeks a composition:

```text
f_k o ... o f_2 o f_1 (DF_0) -> DF_k
```

such that `Goal(DF_k)` is satisfied or impossibility is certified while all protected invariants and claim ceilings are preserved.

## 11. Function vocabulary

The canonical operator surface is:

```text
Observe
Distinguish
Relate
Filter
Select
Transform
Project
Branch
Compose
Verify
Repair
Learn
Reconstruct
Terminate
```

Domains may add specialized functions but must declare their laws explicitly.

## 12. Domain rotations without evidence contamination

### P vs NP / SAT

Decision Field supplies research machinery; `P ?= NP` remains OPEN.

### Hodge

The same machinery may organize proof admission, rank deficits and counterprobes; it does not transfer P-vs-NP evidence or constitute Hodge proof.

### GSFL

Decision Field supplies the outer possibility/obligation/operator loop; GSFL supplies semantic rotation/invariant-preserving fitting within that loop.

### Orbit / Library

Decision Field supplies task-local distinction and operator selection; Orbit supplies provenance/recovery/authority structure.

### R3 / Recursive Scientific Reconstruction

Decision Field supplies one formal action surface; R3 supplies recursive decomposition, variation, testing, counterexamples and conservative promotion.

### Human planning

Decision Field may model choices and constraints but does not own human goals or agency.

### Quantum mechanics

Decision Field describes structural bookkeeping only; quantum physics determines the actual transition laws.

## 13. Current implementation locations

- canonical specification: `redogit/redogit/DECISION_FIELD_CORE_2026-09-17.md`
- implementation profile: `redogit/Other-Projects-/Decision Field Operator Lab/DECISION_FIELD_CORE_2026-09-17.md`
- generic Python kernel: `redogit/Other-Projects-/Decision Field Operator Lab/decision_field.py`
- kernel tests: `redogit/Other-Projects-/Decision Field Operator Lab/test_decision_field.py`

## 14. Claim ceiling

This synthesis establishes a reusable software/research abstraction and several exact bounded mathematical results used by its specializations.

It does **not** establish:

```text
P = NP
P != NP
Hodge conjecture
universal semantics
universal decision theory
quantum interpretation
physical extra dimensions
machine consciousness
human decision optimality
```

Those remain separate obligations.
