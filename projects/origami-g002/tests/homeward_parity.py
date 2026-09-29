#!/usr/bin/env python3
"""Compare independent JS and executable RMAL traces for bounded schedules."""
import json, pathlib, random, subprocess
ROOT = pathlib.Path(__file__).resolve().parents[1]
rng = random.Random(20260928)
cases = [json.loads((ROOT/'examples/homeward.json').read_text())]
for _ in range(40):
    operations = []
    for i in range(rng.randrange(1,100)):
        op = rng.choices(['discover','homeward','return_step'],[12,1,3])[0]
        operations.append({'op':op, 'id':rng.choice(['__proto__','家','H',str(i),str(i//2)]),
                           'note':str(i)+' Ω\n"\\ 9007199254740993'} if op=='discover' else {'op':op})
    cases.append({'home':'H','operations':operations})
cases.extend([
    {'home':'H','operations':[{'op':'discover','id':str(i),'note':''} for i in range(512)]},
    {'home':'H','operations':[{'op':'discover','id':str(i),'note':'kept'} for i in range(255)]+[{'op':'homeward'}]+[{'op':'return_step'}]*256}
])
for i, data in enumerate(cases):
    outputs=[]
    for command in [[str(ROOT/'build/linux/origami'),'explore','-'],['node',str(ROOT/'reference-js/bin/origami.cjs'),'explore','-']]:
        r=subprocess.run(command,input=json.dumps(data),text=True,capture_output=True,timeout=30)
        assert r.returncode==0 and not r.stderr,(i,command,r.stderr)
        outputs.append(json.loads(r.stdout))
    assert outputs[0]==outputs[1],('trace mismatch',i)
    counts=[x['retainedCount'] for x in outputs[0]['trace']]
    assert counts==sorted(counts),('lost discovery',i)
print(json.dumps({'passed':len(cases),'failed':0,'seed':20260928,
    'comparison':'Exact parsed JSON from JS reference and native RMAL; includes 512 unique discoveries'},indent=2))
