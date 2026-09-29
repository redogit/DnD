# v0.7 implementation review

Review performed by the implementing assistant, not an independent reviewer.

- Kept the frozen v0.3 engine and checkpoint wire format unchanged. The generated
  successor adds only read-only invocation accessors; Mirror uses engine.execute.
- Kept computed candidates separate from admission. Rejected candidate bytes live
  in the versioned opaque recovery receipt, not the admitted state graph.
- Bound function identity to the full declared axes/policy/codec/obligation
  contract. Bound invocation identity to exact source occurrences and context.
- Verified exact reversal without providing original inputs to the inverse.
- Reproduced the C++ consumer failure at the C23-only header guard, then added
  explicit C linkage without bypassing the guard for C implementation files.
- Used one opt-in host symbol; rejected shadowing, reentry, unbound handles,
  wrong types/operations, foreign function/receipt pairs and rejected outputs.
- Strengthened the process witness by binding the exact RMAL source digest to
  archive identity and invocation context. A changed script is a negative probe.
- Preserved the distinction between full native local verification, root C23/ABI
  CI, and mere source-manifest matching. Missing predecessor source must not turn
  a four-test fallback into a reported native integration pass.
- Checked the public/private repository boundary: implementation/evidence remains
  in its private target repository; no private source is copied to public profile
  summaries or used to broaden publication authority.

Remaining boundaries: callbacks and policy IDs are host-trusted, not authenticated;
input validity and exact-codec availability are preconditions; nondeterminism
rejected before receipt creation is not a durable admitted invocation; native
checkpointing is quiescent engine-data persistence, not OS or VM capture; the
original process-local VM trace is not serialized; StateId/InvocationId are
run-local rather than a complete universal four-ID catalog. Meet4, Overlap,
cohort scheduling, external result ownership and Windows durable I/O are separate
implementation obligations. No broad theorem follows from this fixture.
