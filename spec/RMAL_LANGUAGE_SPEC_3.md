# RMAL 3 Formal Language Contract

**Canonical repository:** `redogit/DnD`  
**Implementation language:** C++20  
**Language identity:** RMAL — Ryan McMillan April Language

## 1. Authority model

RMAL is formalized from four distinct evidence classes:

1. **Native current implementation** — behavior established by the C++ source in this repository.
2. **Recovered executable predecessor** — behavior documented/validated in retained RMAL Tool Chain 2.1.x artifacts.
3. **Specified semantic surface** — defined language/VM/link/reconstruction semantics not yet fully lowered by the current C++ compiler.
4. **Historical/recovery material** — chat, Library, repository, or predecessor material retained for reconstruction but not automatically canonical.

```text
RECOVERED != CURRENT
PARSED != EXECUTED
EXECUTED != VERIFIED
VERIFIED != ADMITTED
COMPILED != SCIENTIFICALLY_VALID
```

## 2. Current compilation path

```text
RMAL source
 -> lexer
 -> parser
 -> AST
 -> RMALBC1 lowering
 -> VM execution
 -> trace / audit
```

Target architecture preserved from the 2.1.x lineage:

```text
RMAL source/surface
 -> normalized semantic object
 -> RMAL IR
 -> RMAL-SIR2
 -> transforms / solvers / verification
 -> RMALKDVMLLL link / reconstruction / admission discipline
 -> RMALBC1
 -> RMALOBJ1
 -> RMALEXE1
```

The second path is the formal target architecture. The current C++ implementation does **not** yet establish all stages.

## 3. Implementation-status classes

### A. EXECUTABLE_CURRENT

These forms currently lower to VM behavior:

```text
MODULE
CONST
LET / let
STATE
SET
fn
if / else
while
return
print
assert / ASSERT
REQUIRE
STOP
```

Current value domain:

```text
nil
int64
bool
string
```

Current expression operators:

```text
! -
* / %
+ -
< <= > >=
== !=
&& ||
function-call
```

### B. PARSED_CARRIER_CURRENT

These forms are accepted by the native parser and preserved as directive carriers, but do not yet receive full typed semantic execution:

```text
EXPORT IMPORT EXTERN SURFACE TARGET
TYPE TRAIT CLASS RELATION RELATE OPERATOR CONFIG PROFILE CONTEXT
CLAIM EVIDENCE RULE LANGUAGE ENTITY ATTRIBUTE BOUNDARY
PRESERVE ALLOW DENY FORBID OBLIGATION INVARIANT REMAINDER
COUNTERPROBE VERIFY CONTINUE TRACE
FIND PATH LENS SURVIVE
TAKE GOAL SCOPE BOUND
GENERATE FIT CONSTRAIN MUTATE ROTATE COMPOSE
CLASSIFY BUILD ARCHIVE REPEAT
```

**Important:** acceptance as a carrier is not semantic implementation.

```text
PARSED_CARRIER_CURRENT != EXECUTABLE_CURRENT
```

### C. SPECIFIED_TARGET

Recovered 2.1.x architecture defines or names:

- multiple source surfaces: canonical, user-native, S-expression;
- typed semantic object;
- RMAL IR;
- partial RMAL-SIR2 proof/contract envelope;
- verified optimizer concept;
- domain specialization/capability negotiation;
- RMALBC1;
- RMALOBJ1;
- linker;
- RMALEXE1;
- debugger;
- disassembler/manifest;
- reproducibility/audit tools;
- backend projections such as C++, Python, JavaScript, SQL, JSON and human-readable surfaces;
- RMALKDVMLLL compact semantic/meta surface;
- Decision Field operations;
- Knowledge Decay recovery packets;
- bounded meta-solving/POLYMETA levels.

These remain targets unless a current executable witness exists.

### D. HISTORICAL_OR_RECOVERED

Any construct found only in predecessor files, Library artifacts, chat recovery, or adjacent repositories remains historical/recovered until explicitly promoted.

## 4. Governed semantic object

Recovered semantic-object fields:

```text
subject
surface
semantics
context
state
history
obligations
authority
evidence
carrier
links
cost
remainder
```

The current VM does not yet encode this entire record as a first-class runtime type.

## 5. Layer model

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

These layers are semantic governance boundaries, not merely compiler phases.

## 6. First-use contract

Recovered first-use fields:

```text
TARGET
GOAL
SCOPE
DIRECTION
PRESERVE
ALLOW
FORBID
PRIORITY
SUCCESS
FAILURE
EVIDENCE
OUTPUT
RESOURCE_BOUNDS
AUTHORITY
CONTEXT
TIME
```

A future typed implementation should bind these as a contract object rather than only opaque directives.

## 7. Evidence-state vocabulary

```text
UNEXPLORED
FORMULATED
INTERNALLY_CONSISTENT
PLAUSIBLE
TESTED
COUNTERPROBED
REPLICATED
BOUNDED_CLAIM
VERIFIED_WITHIN_SCOPE
CONTRADICTED
FALSIFIED
INVARIANT_FAILURE
HIDDEN_COST
ORACLE_SHIFT
UNRESOLVED
```

Computed value and evidence state are separate.

## 8. Transform contract

A verified transform is specified by:

```text
source_type
target_type
domain
preconditions
postconditions
preserved_invariants
allowed_changes
cost_bound
inverse_or_reconstruction
claim_ceiling
provenance
```

Reversible carrier algebra:

```text
IDENTITY
COMPOSE
INVERSE
ROTATE
FOLD
FLIP
UNFOLD
RECONSTRUCT
COMPARE_INVARIANTS
```

## 9. RMALKDVMLLL boundary

RMALKDVMLLL means:

**Ryan McMillan April Knowledge Decay Virtual Machine Link Layer Language**

It governs the broader semantic VM/link/reconstruction/admission layer.

Its recovered compact vocabulary includes:

```text
TAKE GOAL SCOPE PRESERVE ALLOW FORBID BOUND
GENERATE FIT CONSTRAIN MUTATE ROTATE COMPOSE
VERIFY COUNTERPROBE CLASSIFY BUILD ARCHIVE REPEAT
```

The current parser may preserve these as directive carriers. That does not establish the compact surface as a fully executable core frontend.

## 10. RMAL-TFEML boundary

RMAL-TFEML is the **Trace-First Execution Memory Layer**.

```text
Program / transform
 -> execution space
 -> trace
 -> evidence attachment
 -> residual
 -> repair
 -> reprojection
 -> continuation
```

`TRACE != PROOF`

## 11. Decision Field operations

Recovered semantic operations:

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

These are specified semantic operations; the current C++ VM does not yet implement them as typed opcodes.

## 12. Meta-level model

```text
M0 data / semantic objects
M1 transforms / solvers / compiler passes
M2 selectors over M1
M3 meta-solvers / optimizer-of-optimizers
M4 POLYMETA compositions over typed M2/M3 families
```

Meta-level composition carries no automatic evidence promotion.

## 13. Knowledge Decay rule

Prefer a carrier transition `C -> C'` only when required semantics are preserved and relevant decay risk does not increase, ideally:

```text
D(C') < D(C)
```

Threatened distinctions include:

- consequential semantics;
- originating obligation;
- provenance;
- evidence authority;
- transform trace;
- unresolved remainder;
- recoverability/Homeward path;
- explicit cost.

## 14. Constitutional non-equivalences

```text
SURFACE != SEMANTICS
RAW_SURFACE != NORMALIZED_SURFACE != SEMANTIC_IR
PARSE != TRUTH
PARSED != EXECUTED
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
HISTORY != CURRENT_AUTHORITY
CURRENT_SYNTHESIS != RETROACTIVE_NAMING
```

## 15. Promotion rule

A recovered construct may move toward canonical executable RMAL only through:

```text
SOURCE
 -> IDENTITY
 -> SEMANTIC CONTRACT
 -> PARSER WITNESS
 -> LOWERING WITNESS
 -> EXECUTION WITNESS
 -> TEST / COUNTERPROBE
 -> DOCUMENTED CLAIM CEILING
 -> PROMOTION
```

No step is inferred from adjacency.
