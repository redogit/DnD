# Code cross-correlation — 2026-10-03

Target repository: `redogit/DnD`
Target baseline: `9f1496484051ed7349194d36f464cb2d20eef2a7`

This record compares the current Git line with the strongest recoverable Library and
closed-PR code lineages. Promotion requires exact source identity plus a fresh
counterprobe/build/test against the current baseline. Historical evidence alone is not
sufficient.

## Exact source anchors evaluated

| Carrier | Exact identity | Result |
|---|---|---|
| Current PR #9 source artifact | SHA-256 `bcb0afb37f39fc16d1e7075277e06a520cb55cf6ab34c149fa0772f15ae0c3b8` | current native/local-plane baseline |
| Closed PR #7 source artifact | head `63db668cd9fa695c09f2ec9289015c628520f698`, artifact SHA-256 `55f1f70e5055b8f03f9bc13293b949518133ead791d0456d2c0210f5daf7bcdf` | promoted additively |
| Closed PR #8 source artifact | head `1b766b88ddbd804564931ac35b6c6dd95b832b59`, artifact SHA-256 `50d0b1f2eb721e22dfc4277928ba3e221475296e5826135db1bb71cd4b1e3ad0` | promoted after current-tree differential test |
| AnyFunctor v0.6 overlap/Word Carrier | Library ZIP SHA-256 `fa9378b1dc57a53acd51ad0514dd49f70b40fd8450eff06cbc0930a509e394e8` | peer lineage; not source-merged |
| AnyFunctor Task-3 monotonic history CURRENT | Library ZIP SHA-256 `b35524cda199b250d674837f8c9a2c56dddbe25084eb92517cfe0513117bfeb9` | 10/10 targeted runtime tests pass; withheld as parallel AnyFunctor authority |
| Decision Field v0.7 Library package | materialized Library ZIP SHA-256 `bfb8c5c331fe99aa7cfcee79a8a28b3e8d3beff697e586b3be25d9b895fcf0e4` | current Git line supersedes code; supplied missing Dynamic fixture |
| Dynamic RMAL recovery overlay | SHA-256 `a59981175d9872d35ffbfcac88263266ea2fe9090d709b1ac5b9a8f577ab9cd1` | exact recovery repair promoted |
| Word Carrier / AnyFunctor / Orbit bundle | SHA-256 `bc60b41efd9186426ebe3f82af244a1c6bb73896fec5d206d203f530dfbcad2a` | withheld: exact `orbit_lab` recovered; fresh custody rerun blocked by missing source blob (follow-up below) |

## Promoted improvements

### WordDNA / triplex native carrier

Exact final PR #8 bytes are reused, including the U+0000 rendering regression repair.
The carrier preserves exact length-bounded UTF-8, scalar offsets, occurrence and
semantic IDs, support/clarity IDs, typed feature pairing, non-destructive fold/unfold,
and three strict directed list axes. New names remain native API concepts rather than
new RMAL lexer keywords.

### Frozen-witness / retained-source CI repair

PR #8's final RMAL workflow is reused. The historical Mirror v0.7 check now reconstructs
its frozen root CMake blob in a temporary witness tree while the current root CMake is
built and tested normally. Retained source is independently unpacked, rebuilt and
tested. This preserves historical evidence without making the root build immutable.

### Bounded structural checker

Closed PR #7's exact synthetic structural experiment is recovered with its finite
receipts. It independently distinguishes GENERATE, VERIFY and ADMIT; preserves ordered
occurrence/authority/evidence distinctions; requires exact reconstruction; rejects
stale or forged certificates; and implements bounded DuplicateEvaluator and
AdapterEvaluator proposals only.

### Dynamic RMAL recovery fixture

`tests/DYNAMIC_RMAL_CHARACTERS.rmal` is restored from exact Git blob
`c6b86d1c81479b742134ee08ca9e83b087afcf9d`. It repairs the existing verifier's
missing input. It remains a design-contract surface, not an executable language claim.

## Fresh differential verification before upload

On the combined current tree:

- root RMAL C23 CTest: **11/11 PASS**;
- WordDNA native and source-carrier tests: PASS;
- Dynamic RMAL verifier: **PASS**;
- structural checker: **39/39 PASS**;
- structural checker under `python -O`: **39/39 PASS**;
- local-plane successor: **1/1 PASS**;
- AnyFunctor/local-plane native seam against exact v0.3 archive
  `91a25de44269e6f54d45c5744b38af09eef2daf25b8fedbdab79ca0b4a018292`: PASS.

## Withheld code and why

- **Task-3 singular AnyFunctor runtime:** its focused C++23 suite passes 10/10, but its
  declared source commit `19f88fbdf749d8fdfccf7081cc27695dea4fb02e` is no longer resolvable
  in the connected Git repository and it defines a second execution authority. Preserve
  as a peer until an explicit bridge can prove authority/history equivalence.
- **v0.6 overlap / Proof Carrier line:** verified historical behavior is valuable, but
  it predates and does not contain the current native persistence/Mirror/Homeward line.
  Source-level merging would risk identity and replay regressions.
- **Orbit custody bundle:** still not self-contained. The exact `orbit_lab` dependency
  is now recovered, but its custody rerun stops on a missing content-addressed source
  blob before completing any of its seven checks. The historical result JSON remains
  historical evidence; it is not substituted for a fresh passing run.
- **Older RMAL 2.x / Decision Field experiments:** retained as lineage, not promoted over
  current RMAL 3.1/C23 unless a specific missing behavior survives a new current-tree probe.

```text
NEWER_TIMESTAMP != SUCCESSOR_AUTHORITY
TESTED_PEER != SAFE_SOURCE_MERGE
GENERATE != VERIFY != ADMIT
HISTORICAL_EVIDENCE != CURRENT_EXECUTION
PRESERVED_WITNESS != IMMUTABLE_CURRENT_BUILD
```

## Orbit dependency recovery follow-up — 2026-10-03

**Decision: WITHHELD. Dependency identity recovered; custody fixture incomplete.**
This follows the missing-import finding at DnD commit
`57786dd85cb13c4a8f657ae4a66ca6d4058854fa`; it preserves that failure as the prior stage.
No Orbit runtime, adapter or execution authority is promoted into DnD.

### Exact recovery chain

In `redogit/conscience64`, let `P` denote
`research/cross-carrier/2026-09-13/internal-update/` and `E` denote the archive-member
prefix `connection_next/extracted/orbit_lab_reflow_1_0/engine/`.

1. [PR #1](https://github.com/redogit/conscience64/pull/1), head
   `d02fe69465c7c39d70b2ea16579230d637027993`, branch
   `update/internal-workspace-20260914`, introduces
   `P/internal_connection_test_2026-09-13.zip`. The path-scoped reachable commit history
   through the supplied anchor returns this one introducing commit.
2. The introducing commit, merge `83d5bab5623dbbfff669a6fbbc3a27a4c48bfc1f`, and supplied
   anchor `ea546a415261ca21ae3be39dbe070e8ecade19f6` all retain Git blob
   `02bd91c27903f0ba6781c80ebff687d1a271b5ab` for that ZIP.
3. The [anchor README](https://github.com/redogit/conscience64/blob/ea546a415261ca21ae3be39dbe070e8ecade19f6/research/cross-carrier/2026-09-13/internal-update/README.md)
   explicitly says to extract the ZIP to restore `connection_next/` alongside
   `global_search/`. The direct repository lookup of `P/E/orbit_lab.py` returns 404:
   it is an archive member, not a directly committed source path.
4. The downloaded ZIP is 4,648,372 bytes, SHA-256
   `c18a59b2922e8556eff08077b7a4774f6dc08ad9cbca12234ab945624aa8545d`.
   Its computed Git blob ID also matches. All **736/736** members match the sizes and
   SHA-256 values in the anchor's `MATERIALS.json`; zero mismatches.
5. `P/global_search/import_working_packet.py` derives `ROOT` from its parent directory,
   adds `ROOT/E` to `sys.path`, and executes `from orbit_lab import OrbitLab`.
   Both `MATERIALS.json` and the embedded `BUNDLE_MANIFEST.sha256` identify the engine
   and test below. The custody bundle's retained `ORBIT_THREE_NODE_RESULT.json`
   independently names the same engine digest, establishing the intended dependency
   relationship; that retained receipt alone does not establish a fresh test pass.

| Recovered archive member | Bytes | SHA-256 |
|---|---:|---|
| `E/orbit_lab.py` | 203,048 | `2910df6adb525143423fb23676e5aaf376669cd7686401b42f63d64863fe1f18` |
| `E/tests/test_orbit_lab.py` | 2,532 | `fe426c89b8762c90f81c12b06e6da85a080508028f5abd8da0c5431007bf8912` |

### Fresh execution and remaining missing input

The exact custody ZIP was recovered at **85,969 bytes** with the original SHA-256
`bc60b41efd9186426ebe3f82af244a1c6bb73896fec5d206d203f530dfbcad2a`.
All **29/29** file entries in its `PACKAGE_SHA256.txt` verify. Its unchanged test and
adapter have SHA-256 `cfbf808cfcb9e105e58f54420a78cb1a39655498fee515e76e290b4719e09241`
and `09847836a94fb0e21f773324470a08e29cf8ae9e595e7c11a7e8fd800747b6c4`, respectively.

Fresh Linux x86-64 / Python 3.12.14 results, with bytecode writing disabled:

| Probe | Result | Evidence ceiling |
|---|---|---|
| Recovered engine's retained `test_orbit_lab.py` | **8/8 PASS**, exit 0 | This test file only; not a rerun of the historical 209-test suite |
| Exact custody test, recovered engine on `PYTHONPATH` | Exit 1: historical `/mnt/data/orbit_real_integration/ORBIT_THREE_NODE_RESULT.json` path absent | Import is resolved; runner assumes old workspace paths |
| Custody test with only three workspace-prefix relocations | Exit 1: `CustodyVerificationError: source blob missing` | State imports and setup reaches the first positive custody operation; **0/7 checks completed** |

Only path literals in a temporary test copy were relocated. The engine, adapter,
Orbit state, result fixture, and test assertions were unchanged. The original ZIP
and test bytes remain intact. Path mappings, commands, hashes and fresh output are
retained in [the recovery evidence](orbit-recovery-2026-10-03/EVIDENCE.json) and its
adjacent logs. This is an attempted rerun with the code dependency supplied, **not**
a dependency-and-fixture-complete passing run.

The absent input is the faithful scenario's source blob:

- expected SHA-256: `c390a188b2d3e167e18dca776cdb0945f3e534c0e45c7a5066b1a9aebe31ad60`;
- historical location: `/mnt/data/orbit_real_integration/blobs/<that SHA-256>`;
- retained source occurrence: `occurrence-710993e5d9`;
- retained content identity: `contentidentity-4ec137ac01`.

The exact source bytes are absent from the custody ZIP, the 736-member conscience64
recovery ZIP, and the 834-member pinned Word Carrier successor ZIP
`60e7f3ad4d5ef1d8067e3f6cf78cf158d01203f5dec1dbe240c12f90d8b9b960`.
The latter preserves the source occurrence and exporter code, but this search did not
recover the exact export bytes or their full original invocation. No substitute blob
was generated, no expected digest was changed, and no historical PASS was reused.

### Search scope and next gate

- GitHub default-branch code searches across `redogit` for the exact engine hash and
  `orbit_lab_reflow_1_0` found the conscience64 importer and two inventory records.
- Related-repository `orbit_lab` search covered `redogit/redogit`,
  `redogit/Other-Projects`, `redogit/DnD`, and `redogit/RMAL`; its sole returned file
  was this DnD cross-correlation record. An owner-wide exact source-blob digest query
  returned no matches. Indexed search is not exhaustive commit-history coverage.
- Enumerated 140 current conscience64 branch names through the terminal empty page.
  The introduction branch is preserved in PR #1 metadata but is absent from that
  inventory. Branch enumeration is not a claim that every branch tree was scanned.
- Inspected the untruncated 815-entry anchor tree, archive-path commit history,
  introduction/merge/anchor directory entries, manifests, importer, and exact archives.
  The dependency was recovered at the anchor; no exhaustive unrelated-history scan
  was needed or claimed.

The engine's source identity is now resolved. The faithful export blob remains
unrecovered. Recover bytes matching its digest (or independently reproduce and verify
that exact digest with a preserved invocation), then rerun all seven custody checks.
Only after a complete passing run may current-DnD integration and authority-equivalence
review be considered. Byte recovery or the eight engine tests alone do not grant
promotion, authenticated authority, or a theorem claim.
