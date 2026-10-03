# Synthetic receipt ingress verification — 2026-09-30

Predecessor revision: `37c2653acb336bd06114f8ab5b176260c7a73b9c`, PR #7.
Master remains `2f9fbe3b1964194d8f7b525c6fc90a22b3ff7df9`.
This is a bounded repair to standalone receipt input, not another evaluator.

## Observed failures and repair

| Counterprobe | Before | After |
|---|---|---|
| `blobs` replaced by null, list, integer or string | Uncaught `AttributeError` | API `ValueError`; CLI `REJECTED` |
| Oversized source/report files | Entire files read before rejection | Application receives at most each limit plus one overflow byte |
| 10,000 nested arrays, 20,001 input bytes | Uncaught JSON parser `RecursionError` | Explicit CLI rejection |

`RED_TESTS.txt` retains all seven failing assertions from the initial three
tests. `INITIAL_BOUNDARY_TESTS.txt` records those three passing after repair.
`NESTING_2000_CONTROL.txt` retains a smaller nesting input that already produced
structured rejection on this Python build. `RED_NESTING_TEST.txt` records the
larger parser counterprobe failing before its repair. The four new tests are
included in the full current 28-test suite.

`COUNTERPROBES.json` preserves eight observed inputs as exact reconstruction
recipes and hashes, with their rejection results. Blob replacements reference
the exact earlier process receipt. Repeated-byte and bracket recipes specify
the full input bytes, including absence of a final newline for nesting probes.
These are ingress failures before proposal execution; their evidence is retained
here rather than manufactured as proposal history events.

The source budget is 65,536 bytes; the receipt budget is 2,097,152 bytes.
Application read counts for oversized files were 65,537 and 2,097,153 bytes.
Buffered file I/O may prefetch beyond those returned counts. An independent
review flagged this distinction, and the retained buffered-read control records
one local raw-file-position observation. No exact underlying OS read cap,
total-memory bound, timing bound or timeout for nonregular files is claimed.
The in-memory API still receives caller-created objects and validates their
canonical receipt envelope; its prior allocations are outside this file-read
observation.

## Fresh checks

| Check | Result | Evidence |
|---|---|---|
| Full synthetic suite, Python 3.12.14 | 28/28 | `TESTS.txt` |
| Same suite with optimized Python | 28/28 | `OPTIMIZED_TESTS.txt` |
| Root Python regressions | 3/3 | `ROOT_PYTHON_TESTS.txt` |
| v0.7 source identity check | All 14 match | `V0_7_SOURCE_IDENTITY.txt` |
| Fresh finite panel generation and independent-process replay | Byte-identical to predecessor receipt | `RECEIPT_REPLAY.txt` |
| Independent review | No critical or important findings | `REVIEW.md` |

Fresh generation retains 24 orders, 48 admissions, 48 rejections and 96 exact
source reconstructions. Its 366,026-byte receipt remains SHA-256
`9e8565d2fef18eb0f3098898c34b0242d1a3316e1c7684378bf607c30cbbdce7`.
The source remains SHA-256
`b9e3498d5b3f3487e8c7f1954b082ddae7647ce8b0ba370511c111e63dd790e0`.
No old evidence files are replaced. The earlier synthetic source manifest binds
its predecessor revision; this directory's manifest binds the repaired source
and new evidence bytes. Manifest identity is not a runtime test or authentication.

Commands from the repository root:

```sh
python3 -m unittest discover -s projects/decision-field/experiments/structural -p 'test_*.py' -v
python3 -O -m unittest discover -s projects/decision-field/experiments/structural -p 'test_*.py' -v
python3 -m unittest discover -s tests -p 'test_*.py' -v
python3 projects/decision-field/scripts/check_mirror_manifest.py
python3 projects/decision-field/experiments/structural/probe.py --output /tmp/unused-structural-receipt.json
python3 projects/decision-field/experiments/structural/probe.py --verify /tmp/unused-structural-receipt.json
python3 projects/decision-field/experiments/structural/probe.py --verify projects/decision-field/evidence/structural_probe_2026_09_30/PROCESS_RECEIPT.json
```

Use an unused output path. CI observations must be identified by their own
revision and remain separate from these local checks. The full native Mirror
suite was not rerun in this follow-up; its recovery limitation remains in the
predecessor evidence. K, H/A, proposal/check/admission behavior and the frozen
v0.7 source are unchanged. No runtime connection, behavioral equivalence,
universal evaluator, Pareto planner or P-versus-NP result is added.
