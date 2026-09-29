# First-class Mirror / real RMAL Homeward bridge — v0.7

This is a bounded executable continuation of v0.6, not another engine. The frozen
v0.3 source and the DFNAT001 checkpoint wire format are unchanged.

## One path

```text
RMAL source -> actual C23 parser/bytecode/VM -> opt-in host AnyFunctor callback
-> immutable FunctionObject<NativeValue> -> existing engine.execute
-> computation receipt -> obligation/involution verification -> admission
-> native checkpoint -> distinct reader process -> explicit callback rebinding
-> replay -> exact Homeward native value
```

`FunctionObject<T>` owns a const contract: policy identity, complete declared axes,
obligations and codec identity. Its content identity includes those declarations.
`AnyInvocation<T>` binds ordered native source occurrences, their exact encoded
values, context and execution space. Equal values from different source states
are not treated as the same invocation.

The wrapper uses the existing transform registry, invocation cache, edges and
checkpoint. It does not maintain a parallel execution database. Three read-only
accessors are injected by the hash-bound successor generator; the original engine
file is not edited.

## Generation is not admission

The policy computes a mirror candidate. Source/candidate validation and exact
involution checking determine admission separately. A rejected candidate has no
admitted output StateId. Its exact native bytes, violations, notes, function
identity and input digest remain in a versioned opaque recovery receipt on the
existing invocation edge. Replay and checkpoint recovery retain that rejection.

A successful inverse receives only outputs, context and receipt—not the original
input. It reconstructs the native value and checks its exact codec digest against
the receipt. Receipt queries describe recorded history, not a new live admission.
A stale source cannot start a new Mirror invocation.

## Executable RMAL surface

The host explicitly binds immutable positive function handles before execution.
No host functions exist in a bare VM and scripts cannot register or shadow them.
The single bound symbol has three arguments:

```rmal
let receipt = AnyFunctor("mirror", 1, 1);
assert AnyFunctor("status", 1, receipt) == "SUCCEEDED";
let output_state = AnyFunctor("output", 1, receipt);
let recovered_digest = AnyFunctor("homeward", 1, receipt);
```

The third argument is a source StateId for `mirror` and an InvocationId for
receipt queries. Handles are local to the explicitly declared engine/run, not
universal cross-project identities. `homeward` gives the full reconstructed value
to the host's bounded `last_recovered()` buffer and its SHA-256 to RMAL. A digest
is not a substitute for that value. Rejected outputs cannot be requested as
admitted output or as an admitted inverse.

The host must outlive its VM; the engine and host are quiescent/single-threaded.
Host callbacks are trusted compiled capabilities, not authenticated by their
names or hashes. Change the declared policy identity when its implementation
changes. No dynamic loading, network capability or script-controlled binding is
introduced. Invalid argument types, foreign receipt/function pairs, unsupported
operations, unbound functions, shadowing and VM reentry are rejected.

## Build and restart

Recover the exact original v0.3 archive through `scripts/recover_predecessor.py`
when using Git alone. The complete companion source bundle already contains those
unchanged bytes. Full integration is explicit and cannot silently fall back:

```sh
cmake -S . -B build/mirror -G Ninja \
  -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ \
  -DCMAKE_BUILD_TYPE=Release -DRMAL_BUILD_DECISION_FIELD=ON \
  -DDF_REQUIRE_FULL_NATIVE=ON
cmake --build build/mirror --parallel 2
ctest --test-dir build/mirror --output-on-failure
```

The root compiler implementation remains C23. C++ hosts consume its C linkage;
this does not turn the compiler into C++. The integrated POSIX process witness
executes `examples/mirror_homeward.rmal`, publishes a create-only checkpoint,
then starts another process. That reader receives only checkpoint/digest, the
same explicit RMAL source, and compiled policy/codec/registry. Archive identity
and invocation context bind the exact RMAL source digest. Re-encoding before
and after replay must be byte-identical. A changed script, wrong digest,
truncation, altered bytes and overwrite attempts are rejected.

## Evidence ceiling

This implements one Mirror path for the concrete native SelfState codec. It is
not universal AnyFunctor representation, full four-ID semantic cataloging,
first-class Meet4/Overlap, live cohort scheduling, concurrent process capture,
Windows durable checkpoint I/O, cross-device ABI portability, authentication,
publication authority, or mathematical proof. The existing quartet API remains
an unchanged predecessor path; it is not silently described as using this wrapper.
Externally owned result objects and compiled callbacks are not serialized.

Pure/replay-stable callbacks and exact codecs remain required preconditions.
A nondeterministic callback rejected by the underlying engine before receipt
creation is not claimed as a durable admitted invocation. Codec failure cannot
promise faithful recovery of bytes that the codec could not produce.

```text
GENERATE != VERIFY != ADMIT
IMMUTABLE CONTRACT != AUTHENTICATED CALLBACK
RESTORED CHECKPOINT != PROVEN CLAIM
DIGEST != RECOVERED NATIVE VALUE
ONE MIRROR PATH != EVERY STRUCTURAL TRANSFORM
```
