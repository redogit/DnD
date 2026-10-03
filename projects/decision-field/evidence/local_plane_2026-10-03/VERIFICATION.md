# Local-plane successor verification — 2026-10-03

## Scope

This receipt verifies the additive `FunctionalObject.cs` → AnyFunctor local-plane
successor without modifying the hash-pinned v0.7 Mirror witness.

Tested source revision:

```text
5b05b2db94c0ac23cdc67c9be338449381f86271
```

Exact source was taken from GitHub Actions artifact `RMAL-native-source`
(run `37097272275`, artifact `11264697291`).

```text
artifact ZIP sha256:
bcb0afb37f39fc16d1e7075277e06a520cb55cf6ab34c149fa0772f15ae0c3b8

contained rmal-native-source.zip sha256:
be9611e35baa23ea8e5427000940f600f7e2764a5c937e1447753a0e1c8842ec
```

## Exact predecessor

The predecessor was materialized from the preserved ChatGPT Library object
`decision_field_prototype_v0_3_triadic.zip`, not reconstructed.

```text
sha256:
91a25de44269e6f54d45c5744b38af09eef2daf25b8fedbdab79ca0b4a018292
bytes:
40414
```

That hash is the exact value required by
`projects/decision-field/scripts/recover_predecessor.py`.

## Native build

The exact predecessor archive was recovered into the tested branch source and the
successor was configured with:

```text
DF_LOCAL_PLANE_WITH_NATIVE=ON
DF_PREDECESSOR_SOURCE=<exact recovered v0.3 source>
CMAKE_BUILD_TYPE=Release
```

Observed toolchain:

```text
GNU C++ 14.2.0
OpenSSL 3.5.6
C++23
```

The build compiled and linked both new successor executables:

```text
decision_field_local_plane_successor_tests
decision_field_anyfunctor_local_plane_successor_tests
```

The inherited full v0.7 local build was still compiling when the container command
reached its 120-second execution limit. This is not recorded as a full local-suite
pass. The repository's separate GitHub Actions RMAL C23 Toolchain completed green on
Linux and Windows for the tested source revision, including the preserved 14-file
Mirror source-identity check.

## Direct executable receipts

Exact native seam:

```text
PASS AnyFunctor local plane: admitted route integration, reuse without occurrence collapse, and rejected residual retention
```

Standalone local plane:

```text
PASS local plane: four-stage stair, plural obligations, admitted route integration, average-O(1) reuse lookup, and retained residuals without output caching
```

## What these receipts establish

- the finite four-stage carrier executes as a bounded local-plane successor;
- the plane refuses a singular-obligation construction;
- admitted relations can become reusable local routes;
- equal-valued distinct source occurrences reuse routing without sharing invocation identity;
- rejected AnyFunctor results do not become integrated routes;
- failure is retained as residual state rather than discarded;
- local route lookup is hash-indexed and intended as average `O(1)` lookup.

## Claim ceiling

```text
AVERAGE_O(1)_ROUTE_LOOKUP != O(1)_UNDERLYING_COMPUTATION
ROUTE_REUSE != OUTPUT_REUSE
ROUTE_REUSE != OCCURRENCE_COLLAPSE
ADMISSION_RECEIPT != UNIVERSAL_TRUTH
LOCAL_NATIVE_PROBE != UNIVERSAL_COMPLEXITY_RESULT
TIMEOUT_OF_INHERITED_BUILD != TEST_FAILURE
```
