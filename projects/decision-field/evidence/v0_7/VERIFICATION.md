# v0.7 verification — 2026-09-29

Scope: one concrete native Mirror / real RMAL VM / checkpoint / Homeward path.
The prior PR #2 implementation was already merged and was not rebuilt as a new
project. PR #3's native callback work was retained.

## Exact prerequisites

- v0.6 companion archive SHA-256: `662e2ad2ca25ae526e0e376e4f59940fad1d089e0937a48af663344250b4c3da`.
- Original v0.3 archive SHA-256: `91a25de44269e6f54d45c5744b38af09eef2daf25b8fedbdab79ca0b4a018292`.
- Original engine header SHA-256: `279c4e6cf1ac8b9f9896625bd761d9aa3ed9f89e865c8b47c0061fa28030a34b`.
- Existing PR #3 native callback source head: `51b1e13835988a550ebcb4f30695de1041145493`.
- Exact-head workflow/source-retention successor: `e9def257e610aaee7c89765e96f81105092c929b`.

The local source was recovered from the hash-verified companion and the private
Actions compiler-source artifact. New source/checker blob identities are listed
in `SOURCE_MANIFEST.json`. Matching those identities is not a runtime test.

## Fresh local results

| Scope | Result |
|---|---|
| Original native v0.6 baseline, GCC 14 Release, -Werror | 13/13 |
| Complete root + native + Mirror + RMAL integration, Release | 25/25 |
| Complete integration, Debug with ASan + UBSan + leak detection | 25/25; no sanitizer findings |
| C++ consumer of the C23 ABI | observed RED at C-only header guard, then GREEN |
| Mirror first-class path | observed missing-path RED, then GREEN |
| Real RMAL Mirror host | observed missing-host RED, then GREEN |
| Source manifest | 14 identities match; altered-source counterprobe rejected |

The local Clang binary reports 17.0.0 but accepts `-std=c23` and reports
`__STDC_VERSION__=202311L`; raw environment diagnostics are retained in the bundle.
No compiler-version label alone is used as proof of conformance. The C23 guard is
not overridden. Root Linux/Windows Actions results remain a separate CI scope.

Commands, from the complete source root:

```sh
cmake -S . -B build/release -G Ninja -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ \
  -DCMAKE_CXX_FLAGS=-Werror -DRMAL_BUILD_DECISION_FIELD=ON \
  -DDF_REQUIRE_FULL_NATIVE=ON
cmake --build build/release --parallel 2
ctest --test-dir build/release --output-on-failure

cmake -S . -B build/sanitize -G Ninja -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ \
  '-DCMAKE_C_FLAGS=-fsanitize=address,undefined -fno-omit-frame-pointer' \
  '-DCMAKE_CXX_FLAGS=-Werror -fsanitize=address,undefined -fno-omit-frame-pointer' \
  '-DCMAKE_EXE_LINKER_FLAGS=-fsanitize=address,undefined' \
  -DRMAL_BUILD_DECISION_FIELD=ON -DDF_REQUIRE_FULL_NATIVE=ON
cmake --build build/sanitize --parallel 2
ASAN_OPTIONS=detect_leaks=1:halt_on_error=1 UBSAN_OPTIONS=halt_on_error=1 \
  ctest --test-dir build/sanitize --output-on-failure
```

## Independent restart witness

The writer and independent reader execute the actual `mirror_homeward.rmal`
program through the C23 parser, bytecode and VM, with explicit native bindings.

- Native states: 3; transform edges: 3; invocations: 3.
- Rejected candidates retained exactly: 1, with no admitted output state.
- Native archive bytes: **6,487**.
- Archive SHA-256: `faa15ce950f07ae2f78b9d9e04e6f71dbb5ed1755341d2ba3aec66028078a9fa`.
- Homeward native value SHA-256: `6974ac65a18a5c6133109056f72fd178bd7c6cf92f3193fa6a166c24d4b6e567`.
- Re-encoding before/after replay is byte-identical.
- Overwrite, wrong trusted digest, truncation, altered archive bytes and changed
  RMAL source are rejected.

This is a small fixture, not a universal size, complexity or memory-optimality
claim. The native Mirror unit process measured 4,608 KiB peak RSS in one local
`/usr/bin/time -v` run; that excludes compilation and is not a platform bound.

The v0.6 large regression remains byte-identical: 30,376 states, 5 edges,
5 invocations, 8,755,589 bytes, SHA-256
`8ab9f2b15524ac08d3d8cdf8959e7da344a301cca9a35a51530f86868309670a`.

## Scope of CI and recovery

Root CI binds the exact PR head and tests the C23 VM plus C++ ABI on Linux and
Windows. It also checks source-manifest identity and retains a scoped private
source artifact. Those checks do not pretend to be the full native integration
suite when the original v0.3 source is absent. Full native results above are local
exact-source runs; the companion bundle supplies the original dependency and all
logs. `DF_REQUIRE_FULL_NATIVE=ON` refuses the standalone fallback.

No native Windows durable-I/O result, OS/VM snapshot, generic domain integration,
authentication, public deployment, or independent code-review claim is made.
