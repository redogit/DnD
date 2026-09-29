# Origami G002 — source import and current boundary

This project module carries the approved combined centering and saved planar torsional-core relationship, followed by the bounded `centre-run` traversal. It is separate from the root RMAL/RMALC compiler and from GitHub Pages.

## Current executable relationship

The exact saved fixture has four distinct planar paths, six channel signals and a core outline. At normalized source progress `q`, RMAL evaluates each path and each channel, producing 24 `[x(q), y(q), amplitude(q)]` compositions. Alignment is declared as `q = channel time / 50 = original path order / maximum path order`; interpolation uses original, nonuniform knots. The 24 displays are combinations of saved data, not independent agents or measured physical trajectories.

`centre-run` starts from retained frame ID 0 at `q=1`, accepts scheduled advances toward lower `q`, and stores every complete frame, parent and note. A Homeward request holds a competing advance at the operation boundary; return activates stored parents without discarding the frontier or recomputing a frame. Arrival is by frame ID 0. The saved paths can briefly cross inside the core outline, so radius is not an arrival rule. The three physical-labelled channel amplitudes remain nonzero at the final saved endpoint.

See [the active direction](docs/ACTIVE_DIRECTION.md), [sampler contract](docs/CENTRE_CONTRACT.md), [run contract](docs/CENTRE_RUN_CONTRACT.md), [retired interpretation errors](history/INTERPRETATION_ERRORS.md), and [source identities](SOURCE_MANIFEST.json).

## Repository and toolchain boundary

`program/`, `native/`, `data/`, `examples/`, `tests/` and `reference-js/` are source and fixtures copied byte-for-byte from the previously delivered 1.3.0 package. The local `toolchain/` is the package's **project-specific numeric RMALC fork** based on DnD commit `4282676da043cca93b1622532d94ae419d9795ab`. It supports floating-point values and the bounded numeric host ABI required by Origami. It does not update the repository-root canonical RMAL 3.1 language, compiler, or VM. No call from another DnD project is presumed compatible without an explicit integration test.

The exact package SHA-256 is in `SOURCE_MANIFEST.json`, which lists every imported path and byte hash. This source import omits the packaged Windows binaries, duplicate upstream toolchain copy, private research record and historical build logs. The previously delivered package remains the binary delivery; this tree is source for review and reproducibility.

## Build and test

The intended build uses Python 3 and Zig 0.14.1:

```bash
python3 scripts/build.py --target linux
python3 tests/centering_run.py --binary build/linux/origami
python3 tests/centering.py --binary build/linux/origami
python3 tests/homeward.py --binary build/linux/origami
python3 tests/parity.py --binary build/linux/origami
python3 tests/homeward_parity.py
python3 native/test_host.py build/linux/origami
node --test reference-js/tests/*.test.cjs
```

For Windows, `python3 scripts/build.py --target windows` cross-builds when Zig is available; the delivered package also contains `demo.cmd` and `verify.cmd`. This imported source tree has not been executed on Windows. The prior package's Windows build/format receipt does not establish Windows runtime behavior for 1.3.0.

The [Linux recheck receipt](evidence/LOCAL_RECHECK_2026-09-29.json) records a GCC 13 C2x accommodation used because Zig was unavailable in that environment. It is a bounded independent run, not an official ISO C23/Zig build or a Windows test.

## Remainder

The current runner traverses supplied curves under an explicit schedule. Dynamic path generation, a physical torsional transform/inverse law, adaptive learning, cross-cohort synchronization, and asynchronous interruption remain open. The combined graph is the active design reference; earlier assistant interpretations are retained only as failure evidence. No website is published from this module.
