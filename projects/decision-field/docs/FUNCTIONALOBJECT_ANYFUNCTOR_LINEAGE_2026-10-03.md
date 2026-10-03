# FunctionalObject → AnyFunctor local-plane lineage

Date: 2026-10-03

This record preserves the historical source shape and implements only the smallest
defensible successor relation. It does **not** rewrite the old source, claim that the
old program already contained the current semantics, or treat a similar name as proof
of equivalence.

## Historical source anchor

Original repository: `BanalityOfSeeking/ServeExcel`

Original path: `FunctionalObject.cs`

Earliest commit currently visible for that path:

- commit `cd5bfe729d7461e9773ac4873767096c8e6146a0`
- date: 2019-06-10
- blob: `fc2716e1a3c4c2c7ea18541f25d94a26d24dca54`

The later 2021 `.NET 6` edit (`4331920050bfb1b1aa40eb95c125e2215def117a`)
changes construction syntax but preserves the same wiring topology.

The historical file remains in its original repository. This repository records its
identity and successor relation rather than copying it into the active implementation.

## Source-derived shape

`FunctionalObject<T>` contains:

- ordered `IOBuffer` and `DataBuffer`;
- zero-to-many `Operations` materialized as `TransformBlock<T,T>`;
- zero-to-many `Checks` materialized as `TransformBlock<T,T>`;
- alternating links between I/O, transforms and data;
- terminal `NullTarget<T>()` links for flow that is not otherwise consumed.

That supports a narrow historical statement: the old carrier already represented
plural transformations/checks and reusable dataflow wiring. It did not yet encode
modern obligation identity, provenance, contextual occurrence, admission authority,
Homeward recovery, or retained failure.

## Successor mapping

| Historical carrier | Local-plane successor |
| --- | --- |
| `IOBuffer` ingress | stage 0: Receive |
| current wiring context | stage 1: Contextualize |
| transform/check wiring | stage 2: Represent |
| linked output path | stage 3: Integrate |
| `Operations[]` | represented executable route IDs |
| `Checks[]` | plural local-plane obligations |
| repeated buffer traversal | repeated four-stage stair |
| `NullTarget<T>()` | retained `LocalPlaneResidual` |
| implicit reusable link | admitted local route index |

The stair is finite even though use can continue:

```text
Receive(0) -> Contextualize(1) -> Represent(2) -> Integrate(3) -> Receive(0')
```

`0'` is the same carrier role at a later plane revision, not a claim that the
machine has returned to an identical global state.

## Executable increment

`include/decision_field/local_plane.hpp` adds a bounded local routing index.

A plane:

1. requires **multiple** distinct obligations;
2. binds each observation to a local context signature and representation signature;
3. admits a new route only when the underlying execution was admitted and every
   local obligation is reported satisfied;
4. retains rejected, obligation-incomplete, and conflicting observations as residuals;
5. never overwrites an already integrated route with a conflicting route;
6. provides hash-index lookup for an already integrated local relation.

The lookup is average `O(1)` for the **routing index only**. It does not cache
outputs, skip the executor, erase occurrence identity, or establish an `O(1)`
bound for the underlying computation.

This is the intended shortening:

```text
first occurrence:
discover -> execute/verify -> satisfy obligations -> integrate route

later occurrence:
local route lookup -> execute/verify -> satisfy obligations
```

The learned structure shortens valid wiring; it does not replace processing.

## Why this is not a parallel AnyFunctor

The route ID is intended to be the identity of an admitted executable carrier such as
the current `FunctionObject<T>` / AnyFunctor Mirror path. `LocalPlane` does not
execute functions or mint authority. Execution, verification and admission stay with
the existing engine. The plane only remembers a locally valid route after admission.

That preserves the current boundary:

```text
DISCOVER != EXECUTE
EXECUTE != VERIFY
VERIFY != ADMIT
ADMIT != CACHE_OUTPUT
KNOWN_ROUTE != KNOWN_ANSWER
SHORTER_WIRING != LESS_OBLIGATION
```

## Regression probe

`tests/local_plane_tests.cpp` checks:

- one obligation is rejected because this carrier is explicitly plural;
- the four stair stages remain ordered;
- the first admitted relation changes the local-plane revision;
- the same relation reuses the existing route without growing the plane;
- a conflicting route is retained as residual and cannot overwrite the admitted route;
- incomplete obligation coverage is retained and cannot integrate;
- rejected execution is retained and cannot integrate;
- a genuinely new contextual relation can integrate as a new local route.

This is a local architectural successor probe, not a global complexity result or a
claim that `FunctionalObject.cs` was already AnyFunctor.
