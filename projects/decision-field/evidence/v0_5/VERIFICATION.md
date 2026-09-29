# v0.5 verification — recoverable component/core/surface bridge

Date: 2026-09-27. Source base: `redogit/DnD@4282676da043cca93b1622532d94ae419d9795ab`.

## Fresh local results

| Configuration | CTest targets | Result |
|---|---:|---|
| GCC 14.2 Debug, actual recovered v0.3 engine | 9 | 9 passed |
| GCC 14.2 Release, `-Werror` | 9 | 9 passed |
| Clang 17 Debug, AddressSanitizer + UndefinedBehaviorSanitizer, `-Werror` | 9 | 9 passed |
| Standalone source, predecessor explicitly unavailable | 4 | 4 passed |

These are CTest executable targets, not a claim of 31 independent proofs. The sanitizer run produced no sanitizer findings. The first sanitizer compilation command reached the tool's execution time limit; compilation was resumed and the complete suite then ran successfully. Root RMAL/RMALC C23 files were unchanged and its suite was not rerun. Windows and GitHub Actions were not verified by this local run.

## Reproduction

From the repository root, recover the original archive using `scripts/recover_predecessor.py`, or use the complete companion source bundle with its included `predecessor/v0_3` directory.

```sh
cmake -S projects/decision-field -B build/df-debug -DCMAKE_BUILD_TYPE=Debug
cmake --build build/df-debug --parallel 2
ctest --test-dir build/df-debug --output-on-failure

cmake -S projects/decision-field -B build/df-release -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS=-Werror
cmake --build build/df-release --parallel 2
ctest --test-dir build/df-release --output-on-failure

cmake -S projects/decision-field -B build/df-sanitize -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_BUILD_TYPE=Debug '-DCMAKE_CXX_FLAGS=-Werror -fsanitize=address,undefined -fno-omit-frame-pointer' '-DCMAKE_EXE_LINKER_FLAGS=-fsanitize=address,undefined'
cmake --build build/df-sanitize --parallel 2
ASAN_OPTIONS=detect_leaks=1 UBSAN_OPTIONS=halt_on_error=1 ctest --test-dir build/df-sanitize --output-on-failure
```

## Executed source integration

The original v0.3 archive SHA-256 was checked against the original conversation value, then extracted without altering its source. The bridge tests instantiate that engine, not a replacement fixture.

```text
actual baseline observations                 192
exact numeric projections replayed          192
declared common mandatory coordinates         6
variable coordinates                         10
source numeric coordinate entries          3072
packet shared/override numeric entries     1926
nearest surface records retained            192
farther surface records retained          30144
surface representatives                      16
```

The entry count excludes metadata, per-row indices, source objects and lineage storage. It is not a byte-compression or memory benchmark. The six common coordinates were predeclared by the self-domain; they are not newly proved universal invariants. Exact reconstruction applies to the explicit numeric projection, not all native Object fields.

The synthetic positive/negative fixture separately preserved 96 negative oracle outcomes among 192 rows. A KEEP result certifies finite-scope preservation even when the preserved input is a counterexample.

## Negative checks and repairs

Preserved RED logs document the zero-quality observation omission, non-finite input acceptance, missing bridge/surface implementation, surface argument move-order error, inconsistent recovery summary and overflow-prone mean. Their corresponding tests now pass. Additional rejection cases cover partial/duplicate/misaddressed probes, stale state, missing parent, unstable projection, missing residuals, changed scope, malformed or unknown oracle output and incomplete surface coverage.

## Review and admission limits

Review was performed by the implementing assistant; no independent reviewer was available. This establishes executable behavior for the checked cases, not universal equivalence, complete domain exploration, or a solved external research problem. Missing native serialization, first-class transform unification, live cohort scheduling and RMAL execution remain explicit in CURRENT.md.
