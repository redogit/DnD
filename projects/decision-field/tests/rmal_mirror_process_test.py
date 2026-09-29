"""Actual RMAL VM, separate writer/reader, exact native receipt/recovery checks."""
import hashlib, json, pathlib, subprocess, sys, tempfile
binary,program=sys.argv[1:3]
with tempfile.TemporaryDirectory(prefix='df-rmal-mirror-') as tmp:
    p=pathlib.Path(tmp)/'mirror.dfn'
    def call(mode,path,digest=None):
        args=[binary,mode,str(path),program]+([] if digest is None else [digest])
        return subprocess.run(args,capture_output=True,text=True)
    w=call('write',p);assert w.returncode==0,w.stderr
    writer=json.loads(w.stdout);raw=p.read_bytes()
    assert hashlib.sha256(raw).hexdigest()==writer['sha256']
    r=call('read',p,writer['sha256']);assert r.returncode==0,r.stderr
    reader=json.loads(r.stdout);assert writer==reader
    assert reader['rmal_vm_executed'] is True
    assert reader['rejected_candidates_retained']==1
    overwrite=call('write',p);assert overwrite.returncode!=0 and p.read_bytes()==raw
    wrong=call('read',p,'0'*64);assert wrong.returncode!=0
    q=pathlib.Path(tmp)/'truncated.dfn';q.write_bytes(raw[:-1]);assert call('read',q,writer['sha256']).returncode!=0
    tampered=bytearray(raw);tampered[len(tampered)//2]^=1;q.write_bytes(tampered)
    assert call('read',q,writer['sha256']).returncode!=0
    different=pathlib.Path(tmp)/'changed.rmal';different.write_text(pathlib.Path(program).read_text()+'\n# different source occurrence\n')
    changed=subprocess.run([binary,'read',str(p),str(different),writer['sha256']],capture_output=True,text=True)
    assert changed.returncode!=0
    print(json.dumps({'writer':writer,'independent_reader':reader,'exact_resume':True,'changed_program_rejected':True,'overwrite_rejected':True,'wrong_digest_rejected':True,'truncation_rejected':True,'tamper_rejected_against_trusted_digest':True},sort_keys=True))
