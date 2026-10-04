# Frozen-source checkout follow-up

Predecessor: `16ce3c64e68c28899d8e5634e9d73f4c93c3f461` (the initial bounded
AdapterEvaluator increment). The original source/evidence manifest in this
directory describes that revision and remains unchanged. The follow-up manifest
records the later checkout repair separately.

The initial [synthetic CI run](https://github.com/redogit/DnD/actions/runs/36795545161)
passed on Linux. Windows passed both 39-test suites and fresh/saved replay for both
receipt panels, then failed the newly added v0.7 source-manifest check on
`CMakeLists.txt`. `WINDOWS_CI_FAILURE.json` retains the relevant returned log tail,
with run/job IDs and timestamps. The existing root C23 workflow at the same
commit passed; that is separate from the full native Mirror runtime suite.

Cause: Git's Windows CRLF checkout conversion affected the frozen native witness
files, which were outside the earlier synthetic-only LF rules. This was a
checkout-byte mismatch, not a modified committed native source or failed adapter
admission/replay. `CHECKOUT_RED.json` records a controlled `git -c core.autocrlf=true
checkout-index --all --prefix=<temporary-directory>/` checkout: 0/14 frozen file
hashes matched. With only `core.autocrlf=false`, 14/14 matched.

Repair: add `text eol=lf` only for the exact 14 source paths enumerated in the
existing frozen `evidence/v0_7/SOURCE_MANIFEST.json`. No frozen source, native
manifest, evaluator, checker, K measure, receipt or test is modified. The checker
continues to reject any byte mismatch. `CHECKOUT_GREEN.json` records 14/14 matches
under both Git settings after the repair, plus successful independent replay of
the saved adapter receipt from the simulated CRLF checkout.

The native source identities remain byte-preserved. This does not establish native
Windows runtime equivalence or durability. The normal/optimized synthetic suite
and both receipt panels remain the scope of the hosted synthetic workflow.

Independent read-only follow-up review found no issues. It confirmed that the
anchored LF rules cover exactly the 14 manifest paths, that all 14 files retain
the published bytes/hashes, and that Git resolves each path to LF. Hosted Windows
confirmation remains a separate observation after publication of this repair.
