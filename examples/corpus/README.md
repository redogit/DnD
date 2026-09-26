# RMAL Example Corpus

This directory is the broad example/input/output collection for RMAL.

- `git/` preserves every .rmal path found in accessible redogit Git repositories and the known RMAL development branch.
- `library/` contains all 38 .rmal files directly enumerable in the ChatGPT Library.
- `inputs/` contains source/input manifests.
- `outputs/` contains compiled RMAL objects/executables and generated result/manifests recovered from the Library.
- `INDEX.json` records exact blob identity, counts, discovery scope, and manifest-only names that were not individually retrievable.

Duplicates are intentional when the same bytes occur at different provenance coordinates.

```text
DUPLICATE_BYTES != DUPLICATE_PROVENANCE
DISCOVERED != RETRIEVED
EXAMPLE != LANGUAGE_AUTHORITY
OUTPUT != PROOF
```
