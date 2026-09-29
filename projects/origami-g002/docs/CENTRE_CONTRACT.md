# `centre`: shared-progress composition

Command: `origami centre [input.json|-]`. Omit the input filename to read stdin.

## Input

The exact required fields are:

| Field | Contract |
|---|---|
| `schema` | `origami-centering-input-v1` |
| `q` | Finite number, 0 through 1 |
| `channels` | Six objects, each with `name` and `points` |
| `channels[c].points` | Ordered `[q, amplitude]` knots |
| `paths` | Four objects, each with `name` and `points` |
| `paths[p].points` | Ordered `[q, x, y]` knots |
| `core` | The retained planar outline as `[x,y]` points |

Each knot series contains 2–2000 points, starts at q=0, ends at q=1 and has strictly increasing q. Different series may have different knots. The core contains 2–2000 points. Coordinates/amplitudes must be finite and within ±1e100; each nonempty name is at most 256 UTF-16 code units. Existing host resource limits still apply.

Array indices identify channels and paths. Names may repeat, as they do for the two saved neutral paths. Names are not JSON lookup paths. Unknown fields and malformed records are rejected rather than silently ignored.

The included example is an exact numerical extraction of the approved visual's arrays. The native sampler accepts other bounded curves of the same shape, but that does not make them part of the approved source fixture.

## Evaluation

At an exact knot, return its saved value. Between knots, interpolate linearly using their actual q coordinates. Endpoints use their saved values. Never resample by row index or replace the 80% marker with a zeroing instruction.

For every path p and channel c:

`point[p,c] = [path_x(p,q), path_y(p,q), amplitude(c,q)]`

The sampler is stateless. Request order does not change an answer. Choosing a lower q evaluates an earlier location; it is not an inverse reconstruction or an implemented Homeward override.

## Output

| Field | Meaning |
|---|---|
| `schema` | `origami-centering-frame-v1` |
| `q` | Requested progress |
| `markerQ` | 0.8, the approved saved-graph marker |
| `channels` | Six `{name, amplitude}` entries in input order |
| `paths` | Four `{name, xy}` entries in input order |
| `points` | Twenty-four `{pathIndex, channelIndex, xyz}` entries; path-major, channel-minor order |
| `core` | Unchanged numerical outline from the input |

`xyz[2]` is channel amplitude used as display height. It is not an additional measured spatial coordinate. The output carries current evaluations, not the full curve history; retain the input to preserve every source knot.

`markerQ` is specific to this approved graph's mapping. The `core` is a reference outline: this sampler does not infer a force or alter the input paths to fit it.
