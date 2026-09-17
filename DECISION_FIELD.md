# Decision Field Profile — DnD Successor

Canonical method: `redogit/redogit/DECISION_FIELD_CORE_2026-09-17.md`.

This profile covers only the reusable tabletop-calculation successor contract.

```text
X = parsed dice expressions / bounded calculation states
O = declared parser/arithmetic/roll/modifier contract
D = syntactic and numeric distinctions consequential to that contract
R = parse, count, face-range, injected-randomness and modifier relations
F = parse, validate, compute min/max, roll with injected source, apply modifier, reject malformed input, verify
E = executable self-check + workflow/provenance evidence
G = declared calculations and malformed-input behavior are correct
U = full tabletop rules, proprietary content, campaign semantics and untested edge cases
```

```text
CALCULATION_CORE != FULL_RULES_ENGINE
DETERMINISTIC_TEST != RANDOMNESS_QUALITY_CERTIFICATION
SUCCESSOR != PREDECESSOR_REWRITE
```
