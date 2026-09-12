# DnD — REDO

The original repository is a .NET 6 MAUI shell. It remains intact as the predecessor.

## Current build

From the repository root:

```bash
dotnet build REDOGIT.slnx --configuration Release
dotnet run --project successors/redogit-2026/DnD.Core.csproj --configuration Release --no-build
```

`REDOGIT.slnx` is the current solution entry point. The historical MAUI project and `Orbit.Engine.sln` remain predecessor material rather than being rewritten into the successor.

The current .NET 10 redo starts from reusable tabletop behavior rather than UI scaffolding:

- parse common dice expressions such as `2d6+3`;
- evaluate rolls through an injectable random source;
- compute standard ability modifiers;
- keep the domain core independent of MAUI and Orbit.

The executable runs deterministic checks and exits nonzero if the contract fails.

See [`REDOGIT.md`](REDOGIT.md).
