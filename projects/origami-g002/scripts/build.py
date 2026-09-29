#!/usr/bin/env python3
"""Build real RMALC and a native host with the checked RMAL program embedded."""
import argparse,datetime,hashlib,json,pathlib,platform,shutil,subprocess,sys

ROOT=pathlib.Path(__file__).resolve().parents[1]
p=argparse.ArgumentParser()
p.add_argument('--zig',default='zig')
p.add_argument('--toolchain',type=pathlib.Path,default=ROOT/'toolchain')
p.add_argument('--target',choices=['linux','windows','all'],default='all')
args=p.parse_args()
tc=args.toolchain.resolve()
zig=str(pathlib.Path(args.zig).resolve()) if pathlib.Path(args.zig).exists() else args.zig
out=ROOT/'build';out.mkdir(exist_ok=True)
logs=[]

def run(cmd,capture=None):
    print(' '.join(str(x) for x in cmd),flush=True)
    r=subprocess.run([str(x) for x in cmd],cwd=ROOT,text=True,capture_output=True)
    logs.append({'command':[str(x) for x in cmd],'exitCode':r.returncode,'stdout':r.stdout,'stderr':r.stderr})
    if capture:capture.write_text(r.stdout,encoding='utf-8')
    if r.returncode:
        sys.stderr.write(r.stderr+r.stdout)
        (out/'build-log.json').write_text(json.dumps(logs,indent=2)+'\n')
        raise SystemExit(r.returncode)
    return r.stdout

source='\n'.join((ROOT/'program'/name).read_text(encoding='utf-8') for name in ['core.rmal','models.rmal','exploration.rmal','centering.rmal','centering_run.rmal','dispatch.rmal'])
combined=ROOT/'program/origami.rmal';combined.write_text(source,encoding='utf-8')
data=source.encode('utf-8')+b'\0'
header='/* Generated from program/origami.rmal by scripts/build.py. */\nstatic const char origami_program[] = {\n'
header+='\n'.join(','.join('0x%02x'%x for x in data[i:i+24])+',' for i in range(0,len(data),24))+'\n};\n'
(ROOT/'native/origami_program.h').write_text(header,encoding='ascii')
host=out/'host-tools';host.mkdir(exist_ok=True)
ext='.exe' if platform.system()=='Windows' else ''
compiler=host/('rmalc'+ext)
flags=['-std=c23','-O2','-Wall','-Wextra','-Wpedantic','-Werror','-ffp-contract=off','-I'+str(tc/'include')]
run([zig,'cc',*flags,tc/'src/main.c',tc/'src/rmal.c','-lm','-o',compiler])
run([compiler,'selfcheck'],out/'compiler-selfcheck.txt')
run([compiler,'check',combined],out/'program-check.txt')
run([compiler,'compile',combined],out/'origami.rmalbc.txt')
run([compiler,'manifest',combined,'--json'],out/'program-manifest.json')

targets=['linux','windows'] if args.target=='all' else [args.target]
for target in targets:
    dest=out/target;dest.mkdir(exist_ok=True)
    targetflags=['-target','x86_64-windows-gnu','-static'] if target=='windows' else []
    suffix='.exe' if target=='windows' else ''
    run([zig,'cc',*targetflags,*flags,ROOT/'native/main.c',tc/'src/rmal.c','-lm','-o',dest/('origami'+suffix)])
    run([zig,'cc',*targetflags,*flags,tc/'src/main.c',tc/'src/rmal.c','-lm','-o',dest/('rmalc'+suffix)])
receipt={'date':datetime.datetime.now(datetime.timezone.utc).isoformat(),'platform':platform.platform(),
 'compilerVersion':run([zig,'version']).strip(),'rmalVersion':run([compiler,'version']).strip(),
 'programSHA256':hashlib.sha256(data[:-1]).hexdigest(),'targets':targets,
 'executionModel':'Embedded RMAL source -> RMALC in-memory bytecode -> VM at application startup',
 'commands':logs}
(out/'build-log.json').write_text(json.dumps(receipt,indent=2)+'\n',encoding='utf-8')
print('Build complete.',flush=True)
