# DnD — REDO

The original repository is a .NET 6 MAUI shell. It remains intact as the predecessor.

The current redo starts from reusable tabletop behavior rather than UI scaffolding:

- parse common dice expressions such as `2d6+3`;
- evaluate rolls through an injectable random source;
- compute standard ability modifiers;
- keep the domain core independent of MAUI and Orbit.

## Current successor

See `successors/redogit-2026/`.

```bash
cd successors/redogit-2026
dotnet run
```

The executable runs deterministic checks and exits nonzero if the contract fails.

See [`REDOGIT.md`](REDOGIT.md).
