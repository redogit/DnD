#!/usr/bin/env python3
"""Behavior comparison of the real RMAL application against the preserved JS core."""
import argparse,json,math,pathlib,subprocess,sys

ROOT=pathlib.Path(__file__).resolve().parents[1]
p=argparse.ArgumentParser();p.add_argument('--binary',default=str(ROOT/'build/linux/origami'));args=p.parse_args()
assert pathlib.Path(args.binary).is_file(), 'Compiled RMAL application is required'
checks=[]

def run(command,data):
    r=subprocess.run([args.binary,command,'-'],input=json.dumps(data),text=True,capture_output=True,timeout=30)
    assert r.returncode==0, (command,r.returncode,r.stderr)
    return json.loads(r.stdout)

def same(a,b,path='result'):
    if isinstance(a,bool) or isinstance(b,bool) or a is None or b is None:
        assert a is b,(path,a,b)
    elif isinstance(a,(float,int)) and isinstance(b,(float,int)):
        assert math.isfinite(a) and math.isfinite(b) and math.isclose(a,b,rel_tol=1e-9,abs_tol=1e-10),(path,a,b)
    elif isinstance(a,dict):
        assert a.keys()==b.keys(),(path,a.keys(),b.keys())
        for k in a:same(a[k],b[k],path+'.'+k)
    elif isinstance(a,list):
        assert len(a)==len(b),(path,len(a),len(b))
        for i,(x,y) in enumerate(zip(a,b)):same(x,y,path+f'[{i}]')
    else:assert a==b,(path,a,b)

vector=[i/7-1 for i in range(13)]
cases=[]
for i in range(13):
    for j in range(i+1,13):cases.append(('encode',{'vector':vector,'rotations':[{'i':i,'j':j,'degrees':73}]}))
cases.append(('encode',{'vector':vector,'rotations':[{'i':0,'j':12,'degrees':37},{'i':0,'j':5,'degrees':-71}]}))
cases.append(('encode',{'vector':[0]*13}))
for feed in ['lorenz','rossler','clifford']:
    for steps in ([0,1,100] if feed=='clifford' else [0,1,100,1000]):cases.append(('simulate',{'feed':feed,'initial':[.1,0,0],'steps':steps}))
for x,y in [(-2.5+i*.25,1.7-i*.17) for i in range(21)]:
    cases.append(('simulate',{'feed':'clifford','initial':[x,y,0],'steps':1}))
cases.append(('simulate',{'feed':'lorenz','initial':[0,0,0],'steps':100}))
for fold in [0,15,45,75,89.99,90]:cases.append(('geometry',{'fold':fold}))
cases.append(('geometry',{'fold':22,'n':3,'a':1.2,'b':3.7,'gamma':35}))
for f in [1,2.4,3.91812384838245,5.8,8]:cases.append(('rf',{'frequency':f}))
cases.append(('rf',{'frequency':4.5,'options':{'capacitance':1.2,'inductance':4.4,'crease':0,'resistance':0,'delay':0}}))
cases.extend([
 ('decide',[{'id':'A','observation':[0,0,0],'actions':['a','b']},{'id':'B','observation':[0,0,0],'actions':['b','c']},{'id':'C','observation':[0,0,0],'actions':['a','c']}]),
 ('decide',[{'id':'A','observation':[0],'actions':['common']},{'id':'B','observation':[0],'actions':['common','b']}]),
 ('decide',[{'id':'A','observation':[1],'actions':['a']},{'id':'B','observation':[1.0000000000000002],'actions':['b']}]),
 ('decide',[{'id':'A"é','observation':[],'actions':['a','a','β']},{'id':'B','observation':[],'actions':['β']}])
])
js='''const fs=require('fs'),a=require('./reference-js/index.cjs');
const cases=JSON.parse(fs.readFileSync(0,'utf8'));
const f={encode:d=>a.encode13(d.vector,d.rotations),recover:d=>a.recover13(d),decide:d=>a.decisionSufficiency(d),simulate:d=>a.simulate(d.feed,d.initial,d.steps),geometry:d=>a.miura(d.fold,d.n,d.a,d.b,d.gamma),rf:d=>a.circuit(d.frequency,d.options)};
process.stdout.write(JSON.stringify(cases.map(([c,d])=>f[c](d))));'''
refs=json.loads(subprocess.check_output(['node','-e',js],input=json.dumps(cases),text=True,cwd=ROOT))
for (command,data),expected in zip(cases,refs):
    actual=run(command,data)
    try:same(actual,expected)
    except AssertionError as error:raise AssertionError((command,data,error.args)) from error
    checks.append(command+' reference parity')
    if command=='encode':
        restored=run('recover',actual);same(restored,data['vector'])
        checks.append('serialized inverse')

# Independent physical-coordinate convention and observable collision.
unit=[0]*13;unit[0]=1
quarter=run('encode',{'vector':unit,'rotations':[{'i':0,'j':12,'degrees':90}]})
assert abs(quarter['visible'][0])<1e-12 and abs(quarter['residual'][9]-1)<1e-12
checks.append('independent quarter turn')
plus=[0]*12+[1];minus=[0]*12+[-1]
a=run('encode',{'vector':plus});b=run('encode',{'vector':minus})
assert a['visible']==b['visible'] and a['residual']!=b['residual']
checks.append('visible collision retains residual')

# Long chaotic trajectories amplify libm/V8 rounding. They are checked for
# within-runtime repeatability and analytic map bounds, not cross-runtime identity.
long_request={'feed':'clifford','initial':[.1,0,0],'steps':1000}
long_a=run('simulate',long_request);long_b=run('simulate',long_request)
assert long_a==long_b
assert abs(long_a['state'][0])<=2 and abs(long_a['state'][1])<=1.7 and abs(long_a['state'][2])<=1.7
checks.append('long Clifford repeatability and map bounds')

bad=[('encode',{'vector':[1,2,3]}),('encode',{'vector':[0]*13,'rotations':[{'i':0,'j':0,'degrees':30}]}),
 ('recover',{'schema':'origami-transform-v1','visible':[0]*3,'residual':[0]*9,'rotations':[]}),
 ('simulate',{'feed':'toString','initial':[0,0,0],'steps':0}),('simulate',{'feed':'lorenz','initial':[0,0,0],'steps':-1}),
 ('simulate',{'feed':'lorenz','initial':[1e200]*3,'steps':1}),('geometry',{'fold':91}),('rf',{'frequency':0}),
 ('rf',{'frequency':2.4,'options':{'unknown':1}}),('decide',[]),('decide',[{'id':'A','observation':[0],'actions':[]}]),
 ('encode',{'vector':[0]*13,'claim':'verified'})]
for command,data in bad:
    r=subprocess.run([args.binary,command,'-'],input=json.dumps(data),text=True,capture_output=True,timeout=10)
    assert r.returncode!=0 and not r.stdout,(command,r.stdout,r.stderr)
    checks.append(command+' invalid input rejected')
for malformed in ['{"vector":[],"vector":[]}','{"vector":[NaN]}','{"vector":[1e999]}','{} trailing',' '*1048577]:
    r=subprocess.run([args.binary,'encode','-'],input=malformed,text=True,capture_output=True,timeout=10)
    assert r.returncode!=0 and not r.stdout,(r.stdout,r.stderr)
    checks.append('malformed JSON rejected')
print(json.dumps({'passed':len(checks),'failed':0,'binary':args.binary,'scope':'Native host + real RMAL bytecode/VM; bounded numerical parity and invalid-input fixtures'},indent=2))
