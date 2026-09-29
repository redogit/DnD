'use strict';
const test = require('node:test');
const assert = require('node:assert/strict');
const { spawnSync } = require('node:child_process');
const path = require('node:path');
const A = require('../index.cjs');

test('serialized transform contains enough information to recover without source input', () => {
  const input = Array.from({ length: 13 }, (_, i) => i / 7 - 1);
  const packet = A.encode13(input, [{ i: 0, j: 12, degrees: 37 }, { i: 0, j: 5, degrees: -71 }]);
  assert.deepEqual(Object.keys(packet).sort(), ['residual', 'rotations', 'schema', 'visible']);
  const restored = A.recover13(JSON.parse(JSON.stringify(packet)));
  restored.forEach((v, i) => assert.ok(Math.abs(v - input[i]) < 1e-12));
  assert.throws(() => A.recover13({ ...packet, residual: undefined }));
  assert.throws(() => A.recover13({ ...packet, admitted: true }));
});

test('accepts only dense finite vectors and supported transformation metadata', () => {
  assert.throws(() => A.encode13(Array(13)));
  assert.throws(() => A.encode13([...Array(12).fill(0), Infinity]));
  for (const r of [{ i: 1, j: 1, degrees: 2 }, { i: 0, j: 13, degrees: 2 }, { i: 0, j: 1, degrees: 361 }]) {
    assert.throws(() => A.encode13(Array(13).fill(0), [r]));
  }
  for (const feed of ['toString', '__proto__', 'unknown']) assert.throws(() => A.simulate(feed, [0, 0, 0], 0));
  assert.throws(() => A.simulate('lorenz', [0, 0, 0], -1));
  assert.throws(() => A.step('lorenz', [1e200, 1e200, 1e200]));
});

test('decision sufficiency intersects whole fibers and rejects unusable contracts', () => {
  const rows = [
    { id: 'a', observation: [0], actions: ['a', 'b'] },
    { id: 'b', observation: [0], actions: ['b', 'c'] },
    { id: 'c', observation: [0], actions: ['a', 'c'] }
  ];
  assert.equal(A.decisionSufficiency(rows).sufficient, false);
  assert.deepEqual(A.decisionSufficiency(rows).fibers[0].commonActions, []);
  assert.equal(A.decisionSufficiency(rows.map((r, i) => ({ ...r, observation: [i] }))).sufficient, true);
  assert.throws(() => A.decisionSufficiency([]));
  assert.throws(() => A.decisionSufficiency([{ id: 'a', observation: [0], actions: [] }]));
  assert.throws(() => A.decisionSufficiency([rows[0], rows[0]]));
});

test('command-line JSON pipeline runs without a browser', () => {
  const cli = path.join(__dirname, '../bin/origami.cjs');
  const input = Array.from({ length: 13 }, (_, i) => i);
  const run = (command, data) => spawnSync(process.execPath, [cli, command, '-'], {
    input: JSON.stringify(data), encoding: 'utf8'
  });
  const enc = run('encode', { vector: input, rotations: [{ i: 0, j: 12, degrees: 90 }] });
  assert.equal(enc.status, 0, enc.stderr);
  const dec = run('recover', JSON.parse(enc.stdout));
  assert.equal(dec.status, 0, dec.stderr);
  JSON.parse(dec.stdout).forEach((v, i) => assert.ok(Math.abs(v - input[i]) < 1e-12));
  const bad = run('encode', { vector: [1, 2, 3] });
  assert.equal(bad.status, 1);
  assert.equal(bad.stdout, '');
  assert.ok(JSON.parse(bad.stderr).error);
  const tooBig = spawnSync(process.execPath, [cli, 'encode', '-'], { input: ' '.repeat(1048577), encoding: 'utf8' });
  assert.equal(tooBig.status, 1);
  assert.match(tooBig.stderr, /1 MiB/);
});
