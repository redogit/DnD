# Decision Field Core — 2026-09-17

**Status:** canonical cross-project structural specification; not a claim that all domains share one physical mechanism.

## Core idea

A recurring structure across the work is broader than decision-making. The common object is the dynamics of consequential distinctions under transformation.

The domain-neutral state is parameterized as:

```text
DecisionField<Possibility, Distinction, Relation, Operator, Observation, Evidence, Goal>
```

with one active field and a set of functions that operate on it and transform it toward a declared result.

## Parameterization

Let

- `X` = current possibility / condition space;
- `O` = observer, task, goal, or protected obligation;
- `~_O` = consequential-equivalence relation under the current obligation;
- `D = X / ~_O` = consequentially distinct states;
- `R` = typed relation structure over `D`;
- `F = {f_i}` = admitted exact or evidence-qualified operators;
- `E` = evidence, provenance, chronology, uncertainty and claim ceiling;
- `G` = desired result / obligation / acceptance condition;
- `U` = unresolved remainder.

A complete field is:

```text
DF = (X, O, D, R, F, E, G, U)
```

Each operator is explicitly typed:

```text
f_i : DF -> DF'
```

and must declare:

```text
preconditions
preserved invariants
allowed mutation
cost/resource bound
evidence produced
failure mode
reconstruction/lift map when destructive
claim ceiling
```

The execution problem is:

```text
Given DF0 and Goal G,
select/compose functions f_1 ... f_k
such that Result(DFk) satisfies G,
while preserving all declared invariants and evidence boundaries.
```

## Consequential-distinction rule

A difference between `x` and `y` is promoted to an active distinction only when collapsing it changes a protected consequence in the declared scope.

```text
x ~_O y
iff
no admitted consequential test currently distinguishes x and y for O.
```

A failed test does not imply identity; it only means the tested obligation did not require that distinction.

## Dynamics

```text
Difference
-> Test / Observation / Intervention
-> Consequence
-> Earned Distinction
-> Relation
-> Operator / Transform
-> New Condition
-> Observation
-> Repair / Learning / Reconstruction
-> revised distinction field
```

Selection is therefore one operator inside the larger dynamics, not the universal primitive.

## Function classes

A Decision Field implementation may register operators from several classes:

```text
Observe      : DF -> Observation
Distinguish  : (DF, Observation) -> DF
Relate       : DF -> DF
Filter       : DF -> DF
Select       : DF -> Candidate
Transform    : DF -> DF
Project      : DF -> DF
Branch       : DF -> {DF_i}
Compose      : {f_i} -> f
Verify       : DF -> Certificate
Repair       : DF -> DF
Learn        : {DF_i, Certificate_i} -> DF
Reconstruct  : DF_reduced -> DF_source_result
Terminate    : DF -> Result
```

Functions may be deterministic, probabilistic, quantum-domain-specific, human-selected, or externally controlled. Their local transition law belongs to the domain and must not be inferred from the shared structural schema.

## Domain specializations

### SAT / NP search

```text
X = assignments / solver states
R = clauses, implications, learned relations
F = simplify, branch, project, resolve, repair, certify
G = SAT witness or UNSAT certificate
```

### DEAN / Independent Set

```text
X = candidate cohorts
R = exclusions, capacities, derived relations
F = filter, cover dualize, LP, frame hammer, MPCA, branch, verify
G = four valid cohorts
```

### Quantum measurement

```text
X = quantum state + admissible measurement context
R = tensor/nonseparable relations, operator algebra
F = physical evolution + measurement instrument
G = observed outcome / conditioned post-measurement state
```

The shared field structure does **not** imply Boolean underlying quantum dynamics or a shared physical mechanism.

### Planning / human action

```text
X = available actions / conditions
R = dependencies, resources, consent, constraints
F = inspect, choose, act, observe, revise
G = user-declared outcome or next bounded step
```

### Knowledge / Orbit

```text
X = recoverable knowledge states
R = provenance, chronology, lineage, source/witness relations
F = retrieve, compare, transport, reconstruct, admit
G = task-local reconstructibility
```

### GSFL / semantic fitting

```text
X = candidate representations
R = source meaning, task context, invariants
F = semantic rotations and fitters
G = usable human surface with reconstructible invariants
```

## Gyroscopic execution

The active solver may rotate the same obligation across different exact carriers:

```text
Graph <-> Cover <-> LP <-> SAT <-> SQL/Factor <-> Algebra <-> MPCA <-> Context
```

A rotation may change representation but not silently change the obligation.

Operational loop:

```text
normalize
-> detect consequential distinctions
-> generate admissible operators
-> estimate certified gain/cost
-> execute best justified operator
-> propagate all consequences
-> learn common/conflict information
-> rotate learned structure into other carriers
-> repair to fixed point
-> repeat until Goal or certified impossibility
```

## Infinite-density gravity

"Infinity" is represented as priority/closure, not literal infinite computation.

Use lexicographic priorities or a formal large-weight limit:

```text
terminal proof
>> irreversible forced fact
>> exact factorization
>> certified rank/nullity contraction
>> width/kernel reduction
>> heuristic steering gain
```

Every repair triggers re-evaluation of all carriers. Obstructions are attack generators, not stop markers.

## Maximal PCA / certified algebra carrier

For a polynomial-size feature dictionary `M`, all exact proved polynomial consequences are linearized into:

```text
A y = b
```

where `y = Phi_M(x)`.

Certified nullity:

```text
d = |M| - rank(A)
M_affine = 2^d
```

A new action adding `B y = c` gains exactly:

```text
g = rank(B N)
```

where `N` spans `ker(A)`.

Then:

```text
d' = d - g
M_affine' = M_affine / 2^g
```

SVD/PCA steers; exact rational/modular rank proves.

## Claim boundaries

```text
SHARED_STRUCTURE != SHARED_MECHANISM
ANALOGY != EVIDENCE_TRANSFER
REPRESENTATION_EQUIVALENCE != PHYSICAL_EQUIVALENCE
FINITE_VERIFICATION != UNIVERSALITY
FIT != TRUTH
BYTE_IDENTITY != SEMANTIC_TRUTH
QUANTUM_MEASUREMENT != CLASSICAL_BOOLEAN_SELECTION
MODEL != PERSON
INDEX != AUTHORITY
```

## One-sentence form

> A Decision Field is a parameterized possibility space whose consequential distinctions, typed relations, evidence and goal determine which admissible functions may transform it; the system repeatedly applies, verifies, learns from and composes those functions until the desired bounded result or a certified obstruction is reached.
