# Decision Field

Obligation-relative exploration, directional component analysis and recoverable core replay.

The root `redogit/DnD` RMAL/RMALC C23 toolchain remains unchanged. This project is an isolated C++23 research implementation.

## Current path

```text
192 baseline records
-> explicit projection and metric policy
-> component analysis
-> shared coordinates + exact row-specific residual values
-> decode without original inputs
-> replay frozen obligations
```

A triadic `SurfaceDefinition` can also become a scoped `SurfaceObject`. Its 16 representatives index the observations; they do not replace the complete branch history.

Read `CURRENT.md`, `docs/RECOVERABLE_CORE_REPLAY.md`, and `evidence/v0_5/VERIFICATION.md` for the exact implemented scope.

## Build the standalone Git slice

```sh
cmake -S projects/decision-field -B build/decision-field -DCMAKE_BUILD_TYPE=Release
cmake --build build/decision-field --parallel 2
ctest --test-dir build/decision-field -C Release --output-on-failure
```

This runs four test targets when the optional predecessor source is absent. The configuration prints that boundary explicitly.

## Reproduce the actual v0.3 integration

The original conversation archive is `decision_field_prototype_v0_3_triadic.zip`:

```text
SHA-256 91a25de44269e6f54d45c5744b38af09eef2daf25b8fedbdab79ca0b4a018292
```

Recover it without replacing another version:

```sh
python projects/decision-field/scripts/recover_predecessor.py /path/to/decision_field_prototype_v0_3_triadic.zip
```

Then reconfigure/build/test using the commands above. CMake discovers `predecessor/v0_3/`, adds its three original tests, and adds two actual-engine integration tests: nine targets in total.

An existing verified source directory can instead be supplied with:

```sh
cmake -S projects/decision-field -B build/decision-field -DDF_PREDECESSOR_SOURCE=/path/to/decision_field_v0_3_package
```

The companion v0.5 source bundle includes the recovered predecessor. The repository does not pretend that the earlier source-hash manifest alone contains those bytes.

## Sources and instructions

- `instructions/OPERATING_PROTOCOL.md`: accepted user-directed design rules.
- `evidence/CONVERSATION_STRATEGIC_INPUTS.md`: original strategic inputs.
- `docs/DEFINITIONS.md` and `docs/SEMANTIC_CROSS_REFERENCE.md`: earlier definitions and meaning-based relations.
- `docs/RECOVERABLE_CORE_REPLAY.md`: v0.5 contract; supersedes earlier representative-only or selected-member-only core descriptions.
- `provenance/SOURCE_MANIFEST.md`: preserved predecessor claims/hashes.
- `evidence/v0_5/`: fresh tests, negative results, recovery provenance and review.

## Evidence boundaries

```text
RANKING != PRESERVATION_PERMISSION
NUMERIC_PROJECTION_RECOVERY != NATIVE_OBJECT_RECOVERY
KEEP_REPLAY != EVERY_INSTANCE_SUCCEEDS
SAMPLED_SURFACE != GLOBAL_MANIFOLD_PROOF
GENERATE != VERIFY != ADMIT
```
