'use strict';

const Model = require('./lib/model.js');
const Engineering = require('./lib/engineering.js');

const SCHEMA = 'origami-transform-v1';
const MAX_ROTATIONS = 4096;
const MAX_STEPS = 1000000;
const TIMESTEPS = Object.freeze({ lorenz: 0.01, rossler: 0.02, clifford: null });

function record(value, required, optional = [], name = 'Input') {
  if (!value || typeof value !== 'object' || Array.isArray(value)) throw new TypeError(`${name} must be an object`);
  if (required.some(k => !Object.hasOwn(value, k)) || Object.keys(value).some(k => !required.includes(k) && !optional.includes(k))) {
    throw new TypeError(`${name} has missing or unsupported fields`);
  }
}

function vector(value, length, name = 'Vector') {
  if (!Array.isArray(value) || value.length !== length) throw new TypeError(`${name} requires ${length} coordinates`);
  // Array.from visits holes, unlike Array.every/map on sparse arrays.
  const copy = Array.from(value);
  if (!copy.every(Number.isFinite) || !Number.isFinite(Math.hypot(...copy))) {
    throw new TypeError(`${name} coordinates and norm must be finite`);
  }
  return copy;
}

function rotationList(rotations) {
  if (!Array.isArray(rotations) || rotations.length > MAX_ROTATIONS) throw new RangeError(`At most ${MAX_ROTATIONS} rotations are supported`);
  return Array.from(rotations, r => {
    record(r, ['i', 'j', 'degrees'], [], 'Rotation');
    if (!Number.isInteger(r.i) || !Number.isInteger(r.j) || r.i < 0 || r.i > 12 || r.j < 0 || r.j > 12 || r.i === r.j) {
      throw new RangeError('Rotation indices must be distinct integers from 0 to 12');
    }
    if (!Number.isFinite(r.degrees) || Math.abs(r.degrees) > 360) throw new RangeError('Rotation degrees must be between -360 and 360');
    return { i: r.i, j: r.j, degrees: r.degrees };
  });
}

function rotate13(input, i, j, degrees) {
  const v = vector(input, 13);
  rotationList([{ i, j, degrees }]);
  return vector(Engineering.rotate13(v, i, j, degrees), 13, 'Rotated vector');
}

function project13(input) {
  return Engineering.project13(vector(input, 13));
}

/** Apply ordered orthogonal plane rotations, then split into 3 + 10 coordinates. */
function encode13(input, rotations = []) {
  let current = vector(input, 13);
  const history = rotationList(rotations);
  for (const r of history) current = rotate13(current, r.i, r.j, r.degrees);
  return { schema: SCHEMA, visible: current.slice(0, 3), residual: current.slice(3), rotations: history };
}

/** The residual and ordered rotation history are required for reconstruction. */
function recover13(packet) {
  record(packet, ['schema', 'visible', 'residual', 'rotations'], [], 'Transform packet');
  if (packet.schema !== SCHEMA) throw new TypeError('Unsupported transform schema');
  let current = vector(packet.visible, 3, 'Visible coordinates').concat(vector(packet.residual, 10, 'Residual coordinates'));
  current = vector(current, 13);
  const history = rotationList(packet.rotations);
  for (let k = history.length - 1; k >= 0; k--) {
    const r = history[k];
    current = rotate13(current, r.i, r.j, -r.degrees);
  }
  return current;
}

/** Exact finite-sample criterion: every observation fiber has a common action. */
function decisionSufficiency(rows) {
  if (!Array.isArray(rows) || rows.length < 1 || rows.length > 10000) throw new RangeError('Supply between 1 and 10000 states');
  const groups = new Map();
  const ids = new Set();
  let dimension;
  for (const row of rows) {
    record(row, ['id', 'observation', 'actions'], [], 'Decision state');
    if (typeof row.id !== 'string' || !row.id.length || row.id.length > 256 || ids.has(row.id)) throw new TypeError('State IDs must be unique nonempty strings of at most 256 characters');
    ids.add(row.id);
    if (!Array.isArray(row.observation) || row.observation.length > 1024) throw new TypeError('Observation must be an array of at most 1024 finite coordinates');
    const observation = vector(row.observation, row.observation.length, 'Observation');
    dimension ??= observation.length;
    if (observation.length !== dimension) throw new TypeError('All observations must have the same dimension');
    if (!Array.isArray(row.actions) || !row.actions.length || row.actions.length > 1000) throw new TypeError('Each state requires 1 to 1000 admissible actions');
    const actions = new Set();
    for (const action of row.actions) {
      if (typeof action !== 'string' || !action.length || action.length > 256) throw new TypeError('Actions must be nonempty strings of at most 256 characters');
      actions.add(action);
    }
    // JSON numbers are compared exactly after parsing; +0 and -0 share a fiber.
    const key = JSON.stringify(observation);
    if (!groups.has(key)) groups.set(key, { observation, ids: [], commonActions: [...actions] });
    const fiber = groups.get(key);
    fiber.ids.push(row.id);
    fiber.commonActions = fiber.commonActions.filter(action => actions.has(action));
  }
  const fibers = [...groups.values()];
  return { sufficient: fibers.every(f => f.commonActions.length > 0), fibers };
}

function checkFeed(feed) {
  if (typeof feed !== 'string' || !Object.hasOwn(TIMESTEPS, feed)) throw new RangeError('Feed must be lorenz, rossler or clifford');
}

function step(feed, state) {
  checkFeed(feed);
  return vector(Model.step(feed, vector(state, 3)), 3, 'Next stream state');
}

function simulate(feed, initial, steps) {
  checkFeed(feed);
  if (!Number.isSafeInteger(steps) || steps < 0 || steps > MAX_STEPS) throw new RangeError(`Steps must be an integer from 0 to ${MAX_STEPS}`);
  let state = vector(initial, 3, 'Initial stream state');
  for (let i = 0; i < steps; i++) state = step(feed, state);
  return { feed, steps, numericalDt: TIMESTEPS[feed], state };
}

function miura(...args) {
  const result = Engineering.miura(...args);
  result.vertices.forEach(v => vector(v, 3, 'Fold vertex'));
  return result;
}

function circuit(frequency, options = {}) {
  record(options, [], ['capacitance', 'inductance', 'crease', 'resistance', 'delay'], 'Circuit options');
  return Engineering.circuit(frequency, options);
}

module.exports = Object.freeze({
  explore: require('./lib/exploration.cjs').explore,
  SCHEMA, MAX_ROTATIONS, MAX_STEPS,
  encode13, recover13, rotate13, project13, decisionSufficiency,
  step, simulate, miura, circuit, resonance: Engineering.resonance,
  sourceInterpolation: Model.targets
});
