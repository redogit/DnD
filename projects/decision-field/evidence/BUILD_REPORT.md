# Decision Field component-analysis build report

**Date:** 2026-09-27
**Environment:** local Linux container, GCC 14.2, C++23

## TDD witness

The new component-analysis test target was written before the production header existed.

Expected RED result:

```text
fatal error: decision_field/component_analysis.hpp: No such file or directory
```

The header was then implemented and the new test target passed.

## Full verification

Command:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j2
ctest --test-dir build --output-on-failure
```

Result:

```text
decision_field_tests                       Passed
decision_field_self_opposite               Passed
decision_field_triadic_self_attack         Passed
decision_field_component_analysis_tests    Passed

100% tests passed, 0 tests failed out of 4
```

## New tests

`component_analysis_tests.cpp` verifies:

1. the canonical basis contains cardinal, diagonal, lateral, orthogonal and parametric signed views;
2. only stable representative structure is joined into the simpler core while varying dimensions remain residuals;
3. components with sufficient members recursively expose component-of-component substructure.

## Claim ceiling

This verifies the current executable C++ carrier and its test assertions.

It does not establish that the current scoring function is universally optimal, that the basis is complete for every domain, or that component analysis solves an external research problem.


## Standalone decoupling recheck

The component-analysis header was then decoupled from the predecessor engine header so the new GitHub slice can build independently. The full local predecessor + component suite was rerun after this change:

```text
decision_field_tests                       PASS
decision_field_self_opposite               PASS
decision_field_triadic_self_attack         PASS
decision_field_component_analysis_tests    PASS

100% tests passed, 0 tests failed out of 4
```

Current standalone component header SHA-256:

```text
d30b1500aedb4a60f83661171f11b03c0ef7bde2f12d5b32c4eeba92d6ab1ef3
```


## Exact standalone GitHub-slice witness

A clean temporary tree containing only the branch's standalone files was configured and built:

```text
CMake configure: PASS
C++23 compile:   PASS
decision_field_component_analysis_tests: PASS

100% tests passed, 0 failed out of 1
```

Verified file hashes:

```text
component_analysis.hpp
d30b1500aedb4a60f83661171f11b03c0ef7bde2f12d5b32c4eeba92d6ab1ef3

component_analysis_tests.cpp
585f62e239274311e72e37a2846db0e3f2e490644b516bea343fb9d562f0334b
```
