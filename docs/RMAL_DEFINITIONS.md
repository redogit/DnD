# RMAL Definitions

## Current canonical names

### RMAL
**Ryan McMillan April Language.**

A programming language for executable obligations, state, relations, transforms, evidence/provenance carriers, preservation, verification and reconstruction.

RMAL is the executable language surface.

### RMALC
**Ryan McMillan April Language Compiler.**

The compiler/toolchain authority for RMAL source. Compiler success establishes compiler behavior only.

### RMAL-SIR / RMAL-SIR2
Typed semantic intermediate representation.

Carries normalized program meaning between source parsing and lowering/runtime stages.

### RMALBC1
Executable RMAL bytecode carrier.

`BYTECODE != SEMANTIC_TRUTH`

### RMALOBJ1
Relocatable/linkable RMAL object carrier.

`OBJECT_VALIDITY != CLAIM_VALIDITY`

### RMALEXE1
Linked RMAL executable bundle.

`EXECUTION != EVIDENCE`

### RMALKDVMLLL
**Ryan McMillan April Knowledge Decay Virtual Machine Link Layer Language.**

Broader semantic VM/link/reconstruction/admission layer for enumerating, transforming, mutating, rotating, fitting, linking, testing and reconstructing semantically distinct formulations while preserving way-back paths and threatened distinctions.

It does not choose human purpose and it does not silently redefine native project truth.

### RMAL-TFEML
**RMAL Trace-First Execution Memory Layer.**

Programs, transformations, carriers, VM transitions and obligation-resolution attempts are recorded as execution spaces whose traces are first-class memory/provenance objects.

## Programming surface

Current and preserved programming constructs include:

```text
MODULE
EXPORT
CONST / LET
STATE / SET
fn
if / else
while
return
print
assert
CONTEXT
RELATION
REQUIRE
PRESERVE
ALLOW
DENY
OBLIGATION
INVARIANT
CLAIM
EVIDENCE
COUNTERPROBE
VERIFY
REMAINDER
SURFACE
TRACE
CONTINUE
```

The compiler implementation may evolve, but syntax additions must preserve predecessor recoverability and explicit semantics.

## Core semantic boundaries

```text
SURFACE != SEMANTICS
GENERATE != VERIFY != ADMIT
UNKNOWN != FALSE
RELATED != SUPPORTS
FIT != TRUTH
OPTIMIZATION != ADMISSION UNTIL VERIFIED
TRACE != PROOF
COMPILED != SCIENTIFICALLY_VALID
SUCCESSOR != REWRITTEN_PREDECESSOR
FINITE_VERIFICATION != UNIVERSALITY
```

## Knowledge Decay relation

Knowledge Decay is directional: representations and relations should move away from threatened loss of consequential distinctions, provenance, obligation, evidence authority, transform trace, unresolved remainder and Homeward/recovery paths.

RMALKDVMLLL and RMAL-TFEML provide the execution/link/reconstruction surfaces for that requirement.

## Trace-first chain

```text
Object
 -> Surface
 -> Carrier
 -> Trace
 -> Residual
 -> Repair
 -> Reprojection
```

Execution coordinates may change; object identity and reconstructible provenance must remain recoverable.

## Historical naming rule

Current names are not projected backward onto older artifacts unless the source itself used them.

`CURRENT_SYNTHESIS != RETROACTIVE_NAMING`
