# REDOGIT

For this repository, redo means separating domain behavior from the obsolete application shell while preserving the shell as historical evidence.

## Legacy input

The predecessor is a .NET 6 MAUI application with an external `Orbit.Engine` project reference and very little game-domain behavior of its own.

## Successor boundary

The current successor under `successors/redogit-2026/` is a plain .NET core. It owns deterministic tabletop calculations only.

`expression -> parse -> roll source -> result`

and

`ability score -> modifier`

No UI framework is required to verify those operations.

## Completion rule

The executable checks parsing, modifier calculation, and deterministic rolling. Future character sheets, encounter tools, or MAUI views should consume this core rather than redefine its rules.

The old files remain available through Git history. The redo is a successor, not a rewritten past.
