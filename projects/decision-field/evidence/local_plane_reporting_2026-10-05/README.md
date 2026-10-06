# LocalPlane reporting-boundary integration

This is the reviewed reporting/test/evidence update, not a new recurrent controller.
The user's specified visits remain `0_a -> 1_b -> 0_c -> 2_d`, followed by directed
`1_b -> 2_d` wiring and then segment advancement. No proper-wiring predicate,
coverage flag, extra control state, or substitute model is invented.

## Frozen inputs and provenance

- Merge base inspected: `01dfbdecf23404b26e57d0abca5d3a67b67cff22` (includes ServeExcel PR #11).
- Historical audit base: `0d9945879111388d5213af685f3c9a63b4d6bc92`.
- Input archive: `THREE_STATE_DIFFERENTIALS_LATERALS_2026-10-05.zip`, SHA-256
  `7279494ec4c03b3bd716f4e1426ff76985680921a8d373c5187d1eff65d1954d`.
- All 47 manifest entries verified before replay. Original archive and logs were not rewritten.
- The three patch targets were reread through GitHub at the merge base. Their blob
  identities matched the frozen inputs; the intervening changes only added ServeExcel.
- The patch is exactly the supplied `REPORTING_BOUNDARY_REPAIR.patch`: header comments,
  two standalone assertions, test output correction, and an additive design clarification.

`DIFFERENTIALS.json` and `LATERALS.json` are exact historical audit records. Their
statuses refer to that audit, not this merge. Their `frozen/...` evidence references
are archive-member locations in the identified input packet, not paths silently
claimed to exist in this repository. Source occurrences, candidate methods, successor
pointers and implemented seams remain distinct. None of the eight edges transfers
identity, evidence or authority. The packet's model/source recovery remains incomplete.

## Fresh local checks

The supplied runner was rerun in a separate copy. GCC compiled the frozen and patched
standalone tests and eight-case diagnostic; Clang compiled the patched standalone
and frozen diagnostic. Probe outputs agreed byte-for-byte. Two reporting checks
failed against the old success message, and all six passed against the patch.

The existing, byte-verified successor CMake file configured and built the staged patch
as C++23: CTest passed 1/1. The audit probe was then freshly compiled against the staged
canonical header; its output matched the retained native record and all six checks
passed against the CMake-built test. Preprocessed production-header output was identical
before and after the comments-only edit. Exact receipts and RED/GREEN logs are retained
in `VERIFICATION.json`.

Direct clone failed DNS resolution. This was a hash-verified partial-source build,
not a complete local clone or full native suite. The native AnyFunctor seam, inherited
Mirror runtime, Windows runtime and unrelated projects were not executed locally.
Hosted CI is checked separately on the PR, not preclaimed by this receipt.

## Known failure remains open

The empty satisfied-obligation ID case still allocates occurrence 6 and throws without
appending a native residual; the next valid call returns 7. The outer diagnostic retains
that attempt. Passing its characterization test does not repair the native journal.
The original three-state code, proper-wiring rule and segment controller remain unresolved.

No routing tokens, original witnesses, CMake files, workflows, historical receipts,
AnyFunctor admission rules or canonical WordDNA sources were changed by this update.
R3, BrainWorld and SelectiveSSMMixer remain separately scoped lateral candidates, not
substitute identifications. `GENERATE != VERIFY != ADMIT` remains in force.

## Reproduce the six checks without overwriting historical evidence

Run from the repository root on a host with Python, GCC and C++20 support:

```sh
set -eu
repo="$PWD"
audit="$repo/projects/decision-field/evidence/local_plane_reporting_2026-10-05"
work=$(mktemp -d)
mkdir -p "$work/tests" "$work/evidence"
cp "$audit/tests/test_reporting_boundary.py" "$work/tests/"
g++ -std=c++20 -O2 -Wall -Wextra -Werror -pedantic \
  -I "$repo/projects/decision-field/include" \
  "$repo/projects/decision-field/tests/local_plane_tests.cpp" -o "$work/local_plane_tests"
g++ -std=c++20 -O2 -Wall -Wextra -Werror -pedantic \
  -I "$repo/projects/decision-field/include" \
  "$audit/src/differential_probe.cpp" -o "$work/probe"
"$work/probe" > "$work/evidence/native_probe.json"
cmp "$work/evidence/native_probe.json" "$audit/evidence/native_probe.json"
LOCAL_PLANE_TEST_BINARY="$work/local_plane_tests" \
  python -m unittest discover -s "$work/tests" -v
printf 'Fresh evidence retained at %s\n' "$work"
```

The existing cross-platform standalone gate remains:

```sh
cmake -S projects/decision-field/successor/local-plane -B build/local-plane -DCMAKE_BUILD_TYPE=Release
cmake --build build/local-plane --config Release
ctest --test-dir build/local-plane -C Release --output-on-failure
```
