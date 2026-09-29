# Native host boundary

`main.c` is a C23 console host for the RMAL program. The host supplies JSON
transport, input contracts, numeric buffers, standard scalar math and buffered
output. The algorithms, including the approved centering sampler, are implemented in `program/*.rmal`; no Origami
rotation, inverse, decision intersection, stream integration, sheet geometry or
RF model formulas or centering interpolation live in the host.

The build embeds the exact combined RMAL source in `origami_program.h`. The
native application compiles this embedded source to RMALC bytecode and executes
it through the RMAL VM for each invocation. This is an embedded language runtime,
not a claim that RMALC emits native machine code for the algorithms.

## Input and output

Use `origami <centre-run|centre|encode|recover|decide|simulate|geometry|rf|explore> [input.json|-]`.
With no input filename, JSON comes from standard input. With no command, the
application displays help. `--version` identifies the application and compiler.

The JSON contracts follow the supplied JavaScript command-line implementation,
including defaults, zero-based rotation indices, finite vector norms, exact
finite observation equality, Unicode UTF-16 label length limits, field allowlists
and numeric ranges. All input is checked before running RMAL. Optional values
that are explicitly `null` are rejected; absent values use the RMAL defaults.

The native input parser deliberately rejects duplicate object keys, malformed
UTF-8, unpaired escaped UTF-16 surrogates and embedded NUL. These strictness rules
define an accepted-input subset of JavaScript `JSON.parse`; duplicate-key
last-write behavior and strings that cannot be represented by the RMAL C string
API are not silently approximated.

Results accumulate in memory. On input, compilation or VM failure, stdout is
empty and stderr contains a JSON error object with exit code 1. On success,
stdout receives one JSON result and exit code 0. Operating-system write errors
can still interrupt delivery of an already computed result.

## Resource limits

| Resource | Limit |
|---|---:|
| Input | 1 MiB |
| Parsed JSON nodes | 100,000 |
| JSON nesting | 64 levels below the root |
| Output | 8 MiB |
| Numeric buffers | 256 |
| Combined buffer cells | 2,000,000 doubles |
| VM instructions per invocation | 100,000,000 |

The input contracts allow up to 4,096 rotations, 1,000,000 simulation steps,
10,000 decision states, 1,024 observation coordinates and 1,000 actions per state.
The input and instruction budgets may stop a large otherwise well-formed request
before those individual maxima are reached. Failure is explicit; it does not
return a partial decision or numerical result. No wall-clock performance claim
is made.

## Host function ABI

- `command()` returns the selected operation.
- `has(path)`, `number(path)`, `integer(path)`, `text(path)` and `length(path)`
  read checked JSON. An empty path means the root; dots separate object fields
  and array indices. `integer` requires a JavaScript-safe integer.
- `buffer_new(length)` returns a zero-initialized numeric buffer handle;
  `buffer_get(handle,index)` and `buffer_set(handle,index,value)` enforce bounds.
- `sin`, `cos`, `sqrt`, `abs`, `atan2`, `pow`, `log10`, and `hypot2` expose standard
  finite scalar math. Invalid or nonfinite results fail the invocation.
- `quote(text)` returns escaped JSON string syntax. `emit(value)` appends raw
  string contents or JSON number, Boolean or null syntax to the result buffer.

`test_host.py` exercises input rejection and the empty-stdout failure guarantee.
The parent project's parity suite tests model results against the preserved
JavaScript implementation; these tests serve different purposes.

The active `centre` input/output contract is in `../docs/CENTRE_CONTRACT.md`. Historical utilities retain their earlier schemas.

`centre-run` adds retained frame traversal and Homeward control; see `../docs/CENTRE_RUN_CONTRACT.md`. These transitions and retained numeric snapshots execute in RMAL.
