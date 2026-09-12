# DnD REDO — 2026

A small, framework-independent successor to the original MAUI shell.

## Current contract

- Parse `NdS`, `NdS+M`, and `NdS-M` dice expressions.
- Use an injected random source so rolls can be reproduced in tests.
- Compute ability modifiers with `floor((score - 10) / 2)`.
- Fail explicitly on malformed dice expressions or impossible deterministic values.

## Run

```bash
dotnet run --project DnD.Core.csproj
```

Expected checks:

- `dice parsing: PASS`
- `ability modifier: PASS`
- `deterministic roll: PASS`

This intentionally does not reproduce proprietary adventure text, settings, or content. It is a reusable tabletop calculation core.
