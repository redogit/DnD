#!/usr/bin/env python3
"""Host rejection/atomic-output probes; pass the compiled executable as argv[1]."""
import json
import subprocess
import sys

exe = sys.argv[1]
checks = 0

def run(command, payload, accepted=False):
    global checks
    result = subprocess.run([exe, command, '-'], input=payload, capture_output=True)
    assert (result.returncode == 0) == accepted, (command, payload[:100], result.returncode, result.stderr)
    if accepted:
        json.loads(result.stdout)
        assert not result.stderr, result.stderr
    else:
        assert not result.stdout, ('partial stdout', result.stdout[:100])
        assert json.loads(result.stderr)['error']
    checks += 1

assert subprocess.run([exe, '--help'], capture_output=True).returncode == 0
assert subprocess.run([exe, '--version'], capture_output=True).returncode == 0
assert subprocess.run([exe], capture_output=True).returncode == 0
checks += 3
zero = ','.join(['0'] * 13)
run('encode', ('{"vector":[' + zero + ']}').encode(), True)
run('decide', b'[{"id":"\\ud83d\\ude00","observation":[],"actions":["quote\\\" slash\\\\ newline\\n"]}]', True)
for command, payload in [
    ('encode', b'{"vector":[0],"vector":[0]}'),
    ('encode', b'{"vector":[],"\\u0076ector":[]}'),
    ('encode', b'{"vector":[[0]}'),
    ('encode', b'{"vector":[0],"unexpected":1}'),
    ('encode', ('{"vector":[' + zero + '],"rotations":[{"i":0,"j":0,"degrees":2}]}').encode()),
    ('encode', ('{"vector":[' + zero + '],"rotations":null}').encode()),
    ('encode', ('{"vector":[1e999,' + ','.join(['0'] * 12) + ']}').encode()),
    ('encode', b'{"vector":[01]}'),
    ('encode', b'{"vector":[+1]}'),
    ('encode', b'{"vector":[.1]}'),
    ('encode', b'{"vector":[1.]}'),
    ('encode', b'{"vector":[' + b'[' * 65 + b'0' + b']' * 65 + b']}'),
    ('encode', b'{"vector":[]} trailing'),
    ('encode', b' ' * (1024 * 1024 + 1)),
    ('decide', b'[{"id":"\xff","observation":[],"actions":["a"]}]'),
    ('decide', b'[{"id":"\\ud800","observation":[],"actions":["a"]}]'),
    ('decide', b'[{"id":"a\\u0000b","observation":[],"actions":["a"]}]'),
    ('decide', b'[{"id":"same","observation":[],"actions":["a"]},{"id":"same","observation":[],"actions":["a"]}]'),
    ('decide', b'[{"id":"a","observation":[],"actions":["a"]},{"id":"b","observation":[0],"actions":["a"]}]'),
    ('simulate', b'{"feed":"toString","initial":[0,0,0],"steps":1}'),
    ('simulate', b'{"feed":"lorenz","initial":[0,0,0],"steps":1000001}'),
    ('simulate', b'{"feed":"lorenz","initial":[0,0,0],"steps":1.5}'),
    ('simulate', b'{"feed":"lorenz","initial":[1e200,1e200,1e200],"steps":1}'),
    ('geometry', b'{"fold":91}'),
    ('geometry', b'{"fold":0,"n":101}'),
    ('geometry', b'{"fold":0,"a":0}'),
    ('rf', b'{"frequency":2.4,"options":{"surprise":1}}'),
    ('rf', b'{"frequency":0}'),
    ('recover', b'{"schema":"wrong","visible":[0,0,0],"residual":[0,0,0,0,0,0,0,0,0,0],"rotations":[]}'),
]:
    run(command, payload)
print(f'host: {checks} rejection/usage/valid-input probes passed')
