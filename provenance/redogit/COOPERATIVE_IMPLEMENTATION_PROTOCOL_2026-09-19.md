# Cooperative Implementation Protocol — 2026-09-19

## Attribution and authority

This work follows a deliberately asymmetric provenance rule:

- **Design authority:** Ryan McMillan.
- **Coimplementation:** Ryan McMillan + ChatGPT/OpenAI tooling, iteratively implemented and refined under Ryan's direction, constraints, corrections, and acceptance gates.
- **No authority transfer:** implementation assistance does not transfer design authority, scientific authority, repository authority, or authorship of user-origin concepts.

Canonical shorthand:

```text
USER_DESIGN_AUTHORITY
  + HUMAN_AI_COIMPLEMENTATION
  + EXPLICIT_PROVENANCE
  + NO_AUTHORITY_TRANSFER
```

## Root goal

> Increase humanity's practical freedom to understand, preserve, question, create, cooperate, recover, and act on what matters—while keeping knowledge reconstructible, evidence honest, systems accessible, privacy protected, agency intact, and the future open to correction and change.

Short form:

> Help humans remain free, capable, and able to recover what they need to live well.

Root invariant:

```text
THE SYSTEM SERVES HUMAN LIFE.
HUMAN LIFE DOES NOT SERVE THE SYSTEM.
```

## Cooperation law

> **Cooperate by reference, proposal, and explicit handoff — never by silent cross-writing.**

Operational path:

```text
READ / REFERENCE
      ↓
PROPOSE
      ↓
HANDOFF
      ↓
TARGET DECIDES
      ↓
TARGET-LOCAL SUCCESSOR
```

Required distinctions:

```text
OWN_NAMESPACE = EXCLUSIVE_WRITE
OTHER_NAMESPACE = READ_ONLY_BY_DEFAULT
RELATED != AUTHORIZED_TO_EDIT
READ != WRITE
PROPOSED != ACCEPTED
ACCEPTED != VERIFIED
SHARED_METHOD != SHARED_EVIDENCE
SUCCESSOR != REWRITTEN_PREDECESSOR
```

## Git application

1. A repository or workstream remains authoritative for its own state.
2. Cross-repository cooperation uses references, issues, PRs, reviewable proposals, or explicit handoffs.
3. One repository does not silently rewrite another repository's current state.
4. Shared evidence is linked with provenance; the link does not transfer scientific authority.
5. Changes are forward-only successors. Existing commits and predecessor evidence remain intact.
6. Ambiguous or conflicting authority returns `UNRESOLVED`; last-writer-wins is not an authority rule.
7. Publication, repository visibility, and licensing remain independent decisions.

```text
PUBLICLY_VISIBLE != PUBLICATION_APPROVED != COMMERCIALLY_LICENSED
```

## RMAL carrier

The executable RMAL carrier for this protocol is maintained as an additive language artifact in `redogit/Other-Projects-`:

`docs/RMAL_COOPERATIVE_IMPLEMENTATION_PROTOCOL.rmal`

The carrier encodes the root goal, user design authority, human-AI coimplementation relation, cross-write denial, handoff requirement, preservation obligations, and evidence boundaries using the implemented RMAL surface.

The broader prose protocol remains authoritative for semantics that are not yet first-class RMAL syntax.

## Claim ceiling

This protocol governs coordination and provenance. It does not:

- make ChatGPT an independent owner of user-origin work;
- make repository linkage an authority transfer;
- turn compilation into scientific validation;
- claim universal optimality for this coordination model;
- override repository-local privacy, licensing, or evidence gates.
