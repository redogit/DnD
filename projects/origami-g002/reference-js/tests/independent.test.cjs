'use strict';
const test = require('node:test');
const assert = require('node:assert/strict');
const { encode13, recover13, decisionSufficiency, simulate } = require('../index.cjs');

const basis = i => Array.from({ length: 13 }, (_, j) => Number(i === j));
function closeVector(actual, expected, tolerance = 1e-12) {
  assert.equal(actual.length, expected.length);
  actual.forEach((x, i) => assert.ok(Number.isFinite(x) && Math.abs(x - expected[i]) <= tolerance * Math.max(1, Math.abs(expected[i])), `coordinate ${i}: ${x} versus ${expected[i]}`));
}

test('a positive quarter turn carries D1 into D13 with the prescribed orientation', () => {
  const packet = encode13(basis(0), [{ i: 0, j: 12, degrees: 90 }]);
  closeVector(packet.visible, [0, 0, 0]);
  closeVector(packet.residual, [0, 0, 0, 0, 0, 0, 0, 0, 0, 1]);
  const reverseAxis = encode13(basis(12), [{ i: 0, j: 12, degrees: 90 }]);
  closeVector(reverseAxis.visible, [-1, 0, 0]);
});

test('all 78 coordinate planes preserve norm and permit numerical recovery', () => {
  const input = [0.25, -0.5, 1, -2, 3, -0.75, 0.125, 4, -3, 2.5, -1.5, 0.625, 7];
  const norm = Math.hypot(...input);
  for (let i = 0; i < 13; i++) for (let j = i + 1; j < 13; j++) {
    const packet = encode13(input, [{ i, j, degrees: 37 }]);
    assert.ok(Math.abs(Math.hypot(...packet.visible, ...packet.residual) - norm) < 1e-12);
    closeVector(recover13(packet), input);
  }
});

test('serialized recovery reverses a noncommuting sequence without a saved original vector', () => {
  // D1 -> D2 -> D3. Inverting these rotations in forward order cannot recover D1.
  const packet = encode13(basis(0), [{ i: 0, j: 1, degrees: 90 }, { i: 1, j: 2, degrees: 90 }]);
  closeVector(packet.visible, [0, 0, 1]);
  const portable = JSON.parse(JSON.stringify({ schema: 'origami-transform-v1', visible: packet.visible, residual: packet.residual, rotations: packet.rotations }));
  closeVector(recover13(portable), basis(0));
});

test('equal visible coordinates and equal norms do not replace the ten-coordinate residual', () => {
  const positive = encode13(basis(12));
  const negative = encode13(basis(12).map(x => -x));
  closeVector(positive.visible, negative.visible);
  assert.equal(Math.hypot(...positive.residual), Math.hypot(...negative.residual));
  assert.equal(recover13(positive)[12], 1);
  assert.equal(recover13(negative)[12], -1);
  assert.throws(() => recover13({ ...positive, residual: positive.residual.slice(0, 9) }));
});

test('encoding and recovery do not share mutable arrays with their callers', () => {
  const input = basis(0), rotations = [{ i: 0, j: 12, degrees: 90 }];
  const packet = encode13(input, rotations);
  input[0] = 77; rotations[0].degrees = 0; rotations.push({ i: 1, j: 2, degrees: 10 });
  closeVector(recover13(packet), basis(0));
  const recovered = recover13(packet); recovered[0] = 99;
  closeVector(recover13(packet), basis(0));
  assert.equal(packet.rotations.length, 1);
});

test('malformed and numerically overflowing transformations are rejected', () => {
  for (const invalid of [null, [], Array(12).fill(0), Array(14).fill(0), Array(13), [...basis(0).slice(0, 12), NaN], [...basis(0).slice(0, 12), Infinity]]) assert.throws(() => encode13(invalid));
  for (const rotation of [{ i: 0, j: 0, degrees: 1 }, { i: -1, j: 0, degrees: 1 }, { i: 0, j: 13, degrees: 1 }, { i: 0.5, j: 1, degrees: 1 }, { i: 0, j: 1, degrees: NaN }, { i: 0, j: 1, degrees: Infinity }]) assert.throws(() => encode13(basis(0), [rotation]));
  const huge = [Number.MAX_VALUE, Number.MAX_VALUE, ...Array(11).fill(0)];
  assert.throws(() => encode13(huge, [{ i: 0, j: 1, degrees: 45 }]));
  const packet = encode13(basis(0));
  assert.throws(() => recover13({ ...packet, visible: [Infinity, 0, 0] }));
});

test('decision sufficiency depends on whole fibers, not merely pairwise compatibility', () => {
  const same = decisionSufficiency([{ id: 'A', observation: [0], actions: ['keep', 'a'] }, { id: 'B', observation: [0], actions: ['keep', 'b'] }]);
  assert.equal(same.sufficient, true);
  assert.deepEqual(same.fibers[0].commonActions, ['keep']);
  const pair = decisionSufficiency([{ id: 'A', observation: [0, 0, 0], actions: ['positive'] }, { id: 'B', observation: [0, 0, 0], actions: ['negative'] }]);
  assert.equal(pair.sufficient, false);
  const collective = decisionSufficiency([{ id: 'A', observation: [0], actions: ['a', 'b'] }, { id: 'B', observation: [0], actions: ['b', 'c'] }, { id: 'C', observation: [0], actions: ['a', 'c'] }]);
  assert.equal(collective.sufficient, false);
  assert.deepEqual(collective.fibers[0].commonActions, []);
  assert.deepEqual(new Set(collective.fibers[0].ids), new Set(['A', 'B', 'C']));
});

test('decision groups use stored numeric values rather than rounded display coordinates', () => {
  const result = decisionSufficiency([{ id: 'A', observation: [1], actions: ['a'] }, { id: 'B', observation: [1 + Number.EPSILON], actions: ['b'] }]);
  assert.equal(result.sufficient, true);
  assert.equal(result.fibers.length, 2);
});

test('decision results do not alias source rows', () => {
  const rows = [{ id: 'A', observation: [0], actions: ['keep'] }];
  const result = decisionSufficiency(rows);
  rows[0].observation[0] = 1; rows[0].actions[0] = 'change'; rows[0].id = 'B';
  assert.deepEqual(result.fibers[0], { observation: [0], ids: ['A'], commonActions: ['keep'] });
});

test('invalid numeric observations and empty or non-string action sets are rejected', () => {
  for (const observation of [[NaN], [Infinity], Array(1), ['0']]) assert.throws(() => decisionSufficiency([{ id: 'A', observation, actions: ['a'] }]));
  for (const actions of [[], [1], null]) assert.throws(() => decisionSufficiency([{ id: 'A', observation: [0], actions }]));
});

test('simulation preserves a Lorenz fixed point and uses independent initial-state copies', () => {
  const input = [0, 0, 0];
  assert.deepEqual(simulate('lorenz', input, 100).state, [0, 0, 0]);
  const zeroSteps = simulate('lorenz', input, 0); zeroSteps.state[0] = 1;
  assert.deepEqual(input, [0, 0, 0]);
});

test('one Lorenz timestep agrees with independently integrated equations', () => {
  const initial = [0.1, 0, 0], actual = simulate('lorenz', initial, 1);
  let ref = initial.slice();
  const dt = 0.01 / 10000;
  for (let i = 0; i < 10000; i++) {
    const [x, y, z] = ref;
    ref = [x + 10 * (y - x) * dt, y + (x * (28 - z) - y) * dt, z + (x * y - 8 * z / 3) * dt];
  }
  closeVector(actual.state, ref, 2e-7);
  assert.deepEqual(initial, [0.1, 0, 0]);
});

test('each supported simulation is deterministic and finite over 1000 steps', () => {
  for (const feed of ['lorenz', 'rossler', 'clifford']) {
    const a = simulate(feed, [0.1, 0, 0], 1000), b = simulate(feed, [0.1, 0, 0], 1000);
    assert.deepEqual(a, b); assert.ok(a.state.every(Number.isFinite));
    assert.equal(a.feed, feed); assert.equal(a.steps, 1000);
  }
});

test('simulation rejects inherited feed names, invalid counts and divergent states', () => {
  for (const feed of ['unknown', 'toString', '__proto__']) assert.throws(() => simulate(feed, [0, 0, 0], 1));
  for (const steps of [-1, 0.5, Infinity, NaN]) assert.throws(() => simulate('lorenz', [0, 0, 0], steps));
  for (const initial of [[Infinity, 0, 0], [NaN, 0, 0], [1e308, 1e308, 1e308], Array(3)]) assert.throws(() => simulate('lorenz', initial, 1));
});
