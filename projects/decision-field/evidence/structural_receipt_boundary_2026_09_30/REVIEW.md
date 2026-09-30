# Independent ingress review

Read-only reviewer inspected the repair against predecessor
`37c2653acb336bd06114f8ab5b176260c7a73b9c` and ran the 28-test synthetic suite.
Assessment: no critical or important findings; ready to proceed.

The reviewer reported 3,156 schema/value mutations producing only `ValueError`
rejections or unchanged-value no-ops, and unchanged source/receipt identities.
No raw mutation ledger was supplied, so that count is reviewer-reported and is
not represented as an independently retained mutation experiment. The directly
retained tests and counterprobe receipt provide this follow-up's rerun evidence.

One minor finding: buffered reads measure returned application bytes, not exact
filesystem bytes fetched. Documentation and the counterprobe measurement field
now state that narrower claim. The root agent independently recorded a local
buffered-read control. This distinction does not justify a total-memory bound.

Nonregular-file timeouts, arbitrary custom Python objects, runtime integration,
authenticated custody and crash durability remain outside this repair.
