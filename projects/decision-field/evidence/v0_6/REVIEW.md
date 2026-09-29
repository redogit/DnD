# v0.6 review and remaining boundaries

Review by the implementing assistant. No independent reviewer is claimed.

## Review-discovered material defect

Native restore initially checked edge/invocation/cache agreement but did not recompute the key.
The smallest counterprobe rewrote all three to `forged-key`, preserving internal agreement.
The test expected rejection and failed. The repaired validation reconstructs the key from
transform ID/version, configuration/entropy fingerprints and ordered native input fingerprints.
The same counterprobe now passes by rejecting the invalid checkpoint. Historical RED evidence
is preserved; no existing source/evidence was rewritten.

## Required distinctions checked

Success and failure invocations have different native adjacency behavior in v0.3. Failed
invocations remain in edges/invocations/cache without fabricated success-style output links.
Repeated input bindings keep order/multiplicity. Sink invocations keep a full recovery receipt
although their output vector is empty. Generators keep declared context. Stale states are
retained, and dependency invalidation after restore changes flags without removing history.
Full native marker/orientation fields are present in addition to the numeric stance projection.

## Remaining review risks

- Quiescent copy, not a concurrent snapshot or distributed transaction.
- Concrete exact codec demonstrated for SelfState; arbitrary domain codecs need separate tests.
- Legacy callback/ID hash behavior is inherited; new archive SHA-256 does not fix all upstream
  semantic collision or callback-purity concerns.
- An archive with a recomputed checksum can encode a structurally valid false narrative.
  Trusted out-of-band digest/authentication and evidence admission are separate requirements.
- POSIX writer assumes a trusted parent directory and supporting filesystem. It does not defend
  against an attacker replacing path ancestors, and no power-loss hardware test was performed.
- File helper rejects Windows operation explicitly. POSIX success is not Windows validation.
- File read allocates bounded input bytes, but native codecs can have their own resource costs.
- Externally owned field/surface result objects and compiled behavior are not engine-owned state.
- The bounded mutation test is not exhaustive fuzzing or a security certification.

## Outcome

The new tests and all prior in-bundle tests pass in the reported local configurations.
Keep the implementation as a scoped PR successor; do not promote these tests to universal
correctness, independent review, complete domain exploration or scientific proof.
