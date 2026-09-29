'use strict';

function shape(value, keys) {
  if (!value || typeof value !== 'object' || Array.isArray(value)
      || keys.some(key => !Object.hasOwn(value, key))
      || Object.keys(value).some(key => !keys.includes(key))) {
    throw new TypeError('Exploration input has missing or unsupported fields');
  }
}

function dataRecord(value) {
  if (!value || typeof value !== 'object' || Array.isArray(value)) {
    throw new TypeError('Exploration records must contain data properties');
  }
  const copy = Object.create(null);
  for (const key of Reflect.ownKeys(value)) {
    const descriptor = Object.getOwnPropertyDescriptor(value, key);
    if (typeof key !== 'string' || !descriptor || !Object.hasOwn(descriptor, 'value')) {
      throw new TypeError('Exploration records must contain data properties');
    }
    copy[key] = descriptor.value;
  }
  return copy;
}

function text(value, minimum, maximum) {
  if (typeof value !== 'string' || value.length < minimum || value.length > maximum
      || value.includes('\0') || !value.isWellFormed()) {
    throw new TypeError(`Expected valid Unicode text of ${minimum}–${maximum} UTF-16 code units without NUL`);
  }
}

function explore(input) {
  input = dataRecord(input);
  shape(input, ['home', 'operations']);
  text(input.home, 1, 256);
  if (!Array.isArray(input.operations) || input.operations.length > 512) {
    throw new RangeError('Exploration requires at most 512 operations');
  }
  // Validate the entire schedule before constructing any execution result.
  const operations = [];
  for (let i = 0; i < input.operations.length; i++) {
    const entry = Object.getOwnPropertyDescriptor(input.operations, String(i));
    if (!entry || !Object.hasOwn(entry, 'value')) {
      throw new TypeError('Operations must be a dense array of data properties');
    }
    const action = dataRecord(entry.value);
    if (action?.op === 'discover') {
      shape(action, ['op', 'id', 'note']);
      text(action.id, 1, 256);
      text(action.note, 0, 4096);
    } else if (action?.op === 'homeward' || action?.op === 'return_step') {
      shape(action, ['op']);
    } else {
      throw new TypeError('Operation must be discover, homeward or return_step');
    }
    operations.push(action);
  }
  const nodes = [{id: input.home, parentId: null, note: ''}];
  const byId = new Map([[input.home, nodes[0]]]);
  let activeId = input.home, frontierId = input.home, phase = 'exploring';
  const trace = [];
  function record(op, accepted, reason, attemptedId = null, attemptedNote = null) {
    trace.push({seq: trace.length, op, accepted, reason, phase, activeId, frontierId,
      retainedCount: nodes.length - 1, attemptedId, attemptedNote});
  }
  record('start', true, null);
  for (const action of operations) {
    let reason = null;
    if (action.op === 'discover') {
      if (phase !== 'exploring') reason = 'exploration_held';
      else if (byId.has(action.id)) reason = 'duplicate_id';
      else {
        const node = {id: action.id, parentId: activeId, note: action.note};
        nodes.push(node);
        byId.set(node.id, node);
        activeId = frontierId = node.id;
      }
    } else if (action.op === 'homeward') {
      if (phase === 'exploring') phase = activeId === input.home ? 'home' : 'interrupted';
    } else if (phase === 'exploring') reason = 'homeward_required';
    else if (phase !== 'home') {
      activeId = byId.get(activeId).parentId;
      phase = activeId === input.home ? 'home' : 'returning';
    }
    record(action.op, reason === null, reason,
      action.op === 'discover' ? action.id : null, action.op === 'discover' ? action.note : null);
  }
  return {schema: 'origami-exploration-v1', homeId: input.home, nodes,
    activeId, frontierId, phase, trace};
}

module.exports = {explore};
