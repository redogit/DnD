# `centre-run`: retained traversal and Homeward control

This pass adds a controller around the approved combined centering model. All four paths and six channels move together through the same source progress. The existing `centre` command remains available for one-frame evaluation.

## Input and identity

Command: `origami centre-run [input.json|-]`. Input is an object with exactly `schema`, `runId`, `channels`, `paths`, `core` and `operations`.

- `schema` is `origami-centering-run-input-v1`.
- `runId` is a nonempty label of at most 256 UTF-16 code units.
- Curves and core use the existing `CENTRE_CONTRACT.md` rules.
- `operations` has 0–128 entries. Each advance note is a string of 0–4096 UTF-16 code units.

The first retained frame has ID 0, no parent, q=1 and an empty note. Its four planar positions lie at the saved core-side endpoints in the supplied example. Every committed advance receives the next integer frame ID and a link to its recorded parent. IDs are scoped to this run; `runId` is caller-supplied, not an independently verified globally unique identifier. Keep the input and its provenance together with the output.

## Operations

| Input | Effect |
|---|---|
| `{"op":"advance","q":0.8,"note":"..."}` | While exploring, commit a frame only when requested q is strictly less than the active frame's q. Preserve its complete evaluated values and note. |
| `{"op":"advance","q":0,"note":"...","homeward":true}` | Apply the Homeward request before deciding the competing advance. Retain the attempted q and note, commit no new frame, and report `homeward_override`. The optional flag must be Boolean. |
| `{"op":"homeward"}` | Hold further advances; enter `returning` when away, or `home` when already at frame 0. No movement occurs in this operation. |
| `{"op":"return_step"}` | While returning, activate one stored parent. At frame 0, remain home. Before Homeward, reject with `homeward_required`. |

Equal or larger requested q is rejected as `not_outward` while exploring. Any ordinary advance after Homeward is rejected as `exploration_held`. Both rejections retain the attempted q and note. Repeated Homeward requests do not move or reset the traversal. New trace entries still record each request.

The controller starts in `exploring` phase while `atHome` is true. Phase expresses control permission; `atHome` expresses identity. Arrival keeps the hold active. This version has no automatic resume, import/resume session, branching from earlier frames or asynchronous external interrupt.

## What Home means here

Home is `activeFrameId == 0`, not a radius test, not the coordinate origin and not all-zero amplitudes. In the approved data, X, Y and Z remain nonzero at q=1.

Outbound traversal means decreasing saved q. It does not guarantee monotonically increasing radial distance. The approved straight-line interpolation across source gaps can curve inward in radius; a chord near q=1 cuts inside the radius-0.15 outline. For example q≈0.9986829314769925 has radius≈0.1438683988893535 on each spiral, but is still a distinct frame away from Home. Preserve this geometry and use identity for arrival.

## Retained output

Output schema: `origami-centering-run-v1`.

| Field | Meaning |
|---|---|
| `runId`, `homeFrameId` | Input run label; frame 0 is Home |
| `core` | Unchanged reference outline, emitted once |
| `frames` | Every committed frame, including initial Home; each has `id`, `parentId`, `q`, `note`, `channels`, `paths`, `points` |
| `trace` | Initial state and one event per supplied operation |
| `activeFrameId` | Current retained frame |
| `frontierFrameId` | Farthest committed frame in this strictly decreasing-q schedule; unchanged on return |
| `phase`, `atHome` | Final control phase and arrival identity |

Each trace entry has `seq`, `op`, `accepted`, `reason`, `phase`, `atHome`, `activeFrameId`, `frontierFrameId`, `retainedCount`, `attemptedQ`, `attemptedNote` and `homewardRequested`. `retainedCount` includes frame 0. Unused attempted fields and successful reasons are null. Each frame contains all four path positions, six amplitudes and 24 composed points in the same order as `centre`.

Returns reactivate stored frame values. They do not reconstruct them from a projection, rerun interpolation or delete the departed frame. Historical notes, attempted moves and the frontier remain available after arrival.

## Execution boundary

All control decisions, frame storage, parent links and composition execute in RMAL. The C host validates JSON and supplies bounded storage and transport. The whole request is deterministic within the tested runtime and produces output only on success. Existing input, output and VM budgets remain in force; output failure never silently truncates the history.

The complete input is validated before any operation executes. A malformed request is rejected as a whole; Homeward is not a way to execute otherwise invalid input.

Homeward priority is implemented at explicit operation boundaries, including a competing advance in the same operation. It is not preemption of an instruction already executing or a live OS event. Movement follows supplied curves; the controller does not generate new exploratory geometry or establish a physical torsion law.
