# DnD — REDO

The original repository is a .NET 6 MAUI shell. It remains intact as the predecessor.

## Current build — v2

From the repository root:

```bash
dotnet build REDOGIT.slnx --configuration Release
dotnet run --project successors/redogit-2026-v2/DnD.SelfCheck/DnD.SelfCheck.csproj --configuration Release --no-build
```

`REDOGIT.slnx` is the current solution entry point. v2 separates reusable tabletop behavior from verification:

- `DnD.Core` — dice parsing, range calculation, injectable randomness, and ability modifiers;
- `DnD.SelfCheck` — executable contract for parsing, min/max totals, deterministic rolling, ability modifiers, and malformed-input rejection.

GitHub Actions builds this same root solution and runs the same verifier.

## Preserved predecessors

- The historical MAUI project and `Orbit.Engine.sln` remain predecessor material.
- `successors/redogit-2026/` remains the first verified framework-independent successor and is now the predecessor to v2.

This successor intentionally contains reusable tabletop calculations only. It does not reproduce proprietary adventure, setting, or rulebook text.

See [`REDOGIT.md`](REDOGIT.md).
