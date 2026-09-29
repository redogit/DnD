# 13D Origami Algorithm

Standalone JavaScript implementation. Node.js 22 or newer; no installation, browser, server, network access or external packages required.

The numeric algorithm applies an ordered sequence of orthogonal rotations to a 13-coordinate vector, separates the first three coordinates from the ten-coordinate residual, and reconstructs the input by reversing the rotations. The decision algorithm checks whether every group of indistinguishable states has at least one action admissible for the entire group.

Version `1.1.0-homeward` also exports `explore(input)` and CLI command `explore`: retained single-branch discovery, Homeward control and a full operation trace. See `../docs/HOMEWARD.md` for the contract, examples and boundaries. Run `node bin/origami.cjs explore ../examples/homeward.json` from this directory. This pass does not yet combine exploration with the numerical transforms.

## Run

Extract this folder, then run:

```sh
node bin/origami.cjs encode examples/encode.json > packet.json
node bin/origami.cjs recover packet.json
node bin/origami.cjs decide examples/decisions.json
node --test tests/*.test.cjs
```

Commands accept a JSON filename, or `-` for standard input. Output is JSON. Invalid input produces an error on standard error and exit code 1. Input files are limited to 1 MiB. Nothing is uploaded or saved automatically.

## Use from code

```js
const { encode13, recover13, decisionSufficiency } = require('./index.cjs');

const input = [1, 2, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1];
const packet = encode13(input, [
  { i: 0, j: 12, degrees: 45 },
  { i: 1, j: 8, degrees: -30 }
]);

const restored = recover13(JSON.parse(JSON.stringify(packet)));
const check = decisionSufficiency([
  { id: 'positive', observation: [0, 0, 0], actions: ['positive D13'] },
  { id: 'negative', observation: [0, 0, 0], actions: ['negative D13'] }
]);
// check.sufficient === false: identical observations require incompatible actions.
```

Indices are zero-based: `0` is D1 and `12` is D13. Angles are degrees in [-360, 360]. Rotation order matters. Vectors must have 13 finite coordinates with a finite Euclidean norm; at most 4096 rotations are accepted. Caller arrays are copied.

Supply dimensionless coordinates, or normalize each physical channel using an explicit external unit/scale convention before calling the algorithm. Retain that convention if physical units must be reconstructed. No normalization or physical channel coupling is inferred from the source's conflicting dimension groupings.

## Algorithm and guarantees

For each rotation in plane `(i,j)`, with `c = cos(theta)` and `s = sin(theta)`:

```text
y[i] = c*x[i] - s*x[j]
y[j] = s*x[i] + c*x[j]
```

Other coordinates remain unchanged. Encoding returns exactly:

```json
{
  "schema": "origami-transform-v1",
  "visible": [0, 0, 0],
  "residual": [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
  "rotations": []
}
```

Recovery concatenates `visible` and `residual`, then applies the negative angles in reverse order. The packet does not store the original input. The transform is invertible in exact arithmetic; numerical reconstruction uses JavaScript double precision and is not byte-exact recovery of arbitrary source data. Tests use a 1e-12 absolute tolerance on their bounded fixtures. Extremely small values may underflow; ill-scaled inputs can lose small components, and long sequences can accumulate error. Overflowing coordinates or norms are rejected. The residual is necessary: three visible coordinates alone cannot reconstruct an unrestricted 13-coordinate state. This representation is not data compression.

Packets are validated for shape and numerical validity. They are not signed or authenticated; a numerically valid edited packet represents a different state. They carry no scientific claim status or admission authority. They are independent of the former visualizer's UI snapshot format.

For finite states `S`, observation function `C`, and nonempty admissible action sets `A(s)`, the decision check evaluates:

`for every observed value z: intersection of A(s) over all s with C(s)=z is nonempty`

The function returns `{sufficient, fibers}`; each fiber contains its observation, state IDs and common actions. Empty intersections identify precisely the supplied groups that fail the criterion. No action is executed. Pairwise compatibility is insufficient: the example action sets `{a,b}`, `{b,c}` and `{a,c}` share no action across the whole group.

Observations are supplied finite numeric arrays of equal length, compared exactly after JSON parsing; +0 and -0 coincide. No rounding, approximate grouping or uncertainty model is imposed. Results concern only the supplied finite states and action sets. Limits: 10000 states, 1024 coordinates per observation, 1000 actions per state, unique state IDs, and nonempty string IDs/actions of at most 256 characters.

## Supporting algorithms

| API / command | Computation |
| --- | --- |
| `rotate13(v,i,j,degrees)`, `project13(v)` | Single-plane transform; projection plus residual and norms |
| `step(feed,state)`, `simulate(feed,initial,steps)` / `simulate` | Lorenz and Rossler RK4 streams; discrete Clifford map |
| `miura(fold,n=13,a=2.7,b=2.7,gamma=60)` / `geometry` | Ideal zero-thickness Miura sheet vertices and coupled fold angles |
| `circuit(frequency,options)` / `rf` | Passive two-port ABCD estimate of complex S21 |
| `resonance(inductance,capacitance)` | Ideal LC notch in GHz, inputs nH and pF |
| `sourceInterpolation(fold)` | Original frequency/polarization interpolation; an unverified source formula |

```sh
node bin/origami.cjs simulate examples/simulation.json
node bin/origami.cjs geometry examples/geometry.json
node bin/origami.cjs rf examples/rf.json
```

Simulation returns the final state and exact step count, with no hidden running process. Lorenz uses two RK4 half-steps of 0.005 per call; Rossler uses two of 0.01. The Clifford map is a discrete attractor map, not Clifford algebra. At most 1000000 steps per simulation are accepted. A finite next step does not establish global stability for arbitrary initial states.

The geometry uses arbitrary consistent length units, fold 0..90 degrees, sector angle 0.01..89.99 degrees and 1..100 cells along each direction. The 90-degree endpoint permits ideal zero-thickness overlap. It does not model finite material thickness, strain or fatigue.

RF inputs use GHz, pF, nH, ohms and ps. Defaults: two 50-ohm 10-ps lines, a series 0.08-ohm/1.5-nH crease impedance and a shunt series 2.2-nH/0.75-pF branch. Available options are `capacitance`, `inductance`, `crease`, `resistance`, and `delay`. The exact ideal notch returns magnitude 0 and `null` dB/phase values. There is no fold-to-capacitance law, varactor or active-device model.

## Sources and research connection

`source/` preserves both supplied files byte-for-byte. The original HTML is documentary input; it is not the application entry point. `evidence/` retains the earlier claim ledger and validation report as historical records. Statements in those documents about the visualizer describe that earlier version. Their original-document links resolve to the files under `source/` in this package. `SOURCE-MANIFEST.json` records exact source and predecessor module hashes.

The research ideas now implemented as reusable algorithms are:

- Retained residual and inverse-order reconstruction: reversible recovery requires the information omitted by a projection.
- Obligation-indexed decision sufficiency: the same observation can be adequate for one requested decision and inadequate for another.
- Separation of a numeric result from evidence/admission: no transform packet can promote a physical claim or authorize an external action.

These implement methods discussed in the September 20–27 AnyFunctor, Decision Field, Word Carrier and RMAL research connection work. Native adapters to those projects are not included. The prior cross-project report remains a separate research artifact.

Full 8192-component Cl(13,0) multivector propagation and the specification's gravitational, thermodynamic, physical latch and hardware-reversibility claims are not implemented or established by these vector algorithms. The physical validation findings in the existing audit remain applicable.

## Verification

`tests/` contains executable numerical, input-contract and CLI tests, including independent coordinate expectations, all 78 planes, noncommuting inverse sequences, projection collisions, whole-fiber intersections, deterministic streams, an independent integration reference, 1014 Miura facets and passive RF boundaries. `TEST-RESULTS.txt` is the captured run for this package. No hardware experiment, ngspice execution or native cross-project runtime is claimed.
