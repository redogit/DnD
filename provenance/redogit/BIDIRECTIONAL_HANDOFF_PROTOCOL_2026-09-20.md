# Bidirectional Handoff Pairity Protocol — 2026-09-20

**Status:** `CURRENT COORDINATION SUCCESSOR / MACHINE-READABLE CONTRACT / TARGET-LOCAL AUTHORITY`

This protocol makes "both ways" explicit without permitting silent cross-writing.

It succeeds the one-way shorthand:

```text
REFERENCE
-> PROPOSAL
-> HANDOFF
-> TARGET_DECISION
-> TARGET_LOCAL_SUCCESSOR
```

by adding a required return relation:

```text
A
-> REQUEST / PROPOSAL
-> B

B
-> TARGET-LOCAL DECISION
-> RESPONSE
-> A
```

The return path does not transfer authority back to the requester. It reports what the target decided under its own authority.

## 1. Pairity rule

A complete handoff relation preserves both directed edges:

```text
A -> B
B -> A
```

and the relation between them.

```text
REQUEST != COMMAND
RESPONSE != AUTHORITY_TRANSFER
ACCEPTED != VERIFIED
REJECTED != FAILURE
NEEDS_EVIDENCE != REJECTED
UNRESOLVED != FALSE
```

A response may preserve mismatch. Parity is never forced.

## 2. Request packet

A request must carry enough information for the target to decide locally:

```text
handoff_id
from
to
subject_uid
goal
obligation
request_class
requested_action
source_refs[]
evidence_refs[]
privacy
claim_ceiling
unresolved[]
way_back[]
```

A request may reference external evidence. It may not import that evidence's authority.

## 3. Response packet

The target returns a response linked to the exact request:

```text
response_id
in_reply_to
from
to
status
decision_reason
successor_refs[]
evidence_refs[]
unresolved[]
privacy
claim_ceiling
way_back[]
```

Allowed statuses:

```text
ACCEPTED
REJECTED
NEEDS_EVIDENCE
UNRESOLVED
```

`ACCEPTED` means the target accepted the bounded request under its local authority. It does not mean the resulting artifact is scientifically verified unless the response separately cites target-local verification evidence.

## 4. Privacy monotonicity

A handoff may preserve or strengthen a privacy boundary. It may not silently weaken one.

```text
PRIVATE -> PRIVATE         allowed
PRIVATE -> MORE_RESTRICTED allowed
PRIVATE -> PUBLIC          deny unless independently re-grounded through an authorized target-local process
```

For the current private-method carrier:

```text
PRIVATE METHOD MAY INFORM SOLVING
PRIVATE SOURCE MUST NOT PROPAGATE
METHOD CARRIER != PROJECT EVIDENCE
AUTHORIZED READ != PUBLICATION PERMISSION
```

A response cannot launder:

- private source pointers;
- hidden identity detail;
- a false `independently_regrounded` claim;
- publication permission;
- a higher claim ceiling than target-local evidence earns.

## 5. Counterproposal

A disagreement or requested repair creates a **new handoff**, not a mutation of the prior response.

```text
REQUEST_1
-> RESPONSE_1
-> COUNTERPROPOSAL
-> REQUEST_2
-> RESPONSE_2
```

Thus chronology remains reconstructible.

## 6. Provenance

Every pair preserves:

```text
request identity
response identity
source repository/workstream
target repository/workstream
target decision
source/evidence references
privacy boundary
claim ceiling
unresolved remainder
way back
```

The source and target remain independently authoritative.

## 7. Local adapter rule

The central protocol does not imply every repository implements both network directions.

Each local adapter must declare:

```text
INBOUND_STATUS
OUTBOUND_STATUS
SUPPORTED_REQUEST_CLASSES
PRIVACY_BOUNDARY
LOCAL_AUTHORITY
VERIFICATION_EVIDENCE
UNRESOLVED
```

Examples of legitimate states:

```text
INBOUND = VERIFIED_BOUNDED
OUTBOUND = REFERENCE_ONLY

INBOUND = DESIGN_ONLY
OUTBOUND = GIT_HANDOFF_ONLY

INBOUND = NOT_IMPLEMENTED
OUTBOUND = NOT_IMPLEMENTED
```

## 8. Current concrete evidence

Conscience64 PR #170 establishes a bounded inbound private-method handoff across the loopback Knowledge Bridge:

- restricted `METHOD` carrier accepted;
- privacy-origin marker preserved through POST -> ledger -> authorized GET;
- unauthenticated GET, sync and search conceal it;
- visibility laundering rejected;
- source-pointer substitution rejected;
- false independent-regrounding rejected.

Conscience64 verifies bounded outward private-method privacy enforcement across ECS/client projection, agent/tool projection, nested export blocking, and structured publication hold at `deba0e9b19ef1fe106ba6dea1323eb0896dd5010`.

PR #176 / merge `ee740af81d95533c907a65166564e661807c50eb` adds the first actual target-local structured return path for this protocol:

```text
PRIVATE METHOD packet_uoid
-> POST /v1/handoff-response
-> target-local decision
-> deterministic restricted response_id
-> authorized GET /v1/handoff-response/<response_id>
```

The response is stored in a separate append-only/idempotent ledger, preserves `way_back = [request packet_uoid]`, rejects target impersonation and unknown requests, and cannot claim successor/evidence references before independent re-grounding.

`STRUCTURED_RETURN_PATH != UNIVERSAL_BIDIRECTIONAL_RUNTIME`

The runtime is verified only for the `PRIVATE_METHOD_HANDOFF` request class. A generic structured response runtime for arbitrary request classes remains unimplemented.

The Other-Projects RMAL cooperative carrier establishes implemented RMAL syntax for:

```text
reference
proposal
handoff
target_acceptance
target_local_successor
```

The response-side RMAL successor is also merged: Other-Projects PR #78 at `8f2d4e433d4b1bff3ec6eb0cc6a054b4c5bce284`. Its controlled-surface check passed in run `35505020623` at exact contributor head `cb3d9ef7523caa22a49bfddbaf7caf3dd776af92`. The local evidence, adapter, and pointer were reconciled by PR #82 at `ab562daf0b2df085b97d5d0cc1696a9b4cf505d2`.

This records an authored, structurally checked response carrier. It does **not** establish fresh RMALC check/compile/audit or a generic response-packet runtime. The original successful source/checker blobs and prior adapter declaration remain recoverable.

`CONTROLLED_SURFACE_CHECK != RMALC_COMPILE`
`AUTHORED_CARRIER != GENERIC_RESPONSE_RUNTIME`

### Landed local adapter declarations

- Conscience64 adapter: `research/bridges/libraries-of-libraries/BIDIRECTIONAL_HANDOFF_ADAPTER_2026-09-20.json`, current merge `731b6040d6b342cea423d402ec2fb6c1e5ad59a9`.
- Other-Projects adapter: `BIDIRECTIONAL_HANDOFF_ADAPTER_2026-09-20.json`, initial declaration `8de91a93347aaff1ff039a4899fd56eec6689da3`; current response-carrier reconciliation `ab562daf0b2df085b97d5d0cc1696a9b4cf505d2`.

These adapter records declare local capability state. They do not transfer implementation authority to the central protocol.

## 9. Current claim ceiling

This protocol establishes the coordination contract and machine-readable fields used by the central Libraries-of-Libraries surface.

It does not claim:

- universal distributed-consensus correctness;
- a production network transport between all repositories;
- perfect privacy;
- semantic detection of unmarked private material;
- automatic independent re-grounding;
- scientific truth from agreement.

## 10. Canonical short form

```text
A REQUESTS
-> B DECIDES LOCALLY
-> B RESPONDS
-> A PRESERVES RESPONSE
-> OPTIONAL NEW REQUEST

B REQUESTS
-> A DECIDES LOCALLY
-> A RESPONDS
-> B PRESERVES RESPONSE
-> OPTIONAL NEW REQUEST
```

Both ways.

Still separate.
