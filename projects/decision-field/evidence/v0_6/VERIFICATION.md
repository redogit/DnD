# v0.6 verification — native engine checkpoint and independent restart

Date: 2026-09-27. Git parent: `21e85bd293745b2ff2096fc1000c7a7058f3880b`.

## Fresh local results

| Configuration | CTest targets | Result |
|---|---:|---|
| GCC 14.2 Debug, warnings as errors | 13 | 13 passed |
| GCC 14.2 Release, warnings as errors | 13 | 13 passed |
| Clang 17 Debug, ASan + UBSan, warnings as errors | 13 | 13 passed |
| Standalone source, predecessor absent | 4 | 4 passed |

These are executable targets, not independent proofs. No sanitizer findings were reported.
Root RMAL/RMALC C23 was unchanged and its tests were not rerun. Windows and GitHub Actions
were not verified. Review was performed by the implementing assistant, not an independent agent.

## Process-separated witness

The writer builds the actual recovered v0.3 domain's nearest and farther triadic fields,
adds success/failure/sink/generator/multi-output transform invocations and research records,
then exits after saving a checkpoint. A second OS process reads only that file, trusted digest,
compiled codec and callback registry. It does not call the construction routine.

| Measured item | Value |
|---|---:|
| Native states | 30,376 |
| Transform edges | 5 |
| Invocation records | 5 |
| Archive bytes | 8,755,589 |
| Exact archive re-encoding after load | byte-identical |
| Memoized invocation reuse after restart | passed |
| New invocation/counter continuation | passed |
| Failed invocation and negative evidence retention | passed |
| Zero-output inverse receipt and repeated input order | passed |
| Refusal to overwrite existing checkpoint | passed |

Archive SHA-256:
`8ab9f2b15524ac08d3d8cdf8959e7da344a301cca9a35a51530f86868309670a`

Python hashlib independently checks the writer's OpenSSL digest. The exact native codec
covers SelfState stance, markers and orientation; NUL strings and signed-zero metadata
are separately exercised. Original predecessor source is unchanged and hash-bound.

## Negative tests and repairs

Missing-feature RED tests preceded each implementation. A cache-key counterexample made
edge, invocation and index agree on a forged key; restore initially accepted it. The regression
failed before repair and passed after keys were recomputed from ordered source/context data.

Tests reject wrong codec/policy versions, malformed lengths, partial/trailing archives,
checksum corruption, missing graph/dependency links, invalid counters/addresses, lossy codec,
missing/wrong executable registry, overwrite attempts, symlink reads and oversized files.

256 deterministic body mutations were also tested after recomputing their checksums:
239 rejected and 17 accepted as structurally valid changed records. These 17 are not admitted
truths; the result explicitly shows why an embedded checksum does not authenticate the author.

Clang strict compilation caught an implicit signed-byte conversion; an explicit conversion
fixed it. A long Debug build was interrupted by the command timeout and resumed; only the
subsequent completed runs are counted as passing. Earlier formatting warnings are preserved
in the logs and absent from the final strict builds.

## Reproduce

From a complete source bundle (or Git checkout after exact predecessor recovery):

```sh
cmake -S projects/decision-field -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS=-Werror
cmake --build build --parallel 2
ctest --test-dir build --output-on-failure
```

OpenSSL >= 3 development files and Python 3 are required for native targets.
For sanitizers use Clang with `-fsanitize=address,undefined -fno-omit-frame-pointer`
and run with `ASAN_OPTIONS=detect_leaks=1 UBSAN_OPTIONS=halt_on_error=1`.

## Claim ceiling

Native engine-owned data is reconstructed; callback code, OS/thread state and externally owned
result objects are excluded. Quiescent capture is required. POSIX create-only publication uses
fsync but was not tested by cutting power. Windows file I/O is unimplemented. Checksums and
graph validation do not authenticate or admit evidence. Resource limits are guards, not exact
RAM/performance bounds. Arbitrary codecs remain the caller's explicit, tested responsibility.
