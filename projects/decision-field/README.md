# Decision Field v0.7 — AnyFunctor Mirror / RMAL Homeward

```text
actual RMAL VM -> explicitly bound AnyFunctor -> immutable Mirror FunctionObject
-> existing native invocation/receipt engine -> separate admission
-> checkpoint -> independent reader -> replay and exact Homeward
```

See [Mirror / RMAL Homeward](docs/MIRROR_RMAL_HOMEWARD.md),
[current state](CURRENT.md), [verification](evidence/v0_7/VERIFICATION.md), and the
[runnable RMAL fixture](examples/mirror_homeward.rmal).

## Complete source build

Requirements: a C23 compiler, C++23 compiler, CMake >= 3.25, Python 3 and OpenSSL >= 3.
From the complete source root, including the unchanged v0.3 dependency:

```sh
cmake -S . -B build/mirror -G Ninja -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ \
  -DRMAL_BUILD_DECISION_FIELD=ON -DDF_REQUIRE_FULL_NATIVE=ON
cmake --build build/mirror --parallel 2
ctest --test-dir build/mirror --output-on-failure
```

Git-only users recover the exact original archive with
`scripts/recover_predecessor.py`. The companion source bundle already contains it.
Without that dependency, only the four standalone component/bridge tests are
available; they are not the full native result. `DF_REQUIRE_FULL_NATIVE=ON` refuses
that fallback. Root C23/ABI CI and full native local tests are separate scopes.

This is one Mirror path, not all structural transforms, a universal AnyFunctor
implementation, or mathematical proof. No camera/network/publication permission
is added. Native file persistence is POSIX-only; callbacks and external result
objects are not serialized. The process-local VM trace is not an OS/VM checkpoint.

The earlier v0.6 source and documentation are recoverable at PR #2 merge
`141a60f58bdfe6d36774e0fba5e8350cb6ef5dcf`. Its evidence remains in `evidence/v0_6/`.
The original v0.3 engine and DFNAT001 archive schema remain unchanged.
