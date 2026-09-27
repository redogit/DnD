# Decision Field component-analysis review

**Date:** 2026-09-27

## Scope reviewed

- typed directional basis construction;
- obligation-relative scoring;
- primary-component assignment;
- recursive component-of-component decomposition;
- simpler-core join;
- residual preservation;
- build/test isolation from root RMAL/RMALC.

## Material defect found

### Representative-only core collapse

Initial implementation decided core stability from one representative observation per retained primary component.

Counterexample:

```text
UP component:
  representative x = 1
  retained member x = 2

DOWN component:
  representative x = 1
```

Representative-only comparison falsely concluded:

```text
x = 1 is stable
```

even though a retained state required:

```text
x = 2
```

The regression test was written first and failed.

## Repair

The join now gathers **all retained member observations** from every retained primary component.

A coordinate is admitted only when:

```text
max(all retained values) - min(all retained values) <= tolerance
```

Otherwise the observed range remains a residual.

## Verification after repair

```text
Full local predecessor + component suite: 4/4 PASS
Standalone GitHub slice:                1/1 PASS
```

## Remaining bounded risks

1. Current component assignment is exclusive: each observation enters only its highest-scoring admissible basis at a level. Multi-membership is not yet modeled.
2. The current quality function linearly combines obligation/evidence/cost values without domain-specific normalization; it is an explicit first carrier, not a universal metric.
3. Cardinal/diagonal/lateral/orthogonal bases can be correlated. Orthogonality is a declared domain relation, not something this module proves.
4. Component recursion excludes the parent basis but does not yet calculate new local bases from discovered component geometry.
5. No direct adapter from the 192-probe Decision Field execution record exists yet.

These are Remainder, not hidden assumptions.
