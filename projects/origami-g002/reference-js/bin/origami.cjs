#!/usr/bin/env node
'use strict';
const fs = require('node:fs');
const A = require('../index.cjs');
const LIMIT = 1024 * 1024;
const commands = new Set(['encode', 'recover', 'decide', 'simulate', 'geometry', 'rf', 'explore']);

function fields(value, required, optional = []) {
  if (!value || typeof value !== 'object' || Array.isArray(value)
    || required.some(k => !Object.hasOwn(value, k))
    || Object.keys(value).some(k => !required.includes(k) && !optional.includes(k))) {
    throw new TypeError('JSON input has missing or unsupported fields');
  }
}

async function readInput(filename) {
  const stream = filename === '-' ? process.stdin : fs.createReadStream(filename);
  const chunks = [];
  let size = 0;
  for await (const chunk of stream) {
    size += chunk.length;
    if (size > LIMIT) throw new RangeError('JSON input exceeds 1 MiB');
    chunks.push(chunk);
  }
  return JSON.parse(new TextDecoder('utf-8', {fatal: true, ignoreBOM: true}).decode(Buffer.concat(chunks)));
}

async function main() {
  const [command, filename = '-', ...extra] = process.argv.slice(2);
  if (command === '--help' && filename === '-' && !extra.length) {
    process.stdout.write('Usage: node bin/origami.cjs <encode|recover|decide|simulate|geometry|rf|explore> [input.json|-]\nJSON results go to stdout; errors to stderr. Rotation indices are 0..12.\n');
    return;
  }
  if (!commands.has(command) || extra.length) throw new TypeError('Use --help for usage');
  const data = await readInput(filename);
  let result;
  switch (command) {
    case 'explore': result = A.explore(data); break;
    case 'encode':
      fields(data, ['vector'], ['rotations']);
      result = A.encode13(data.vector, data.rotations);
      break;
    case 'recover': result = A.recover13(data); break;
    case 'decide': result = A.decisionSufficiency(data); break;
    case 'simulate':
      fields(data, ['feed', 'initial', 'steps']);
      result = A.simulate(data.feed, data.initial, data.steps);
      break;
    case 'geometry':
      fields(data, ['fold'], ['n', 'a', 'b', 'gamma']);
      result = A.miura(data.fold, data.n, data.a, data.b, data.gamma);
      break;
    case 'rf':
      fields(data, ['frequency'], ['options']);
      result = A.circuit(data.frequency, data.options);
      break;
  }
  process.stdout.write(JSON.stringify(result, null, 2) + '\n');
}

main().catch(error => {
  process.stderr.write(JSON.stringify({ error: error.message }) + '\n');
  process.exitCode = 1;
});
