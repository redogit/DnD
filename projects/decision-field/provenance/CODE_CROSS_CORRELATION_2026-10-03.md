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
| Word Carrier / AnyFunctor / Orbit bundle | SHA-256 `bc60b41efd9186426ebe3f82af244a1c6bb73896fec5d206d203f530dfbcad2a` | not promoted: package test lacks `orbit_lab` dependency |

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
- **Orbit custody bundle:** not self-contained; its retained test imports missing
  `orbit_lab`. Result JSON is not substituted for an executable dependency-complete test.
- **Older RMAL 2.x / Decision Field experiments:** retained as lineage, not promoted over
  current RMAL 3.1/C23 unless a specific missing behavior survives a new current-tree probe.

```text
NEWER_TIMESTAMP != SUCCESSOR_AUTHORITY
TESTED_PEER != SAFE_SOURCE_MERGE
GENERATE != VERIFY != ADMIT
HISTORICAL_EVIDENCE != CURRENT_EXECUTION
PRESERVED_WITNESS != IMMUTABLE_CURRENT_BUILD
```
