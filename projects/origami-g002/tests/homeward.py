#!/usr/bin/env python3
"""Direct contract checks against the real RMAL/native explore command.

These checks fail if Homeward drops discoveries, allows further expansion,
skips ancestry, loses a rejected note, or accepts malformed contracts.
"""
import argparse
import json
import pathlib
import subprocess

ROOT = pathlib.Path(__file__).resolve().parents[1]
parser = argparse.ArgumentParser()
parser.add_argument('--binary', default=str(ROOT / 'build/linux/origami'))
args = parser.parse_args()
checks = []

def invoke(data, raw=False):
    encoded = data if raw else json.dumps(data, ensure_ascii=True)
    return subprocess.run([args.binary, 'explore', '-'], input=encoded,
                          text=True, capture_output=True, timeout=30)

def execute(operations, home='H'):
    request = {'home': home, 'operations': operations}
    preserved = json.dumps(request, sort_keys=True)
    result = invoke(request)
    assert result.returncode == 0, ('explore must execute', result.returncode, result.stderr)
    assert result.stderr == '', result.stderr
    assert json.dumps(request, sort_keys=True) == preserved
    output = json.loads(result.stdout)
    assert set(output) == {'schema', 'homeId', 'nodes', 'activeId', 'frontierId', 'phase', 'trace'}
    assert output['schema'] == 'origami-exploration-v1' and output['homeId'] == home
    assert len(output['trace']) == len(operations) + 1
    fields = {'seq', 'op', 'accepted', 'reason', 'phase', 'activeId', 'frontierId',
              'retainedCount', 'attemptedId', 'attemptedNote'}
    for seq, event in enumerate(output['trace']):
        assert set(event) == fields and event['seq'] == seq, event
        assert isinstance(event['accepted'], bool)
    return output

def discover(identifier, note=''):
    return {'op': 'discover', 'id': identifier, 'note': note}

def check(name, condition):
    assert condition, name
    checks.append(name)

empty = execute([])
check('initial home is retained', empty['nodes'] == [{'id': 'H', 'parentId': None, 'note': ''}])
check('initial trace is exact', empty['trace'] == [{'seq': 0, 'op': 'start', 'accepted': True,
      'reason': None, 'phase': 'exploring', 'activeId': 'H', 'frontierId': 'H',
      'retainedCount': 0, 'attemptedId': None, 'attemptedNote': None}])

ops = [discover('A', 'first'), discover('B', 'second'), {'op': 'homeward'},
       discover('C', 'preserve this interrupted attempt'), {'op': 'return_step'},
       {'op': 'homeward'}, {'op': 'return_step'}, discover('B', 'held before duplicate'),
       {'op': 'return_step'}]
out = execute(ops)
check('all committed discoveries and ancestry survive', out['nodes'] == [
    {'id': 'H', 'parentId': None, 'note': ''}, {'id': 'A', 'parentId': 'H', 'note': 'first'},
    {'id': 'B', 'parentId': 'A', 'note': 'second'}])
check('arrival preserves frontier', (out['activeId'], out['frontierId'], out['phase']) == ('H', 'B', 'home'))
check('Homeward interrupts without moving', (out['trace'][3]['phase'], out['trace'][3]['activeId']) == ('interrupted', 'B'))
check('blocked discovery retains exact attempted note', out['trace'][4]['reason'] == 'exploration_held'
      and not out['trace'][4]['accepted'] and out['trace'][4]['attemptedId'] == 'C'
      and out['trace'][4]['attemptedNote'] == ops[3]['note'])
check('one return step follows one parent', (out['trace'][5]['phase'], out['trace'][5]['activeId']) == ('returning', 'A'))
check('repeated Homeward does not restart return', out['trace'][6]['phase'] == 'returning' and out['trace'][6]['activeId'] == 'A')
check('arrival does not permit expansion', out['trace'][8]['reason'] == 'exploration_held' and not out['trace'][8]['accepted'])
check('return at home is accepted no-op', out['trace'][9]['accepted'] and out['trace'][9]['activeId'] == 'H')
check('retained counts never shrink', [t['retainedCount'] for t in out['trace']] == [0,1,2,2,2,2,2,2,2,2])
check('control operations have no attempted payload', all(t['attemptedId'] is None and t['attemptedNote'] is None for t in out['trace'] if t['op'] != 'discover'))

out = execute([{'op': 'return_step'}, discover('H', 'root collision'), discover('A'), discover('A', 'duplicate'), discover('B')])
check('return before Homeward is blocked', out['trace'][1]['reason'] == 'homeward_required' and not out['trace'][1]['accepted'])
check('root ID cannot be committed twice', out['trace'][2]['reason'] == 'duplicate_id')
check('committed duplicate cannot corrupt parent chain', out['trace'][4]['reason'] == 'duplicate_id' and out['nodes'][-1]['parentId'] == 'A')

out = execute([{'op': 'homeward'}, {'op': 'homeward'}, discover('x', 'kept'), {'op': 'return_step'}])
check('Homeward before discovery goes home', all(t['phase'] == 'home' for t in out['trace'][1:]))
check('root interruption preserves attempted discovery', out['trace'][3]['attemptedNote'] == 'kept' and len(out['nodes']) == 1)

for depth in [1, 3, 20, 255]:
    operations = [discover(str(k), 'n'+str(k)) for k in range(depth)] + [{'op': 'homeward'}] + [{'op': 'return_step'}] * depth
    out = execute(operations)
    check('depth %s complete return path' % depth, [t['activeId'] for t in out['trace'][depth+2:]] == [str(k) for k in range(depth-2, -1, -1)] + ['H'])
    check('depth %s retained lineage' % depth, len(out['nodes']) == depth+1 and out['frontierId'] == str(depth-1))

text = 'line\nquote" slash\\ tab\t snowman☃ astral😀'
out = execute([discover('__proto__', text), discover('constructor', text), discover('a.b', text), {'op': 'homeward'}, discover('toString', text)], home='H.😀')
check('string identities and notes remain exact', [n['id'] for n in out['nodes']] == ['H.😀','__proto__','constructor','a.b'] and all(n['note'] == text for n in out['nodes'][1:]) and out['trace'][-1]['attemptedNote'] == text)
out = execute([discover('😀'*128, '😀'*2048)])
check('UTF-16 limits admit exact boundary', out['nodes'][1]['note'] == '😀'*2048)
out = execute([{'op': 'homeward'}] * 512)
check('maximum operation count admitted', len(out['trace']) == 513)

invalid = [None, [], {}, {'home':'H'}, {'home':'','operations':[]}, {'home':1,'operations':[]},
    {'home':'x'*257,'operations':[]}, {'home':'😀'*129,'operations':[]}, {'home':'H','operations':None},
    {'home':'H','operations':[],'extra':1}, {'home':'H','operations':[{'op':'homeward','id':'x'}]},
    {'home':'H','operations':[{'op':'return_step','note':'x'}]}, {'home':'H','operations':[{'op':'unknown'}]},
    {'home':'H','operations':[{'op':'discover','id':'a'}]}, {'home':'H','operations':[{'op':'discover','id':'a','note':None}]},
    {'home':'H','operations':[discover('', '')]}, {'home':'H','operations':[discover('a','x'*4097)]},
    {'home':'H','operations':[discover('a','😀'*2049)]}, {'home':'H','operations':[discover('x'*257)]},
    {'home':'H','operations':[{'op':'homeward'}]*513}, {'home':'H','operations':[discover('a','nul\0')]},
    {'home':'H','operations':[discover('a','\ud800')]}, {'home':'H','operations':[3]}]
for index, request in enumerate(invalid):
    result = invoke(request)
    check('invalid schema rejected without output %d' % index, result.returncode != 0 and result.stdout == '' and 'error' in json.loads(result.stderr))
for raw in ['{"home":"H","home":"other","operations":[]}', '{"home":"H","operations":[]} trailing']:
    result = invoke(raw, raw=True)
    check('malformed JSON rejected without partial output', result.returncode != 0 and result.stdout == '')

print(json.dumps({'passed':len(checks), 'failed':0, 'binary':args.binary,
                  'scope':'Real RMAL/native exploration state, retained notes and ancestry, Homeward priority, contract rejection'}, indent=2))
