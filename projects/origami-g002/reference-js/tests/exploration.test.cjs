'use strict';
const test = require('node:test');
const assert = require('node:assert/strict');
const { spawnSync } = require('node:child_process');
const { explore } = require('../index.cjs');
const discover = (id, note = id) => ({op: 'discover', id, note});
const homeward = {op: 'homeward'}, back = {op: 'return_step'};

test('Homeward returns along retained ancestry and preserves a blocked discovery', () => {
  const input = {home:'H', operations:[discover('A'),discover('B'),discover('C'),homeward,discover('X','unfinished'),back,homeward,back,back,back,discover('Y')]};
  const before = structuredClone(input), out = explore(input);
  assert.deepEqual(input, before);
  assert.equal(out.schema, 'origami-exploration-v1');
  assert.deepEqual(out.nodes, [{id:'H',parentId:null,note:''},{id:'A',parentId:'H',note:'A'},{id:'B',parentId:'A',note:'B'},{id:'C',parentId:'B',note:'C'}]);
  assert.equal(out.activeId,'H'); assert.equal(out.frontierId,'C'); assert.equal(out.phase,'home');
  assert.deepEqual(out.trace.map(e=>e.activeId), ['H','A','B','C','C','C','B','B','A','H','H','H']);
  assert.deepEqual(out.trace.map(e=>e.phase), ['exploring','exploring','exploring','exploring','interrupted','interrupted','returning','returning','returning','home','home','home']);
  assert.deepEqual(out.trace[5], {seq:5,op:'discover',accepted:false,reason:'exploration_held',phase:'interrupted',activeId:'C',frontierId:'C',retainedCount:3,attemptedId:'X',attemptedNote:'unfinished'});
  assert.equal(out.trace[11].reason,'exploration_held');
  for (const e of out.trace.slice(3)) assert.equal(e.retainedCount,3);
  input.operations[0].note = 'changed'; assert.equal(out.nodes[1].note,'A');
});

test('Homeward at Home is idempotent in state; unrequested return is blocked', () => {
  const out=explore({home:'H',operations:[back,homeward,homeward,back,discover('A')]});
  assert.equal(out.trace[1].reason,'homeward_required');
  assert.deepEqual(out.nodes,[{id:'H',parentId:null,note:''}]);
  assert.equal(out.phase,'home');
  assert.ok(out.trace.slice(2,5).every(e=>e.accepted && e.activeId==='H' && e.phase==='home'));
  assert.equal(out.trace[5].reason,'exploration_held');
});

test('unique IDs include Home and prototype-like strings without corrupting lineage', () => {
  const out=explore({home:'H',operations:[discover('H'),discover('__proto__'),discover('constructor'),discover('__proto__'),homeward,discover('constructor')]});
  assert.equal(out.trace[1].reason,'duplicate_id'); assert.equal(out.trace[4].reason,'duplicate_id');
  assert.equal(out.trace[6].reason,'exploration_held');
  assert.equal(out.nodes[2].parentId,'__proto__');
  assert.equal(out.trace[4].attemptedNote,'__proto__');
});

test('Unicode and string notes survive exactly; a full bounded path remains connected', () => {
  const note='00123 / \\ "\n\t Ω 🌌 9007199254740993';
  const text=explore({home:'家',operations:[discover('🌌',note),homeward,back]});
  assert.equal(text.nodes[1].note,note); assert.equal(text.trace[1].attemptedNote,note);
  const operations=Array.from({length:255},(_,i)=>discover(String(i)));
  operations.push(homeward,...Array(255).fill(back),back);
  const out=explore({home:'H',operations});
  assert.equal(out.nodes.length,256);assert.equal(out.trace.length,513);assert.equal(out.activeId,'H');
  out.nodes.slice(2).forEach((n,i)=>assert.equal(n.parentId,String(i)));
  const max=explore({home:'🌌'.repeat(128),operations:[discover('X','x'.repeat(4096))]});
  assert.equal(max.nodes[1].note.length,4096);
});

test('the complete schedule is validated; malformed shapes and limits are rejected', () => {
  for (const input of [null,[],{}, {home:'H',operations:[],extra:1}, {home:'',operations:[]}, {home:'x'.repeat(257),operations:[]}, {home:'\ud800',operations:[]}, {home:'H\0',operations:[]}, {home:'H',operations:Array(513).fill(back)}, {home:'H',operations:Array(1)}]) assert.throws(()=>explore(input));
  for (const bad of [null,{}, {op:'unknown'}, {op:'homeward',id:'A'}, {op:'return_step',note:''}, {op:'discover',id:'A'}, discover('', ''), discover('A',null), discover('A','x'.repeat(4097)), discover('A','\udfff'), discover('A','\0')]) assert.throws(()=>explore({home:'H',operations:[discover('valid'),bad]}));
});

test('CLI explore emits valid JSON or a failure with empty stdout', () => {
  const cli=require.resolve('../bin/origami.cjs');
  const run=input=>spawnSync(process.execPath,[cli,'explore','-'],{input:JSON.stringify(input),encoding:'utf8'});
  const good=run({home:'H',operations:[discover('A'),homeward,back]});
  assert.equal(good.status,0,good.stderr); assert.equal(JSON.parse(good.stdout).activeId,'H');
  const bad=run({home:'H',operations:[discover('A'),{op:'bad'}]});
  assert.equal(bad.status,1);assert.equal(bad.stdout,'');assert.ok(JSON.parse(bad.stderr).error);
});

test('programmatic inputs cannot replace validated operations or bypass the bound', () => {
  const operations=[];
  operations[Symbol.iterator]=function*(){for(let i=0;i<600;i++) yield discover(String(i));};
  assert.equal(explore({home:'H',operations}).trace.length,1);
  let reads=0;
  const dynamic={get op(){reads++;return reads===1?'homeward':'unknown';}};
  assert.throws(()=>explore({home:'H',operations:[dynamic]}));
  assert.equal(reads,0);
  const root={operations:[]}; Object.defineProperty(root,'home',{get(){throw Error('getter executed');}});
  assert.throws(()=>explore(root),/data properties/);
});

test('CLI rejects malformed UTF-8 and BOM input without changing identifiers', () => {
  const cli=require.resolve('../bin/origami.cjs');
  const invalid=Buffer.concat([Buffer.from('{"home":"'),Buffer.from([255]),Buffer.from('","operations":[]}')]);
  const bom=Buffer.concat([Buffer.from([239,187,191]),Buffer.from('{"home":"H","operations":[]}')]);
  for (const input of [invalid,bom]) {
    const out=spawnSync(process.execPath,[cli,'explore','-'],{input,encoding:'utf8'});
    assert.equal(out.status,1);assert.equal(out.stdout,'');assert.ok(JSON.parse(out.stderr).error);
  }
});
