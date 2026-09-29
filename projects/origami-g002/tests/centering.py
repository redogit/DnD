#!/usr/bin/env python3
"""Exercise the real centre command against an independent bisect interpolator.

These checks catch index-based interpolation, marker clamping, swapped axes or
indices, lost duplicate labels/core points, stateful requests, and weak validation.
"""
import argparse
import bisect
import copy
import json
import math
import pathlib
import subprocess

ROOT = pathlib.Path(__file__).resolve().parents[1]
parser = argparse.ArgumentParser()
parser.add_argument('--binary', default=str(ROOT / 'build/linux/origami'))
args = parser.parse_args()
checks = []


def check(name, condition):
    assert condition, name
    checks.append(name)


def invoke(data, raw=False):
    encoded = data if raw else json.dumps(data, ensure_ascii=True, separators=(',', ':'))
    return subprocess.run([args.binary, 'centre', '-'], input=encoded, text=True,
                          capture_output=True, timeout=30)


def interpolate(points, q):
    """Reference uses bisect rather than the RMAL evaluator's interval scan."""
    upper = bisect.bisect_left([point[0] for point in points], q)
    if points[upper][0] == q:
        return points[upper][1:]
    left, right = points[upper - 1], points[upper]
    weight = (q - left[0]) / (right[0] - left[0])
    return [(1 - weight) * a + weight * b for a, b in zip(left[1:], right[1:])]


def close(actual, expected):
    return math.isfinite(actual) and math.isclose(actual, expected, rel_tol=2e-13, abs_tol=2e-13)


def execute(request, title):
    result = invoke(request)
    assert result.returncode == 0, ('centre must execute', title, result.returncode, result.stderr)
    assert result.stderr == '', result.stderr
    out = json.loads(result.stdout)
    check(title + ': exact frame contract', set(out) == {'schema', 'q', 'markerQ', 'channels', 'paths', 'points', 'core'}
          and out['schema'] == 'origami-centering-frame-v1' and out['q'] == request['q'] and out['markerQ'] == 0.8)
    check(title + ': complete dimensions', len(out['channels']) == 6 and len(out['paths']) == 4 and len(out['points']) == 24)
    check(title + ': core retained exactly', out['core'] == request['core'])
    for index, source in enumerate(request['channels']):
        actual = out['channels'][index]
        expected = interpolate(source['points'], request['q'])[0]
        check(title + ': channel %d' % index, set(actual) == {'name', 'amplitude'}
              and actual['name'] == source['name'] and close(actual['amplitude'], expected))
        if request['q'] in [point[0] for point in source['points']]:
            check(title + ': exact channel knot %d' % index, actual['amplitude'] == expected)
    for index, source in enumerate(request['paths']):
        actual = out['paths'][index]
        expected = interpolate(source['points'], request['q'])
        check(title + ': path %d' % index, set(actual) == {'name', 'xy'} and actual['name'] == source['name']
              and len(actual['xy']) == 2 and all(close(a, b) for a, b in zip(actual['xy'], expected)))
        if request['q'] in [point[0] for point in source['points']]:
            check(title + ': exact path knot %d' % index, actual['xy'] == expected)
    for index, point in enumerate(out['points']):
        path, channel = divmod(index, 6)
        check(title + ': composition %d' % index, set(point) == {'pathIndex', 'channelIndex', 'xyz'}
              and point['pathIndex'] == path and point['channelIndex'] == channel
              and point['xyz'] == out['paths'][path]['xy'] + [out['channels'][channel]['amplitude']])
    return out, result.stdout


fixture = json.loads((ROOT / 'examples/centering.json').read_text(encoding='utf-8'))
baseline = json.dumps(fixture, sort_keys=True)
frames = {}
for q in [0, 0.5, 0.8, 1]:
    request = copy.deepcopy(fixture)
    request['q'] = q
    frames[q], _ = execute(request, 'source q=%s' % q)
check('source request retained without mutation', json.dumps(fixture, sort_keys=True) == baseline)
check('marker does not prematurely zero final channels', all(row['amplitude'] != 0 for row in frames[0.8]['channels'][3:]))
check('endpoint retains physical activity', all(row['amplitude'] != 0 for row in frames[1]['channels'][:3]))
check('endpoint final channels zero', all(row['amplitude'] == 0 for row in frames[1]['channels'][3:]))
check('endpoint paths retain radius .15', all(math.isclose(math.hypot(*row['xy']), .15, rel_tol=1e-13) for row in frames[1]['paths']))
check('repeated neutral labels retain distinct path positions', frames[0.5]['paths'][2]['name'] == frames[0.5]['paths'][3]['name']
      and frames[0.5]['paths'][2]['xy'] != frames[0.5]['paths'][3]['xy'])

postmarker = min(point[0] for point in fixture['channels'][3]['points'] if point[0] > .8)
check('source first postmarker knot is retained', postmarker == 0.8011444921316166)
request = copy.deepcopy(fixture)
request['q'] = postmarker
out, _ = execute(request, 'first postmarker knot')
check('first postmarker knot zeros final channels', all(row['amplitude'] == 0 for row in out['channels'][3:]))

path_points = fixture['paths'][0]['points']
gap_index = max(range(1, len(path_points)), key=lambda k: path_points[k][0] - path_points[k - 1][0])
for q in [(path_points[gap_index - 1][0] + path_points[gap_index][0]) / 2,
          path_points[gap_index][0], fixture['channels'][0]['points'][351][0],
          fixture['paths'][2]['points'][51][0]]:
    request = copy.deepcopy(fixture)
    request['q'] = q
    execute(request, 'irregular gap or exact source knot q=%s' % q)

# Hand-checkable nonuniform schedules differ by series, exposing any shared or
# index-derived q axis. At q=.5 channel 0 is 20/3, and path 0 is [3, 8].
small = {'schema': 'origami-centering-input-v1', 'q': .5,
         'channels': [{'name': 'channel %d' % i, 'points': [[0, i], [.25, 10 + i], [1, i]]} for i in range(6)],
         'paths': [{'name': 'same', 'points': [[0, i, -i], [.5, 3 + i, 8 - i], [1, -i, i]]} for i in range(4)],
         'core': [[-1e100, 1e100], [0, -0.0]]}
out, _ = execute(small, 'independent nonuniform series')
check('hand-checked nonuniform amplitude', math.isclose(out['channels'][0]['amplitude'], 20 / 3, rel_tol=1e-14))
check('hand-checked exact path knot', out['paths'][0]['xy'] == [3, 8])

seen = {}
for q in [.8, 0, 1, .5, .8, 1, 0, .5]:
    request = copy.deepcopy(small)
    request['q'] = q
    _, serialized = execute(request, 'out-of-order q=%s' % q)
    if q in seen:
        check('repeated request deterministic q=%s' % q, seen[q] == serialized)
    seen[q] = serialized

boundary = copy.deepcopy(small)
for source in boundary['channels']:
    source['name'] = '😀' * 128
    source['points'] = [[0, -1e100], [1, 1e100]]
for source in boundary['paths']:
    source['name'] = 'x' * 256
    source['points'] = [[0, -1e100, 1e100], [1, 1e100, -1e100]]
execute(boundary, 'numeric and UTF-16 boundaries')
maximum = copy.deepcopy(small)
for source in maximum['channels']:
    source['points'] = [[k / 1999, k % 2] for k in range(2000)]
for source in maximum['paths']:
    source['points'] = [[k / 1999, k % 2, -(k % 2)] for k in range(2000)]
maximum['core'] = [[k % 2, -(k % 2)] for k in range(2000)]
execute(maximum, 'maximum series and core counts')

invalid = [(name, value) for name, value in [('null root', None), ('array root', []), ('empty root', {})]]


def reject_mutation(name, mutate):
    request = copy.deepcopy(small)
    mutate(request)
    invalid.append((name, request))


for field in small:
    reject_mutation('missing ' + field, lambda request, key=field: request.pop(key))
reject_mutation('unknown root field', lambda r: r.update(extra=1))
reject_mutation('wrong schema', lambda r: r.update(schema='origami-centering-input-v2'))
for value in [None, True, '0.5', -0.001, 1.001, 1e101]:
    reject_mutation('invalid q %r' % value, lambda r, v=value: r.update(q=v))
for field, count, dimension in [('channels', 6, 2), ('paths', 4, 3)]:
    for value in [None, {}, [], small[field][:-1], small[field] + [small[field][0]]]:
        reject_mutation('invalid ' + field + ' count/type', lambda r, f=field, v=value: r.update({f: v}))
    for value in [None, {}, {'name': 'x'}, {'points': [[0, 0], [1, 0]]}]:
        reject_mutation('invalid ' + field + ' record', lambda r, f=field, v=value: r[f].__setitem__(0, v))
    reject_mutation('unknown ' + field + ' field', lambda r, f=field: r[f][0].update(extra=1))
    for value in ['', None, 1, 'x' * 257, '😀' * 129, 'bad\0', '\ud800']:
        reject_mutation('invalid ' + field + ' label', lambda r, f=field, v=value: r[f][0].update(name=v))
    for value in [None, {}, [], [[0] * dimension], [[k / 2000] + [0] * (dimension - 1) for k in range(2001)],
                  [[0] * (dimension - 1), [1] + [0] * (dimension - 1)],
                  [[0] * (dimension + 1), [1] + [0] * (dimension - 1)],
                  [[0] * dimension, [0] * dimension, [1] + [0] * (dimension - 1)],
                  [[0] * dimension, [.8] + [0] * (dimension - 1), [.5] + [0] * (dimension - 1), [1] + [0] * (dimension - 1)],
                  [[.1] + [0] * (dimension - 1), [1] + [0] * (dimension - 1)],
                  [[0] * dimension, [.9] + [0] * (dimension - 1)]]:
        reject_mutation('invalid ' + field + ' knots', lambda r, f=field, v=value: r[f][0].update(points=v))
    for column in range(dimension):
        for value in [None, True, '0', 1e101, -1e101]:
            reject_mutation('invalid ' + field + ' coordinate', lambda r, f=field, c=column, v=value: r[f][0]['points'][0].__setitem__(c, v))
for value in [None, {}, [], [[0, 0]], [[0, 0]] * 2001, [[0], [0, 0]], [[0, 0, 0], [0, 0]], [[None, 0], [0, 0]], [[True, 0], [0, 0]], [[1e101, 0], [0, 0]]]:
    reject_mutation('invalid core', lambda r, v=value: r.update(core=v))
for name, request in invalid:
    result = invoke(request)
    check(name + ': rejected without stdout', result.returncode == 1 and result.stdout == '' and 'error' in json.loads(result.stderr))

raw = json.dumps(small, separators=(',', ':'))
for name, malformed in [('duplicate key', raw.replace('"q":0.5', '"q":0.5,"q":0.6')),
                        ('trailing content', raw + ' trailing'), ('truncated JSON', raw[:-1]),
                        ('nonfinite number', raw.replace('"q":0.5', '"q":1e999')),
                        ('nonstandard NaN', raw.replace('"q":0.5', '"q":NaN')),
                        ('input byte budget', ' ' * (1024 * 1024) + raw)]:
    result = invoke(malformed, raw=True)
    check(name + ': rejected without stdout', result.returncode == 1 and result.stdout == '' and 'error' in json.loads(result.stderr))

print(json.dumps({'passed': len(checks), 'failed': 0, 'binary': args.binary,
                  'scope': 'Real RMAL interpolation/composition, exact knots/endpoints, marker behavior, duplicate-label indices, core retention, stateless requests, bounded contract rejection'}, indent=2))
