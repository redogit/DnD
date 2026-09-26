# RMAL Git Lineage Consolidation

Centralized into `redogit/DnD` by explicit user direction on 2026-09-26.

## DnD predecessor

Former DnD head:

`bf1a2fc39957351a779091d748366996e80e0573`

The old DnD working tree is not present in the new current tree. It remains recoverable from Git history.

## Imported source heads

- `redogit/Other-Projects-@main`
  - `a9043152b89bc7d8ba17723c6e51d9027018127d`
- `redogit/conscience64@main`
  - `61a011fadb89d7172dd968dfb9a2d95a4dbb69db`
- `redogit/redogit@main`
  - `2ee9125e91d36bb417d0f99981fc4c32620eb3a2`
- native C++ RMAL successor branch `redogit/Other-Projects-@rmal-cpp-language-v3`
  - `3963ea649449ef7b7a517cfa7174c6385fdef5fa`

The superseded C++ RMAL PR in Other-Projects- was closed after this repository became the canonical home.

## Consolidation rule

The originals are intentionally retained at their source repositories.

```text
CENTRALIZED != SOURCE_DELETED
COPIED_WITH_PROVENANCE != AUTHORITY_TRANSFER
RELATED != MERGED
HISTORY_PRESERVED != CURRENT_WORKING_TREE
```

The `provenance/` paths preserve source-repository context. Canonical current material is projected into top-level `src/`, `include/`, `spec/`, `docs/`, `rmal/`, `tests/`, and `evidence/`.
