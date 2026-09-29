#!/usr/bin/env python3
"""Real bounded runner checks: literals catch transition/priority/lineage bugs.

Sampler parity and hand-computed values catch storage loss or axis swaps. The
accepted maximum catches accidental per-frame allocation and unbounded sampling.
"""
import argparse
import copy
import hashlib
import json
import math
import pathlib
import subprocess

ROOT = pathlib.Path(__file__).resolve().parents[1]
p = argparse.ArgumentParser()
p.add_argument('--binary', default=str(ROOT / 'build/linux/origami'))
args = p.parse_args()
checks = []


def check(name, condition):
    assert condition, name
    checks.append(name)


def invoke(request, command='centre-run', raw=False):
    encoded = request if raw else json.dumps(request, ensure_ascii=True, separators=(',', ':'))
    return subprocess.run([args.binary, command, '-'], input=encoded, text=True,
                          capture_output=True, timeout=90)


def execute(request, title):
    r = invoke(request)
    assert r.returncode == 0, ('centre-run must execute', title, r.returncode, r.stderr)
    check(title + ': stderr empty', r.stderr == '')
    out = json.loads(r.stdout)
    check(title + ': exact run envelope', set(out) == {'schema', 'runId', 'homeFrameId', 'core', 'frames', 'trace', 'activeFrameId', 'frontierFrameId', 'phase', 'atHome'}
          and out['schema'] == 'origami-centering-run-v1' and out['runId'] == request['runId'] and out['homeFrameId'] == 0)
    check(title + ': core preserved once', out['core'] == request['core'] and all('core' not in f for f in out['frames']))
    for frame in out['frames']:
        check(title + ': frame keys ' + str(frame['id']), set(frame) == {'id', 'parentId', 'q', 'note', 'channels', 'paths', 'points'})
        check(title + ': frame dimensions ' + str(frame['id']), len(frame['channels']) == 6 and len(frame['paths']) == 4 and len(frame['points']) == 24)
    ids = [frame['id'] for frame in out['frames']]
    check(title + ': stable contiguous IDs', ids == list(range(len(ids))))
    check(title + ': active and frontier resolve', out['activeFrameId'] in ids and out['frontierFrameId'] in ids)
    check(title + ': Home is root identity', out['atHome'] == (out['activeFrameId'] == 0))
    check(title + ': one event per operation', len(out['trace']) == len(request['operations']) + 1)
    for seq, event in enumerate(out['trace']):
        check(title + ': complete event ' + str(seq), set(event) == {'seq', 'op', 'accepted', 'reason', 'phase', 'atHome', 'activeFrameId', 'frontierFrameId', 'retainedCount', 'attemptedQ', 'attemptedNote', 'homewardRequested'}
              and event['seq'] == seq and event['activeFrameId'] in ids and event['frontierFrameId'] in ids
              and event['atHome'] == (event['activeFrameId'] == 0))
    return out, r.stdout


small = {'schema': 'origami-centering-run-input-v1', 'runId': 'literal-run',
         'channels': [{'name': 'c%d' % i, 'points': [[0, i], [.25, 10 + i], [1, i]]} for i in range(6)],
         'paths': [{'name': 'same', 'points': [[0, i, -i], [.5, 3 + i, 8 - i], [1, -i, i]]} for i in range(4)],
         'core': [[.15, 0], [0, .15]], 'operations': []}
start, _ = execute(small, 'empty run')
check('start literal state', start['trace'] == [{'seq': 0, 'op': 'start', 'accepted': True, 'reason': None,
      'phase': 'exploring', 'atHome': True, 'activeFrameId': 0, 'frontierFrameId': 0, 'retainedCount': 1,
      'attemptedQ': None, 'attemptedNote': None, 'homewardRequested': False}])
check('start literal frame', start['frames'][0]['id'] == 0 and start['frames'][0]['parentId'] is None
      and start['frames'][0]['q'] == 1 and start['frames'][0]['note'] == '' and start['phase'] == 'exploring')

request = copy.deepcopy(small)
request['operations'] = [
    {'op': 'return_step'},
    {'op': 'advance', 'q': 1, 'note': 'equal is rejected'},
    {'op': 'advance', 'q': .5, 'note': 'retain A', 'homeward': False},
    {'op': 'advance', 'q': .75, 'note': 'inward is rejected'},
    {'op': 'advance', 'q': .25, 'note': 'retain B'},
    {'op': 'advance', 'q': 0, 'note': 'priority retains this attempt', 'homeward': True},
    {'op': 'advance', 'q': 0, 'note': 'held attempt survives'},
    {'op': 'homeward'}, {'op': 'return_step'}, {'op': 'homeward'},
    {'op': 'return_step'}, {'op': 'return_step'},
    {'op': 'advance', 'q': 0, 'note': 'no resume at Home'},
]
out, serialized = execute(request, 'priority and retained return')
# Each tuple was derived from the public contract, not a reference state machine.
expected = [
    (True, None, 'exploring', 0, 0, 1, False),
    (False, 'homeward_required', 'exploring', 0, 0, 1, False),
    (False, 'not_outward', 'exploring', 0, 0, 1, False),
    (True, None, 'exploring', 1, 1, 2, False),
    (False, 'not_outward', 'exploring', 1, 1, 2, False),
    (True, None, 'exploring', 2, 2, 3, False),
    (False, 'homeward_override', 'returning', 2, 2, 3, True),
    (False, 'exploration_held', 'returning', 2, 2, 3, False),
    (True, None, 'returning', 2, 2, 3, True),
    (True, None, 'returning', 1, 2, 3, False),
    (True, None, 'returning', 1, 2, 3, True),
    (True, None, 'home', 0, 2, 3, False),
    (True, None, 'home', 0, 2, 3, False),
    (False, 'exploration_held', 'home', 0, 2, 3, False),
]
for seq, want in enumerate(expected):
    e = out['trace'][seq]
    check('literal transition %d' % seq, tuple(e[k] for k in ['accepted', 'reason', 'phase', 'activeFrameId', 'frontierFrameId', 'retainedCount', 'homewardRequested']) == want)
    operation = request['operations'][seq - 1] if seq else {'op': 'start'}
    check('attempt retained %d' % seq, e['op'] == operation['op'] and e['attemptedQ'] == operation.get('q') and e['attemptedNote'] == operation.get('note'))
check('frame notes and lineage stable', [(f['id'], f['parentId'], f['q'], f['note']) for f in out['frames']] == [(0, None, 1, ''), (1, 0, .5, 'retain A'), (2, 1, .25, 'retain B')])
check('literal interpolated frame', math.isclose(out['frames'][1]['channels'][0]['amplitude'], 20 / 3, rel_tol=1e-14) and out['frames'][1]['paths'][0]['xy'] == [3, 8])
check('literal knot frame', out['frames'][2]['channels'][0]['amplitude'] == 10 and out['frames'][2]['paths'][0]['xy'] == [1.5, 4])
check('final frontier survives Home', (out['phase'], out['atHome'], out['activeFrameId'], out['frontierFrameId']) == ('home', True, 0, 2))
check('replay byte repeatable', invoke(request).stdout == serialized)

# Returning must preserve the full serialized retained-frame array, notes, and
# lineage even when the active cursor moves and additional attempts are held.
prefix = copy.deepcopy(request)
prefix['operations'] = prefix['operations'][:5]
before, _ = execute(prefix, 'before interruption')
check('all frame bytes retained through return', json.dumps(before['frames'], separators=(',', ':')) == json.dumps(out['frames'], separators=(',', ':')))
for frame in out['frames']:
    sample = {k: copy.deepcopy(small[k]) for k in ['channels', 'paths', 'core']}
    sample.update(schema='origami-centering-input-v1', q=frame['q'])
    result = invoke(sample, command='centre')
    assert result.returncode == 0, result.stderr
    centre = json.loads(result.stdout)
    check('shared sampler exact parity frame %s' % frame['id'], all(frame[k] == centre[k] for k in ['channels', 'paths', 'points']))

for operations, title, want_phase, want_reason in [
    ([{'op': 'homeward'}, {'op': 'homeward'}, {'op': 'return_step'}], 'Home idempotence', 'home', None),
    ([{'op': 'advance', 'q': 1, 'note': '', 'homeward': True}], 'override wins over equal q at Home', 'home', 'homeward_override'),
    ([{'op': 'advance', 'q': 0, 'note': ''}, {'op': 'advance', 'q': 0, 'note': ''}], 'zero outward boundary', 'exploring', 'not_outward'),
    ([{'op': 'advance', 'q': .5, 'note': ''}, {'op': 'advance', 'q': 1, 'note': '', 'homeward': True}], 'override wins over inward q', 'returning', 'homeward_override'),
    ([{'op': 'homeward'}, {'op': 'advance', 'q': 0, 'note': '', 'homeward': True}], 'override remains priority when held', 'home', 'homeward_override'),
]:
    req = copy.deepcopy(small); req['operations'] = operations
    result, _ = execute(req, title)
    check(title + ': literal end state', result['phase'] == want_phase and result['trace'][-1]['reason'] == want_reason)

fixture = json.loads((ROOT / 'examples/centering-run.json').read_text())
demo, _ = execute(fixture, 'approved source demo')
check('demo literal cursor trace', [e['activeFrameId'] for e in demo['trace']] == [0, 1, 2, 3, 4, 4, 4, 3, 2, 1, 0, 0])
check('approved Home literal physical channels', [row['amplitude'] for row in demo['frames'][0]['channels']] == [-.365158415444656, -.2091851976606801, .019862427962165363, 0, 0, 0])
check('demo retained five full frames', len(demo['frames']) == 5 and demo['frontierFrameId'] == 4 and demo['phase'] == 'home')
for frame in demo['frames']:
    sample = {k: copy.deepcopy(fixture[k]) for k in ['channels', 'paths', 'core']}
    sample.update(schema='origami-centering-input-v1', q=frame['q'])
    r = invoke(sample, command='centre'); assert r.returncode == 0, r.stderr
    check('approved frame sampler parity %s' % frame['id'], all(frame[k] == json.loads(r.stdout)[k] for k in ['channels', 'paths', 'points']))
chord = copy.deepcopy(fixture)
chord['operations'] = [{'op': 'advance', 'q': .9986829314769925, 'note': 'inside radius remains away'}]
result, _ = execute(chord, 'approved chord enters core radius')
check('chord actually below core radius', min(math.hypot(*row['xy']) for row in result['frames'][1]['paths']) < .15)
check('radial threshold cannot declare Home', result['activeFrameId'] == 1 and result['phase'] == 'exploring' and result['atHome'] is False)
chord['operations'] += [{'op': 'homeward'}, {'op': 'return_step'}]
returned, _ = execute(chord, 'return from inside core radius')
check('inside chord retained on return to identity Home', returned['activeFrameId'] == 0 and returned['atHome'] is True and returned['phase'] == 'home' and returned['frames'] == result['frames'])

boundary = copy.deepcopy(small)
boundary['runId'] = '😀' * 128
boundary['operations'] = [{'op': 'advance', 'q': 0, 'note': '😀' * 2048}]
result, _ = execute(boundary, 'UTF16 limits')
check('Unicode note retained exactly', result['frames'][1]['note'] == boundary['operations'][0]['note'] and result['trace'][1]['attemptedNote'] == boundary['operations'][0]['note'])
maximum = copy.deepcopy(small)
maximum['operations'] = [{'op': 'advance', 'q': (127 - k) / 128, 'note': ''} for k in range(128)]
for series in maximum['channels']:
    series['points'] = [[k / 1999, k % 2] for k in range(2000)]
for series in maximum['paths']:
    series['points'] = [[k / 1999, k % 2, -(k % 2)] for k in range(2000)]
maximum['core'] = [[k % 2, -(k % 2)] for k in range(2000)]
result, _ = execute(maximum, '128 operations and maximum curves')
check('maximum every advance retained', len(result['frames']) == 129 and result['activeFrameId'] == 128 and result['frames'][128]['q'] == 0 and result['frames'][128]['parentId'] == 127)

invalid = [('null root', None), ('array root', []), ('empty root', {})]
def invalid_mutation(name, mutate):
    req = copy.deepcopy(small); mutate(req); invalid.append((name, req))
for key in small:
    invalid_mutation('missing ' + key, lambda r, key=key: r.pop(key))
for key, value in [('schema', 'origami-centering-input-v1'), ('extra', 1), ('runId', ''), ('runId', 1), ('runId', None), ('runId', 'x' * 257), ('runId', '😀' * 129), ('runId', '\ud800'), ('runId', 'bad\0')]:
    invalid_mutation('invalid root ' + key + repr(value), lambda r, key=key, value=value: r.update({key: value}))
for value in [None, {}, '', [None], [[]], [{}], [{'op': 'unknown'}], [{'op': 'advance'}], [{'op': 'advance', 'q': .5}], [{'op': 'advance', 'note': ''}], [{'op': 'homeward', 'q': .5}], [{'op': 'return_step', 'note': ''}], [{'op': 'return_step', 'homeward': True}], [{'op': 'homeward', 'homeward': True}], [{'op': 'advance', 'q': .5, 'note': '', 'extra': 1}], [{'op': 'homeward'}] * 129]:
    invalid_mutation('invalid operations ' + repr(value)[:100], lambda r, value=value: r.update(operations=value))
for key, values in [('q', [None, True, '0.5', -.001, 1.001, 1e101]), ('note', [None, True, 1, 'x' * 4097, '😀' * 2049, '\ud800', 'bad\0']), ('homeward', [None, 0, 1, 'true', [], {}])]:
    for value in values:
        operation = {'op': 'advance', 'q': .5, 'note': ''}; operation[key] = value
        invalid_mutation('invalid advance ' + key + repr(value)[:60], lambda r, operation=operation: r.update(operations=[operation]))
for key, value in [('channels', []), ('paths', []), ('core', []), ('core', [[0, 0]]), ('core', [[1e101, 0], [0, 0]])]:
    invalid_mutation('invalid curve container ' + key, lambda r, key=key, value=value: r.update({key: value}))
for field, dim in [('channels', 2), ('paths', 3)]:
    for points in [[[0] * dim, [0] * dim, [1] + [0] * (dim - 1)], [[.1] + [0] * (dim - 1), [1] + [0] * (dim - 1)], [[0] * dim, [.9] + [0] * (dim - 1)], [[0] * (dim + 1), [1] + [0] * (dim - 1)]]:
        invalid_mutation('invalid ' + field + ' knot schedule', lambda r, field=field, points=points: r[field][0].update(points=points))
    invalid_mutation('invalid ' + field + ' label', lambda r, field=field: r[field][0].update(name=''))
    invalid_mutation('invalid ' + field + ' unknown field', lambda r, field=field: r[field][0].update(extra=1))
for name, req in invalid:
    r = invoke(req)
    check(name + ': atomic rejection', r.returncode == 1 and r.stdout == '' and 'error' in json.loads(r.stderr))
raw = json.dumps(small, separators=(',', ':'))
for title, malformed in [('duplicate key', raw.replace('"operations":[]', '"operations":[],"operations":[]')),
                         ('truncated', raw[:-1]), ('trailing content', raw + ' trailing'),
                         ('nonfinite q', raw.replace('"operations":[]', '"operations":[{"op":"advance","q":1e999,"note":""}]')),
                         ('input budget', ' ' * (1024 * 1024) + raw)]:
    r = invoke(malformed, raw=True)
    check(title + ': atomic rejection', r.returncode == 1 and r.stdout == '' and 'error' in json.loads(r.stderr))
print(json.dumps({'passed': len(checks), 'failed': 0, 'binary': args.binary,
                  'binarySHA256': hashlib.sha256(pathlib.Path(args.binary).read_bytes()).hexdigest(),
                  'scope': 'Literal runner transitions; same-tick priority; lineage, notes and full frame retention; deterministic replay; sampler parity; identity-based Home; strict validation; maximum operation and curve budgets'}, indent=2))
