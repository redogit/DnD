"""Run writer and reader as distinct OS processes with only a file/digest handoff."""
import hashlib,json,pathlib,subprocess,sys,tempfile
binary=sys.argv[1]
with tempfile.TemporaryDirectory(prefix='df-checkpoint-') as temp:
    p=pathlib.Path(temp)/'native.dfn'
    write=subprocess.run([binary,'write',str(p)],capture_output=True,text=True,check=True)
    expected=json.loads(write.stdout)
    assert hashlib.sha256(p.read_bytes()).hexdigest()==expected['sha256']
    read=subprocess.run([binary,'read',str(p),expected['sha256']],capture_output=True,text=True,check=True)
    actual=json.loads(read.stdout)
    assert actual.pop('resume') is True
    assert actual==expected
    before=p.read_bytes()
    collision=subprocess.run([binary,'write',str(p)],capture_output=True,text=True)
    assert collision.returncode!=0 and p.read_bytes()==before
    bad=pathlib.Path(temp)/'damaged.dfn';bad.write_bytes(before[:-1])
    rejected=subprocess.run([binary,'read',str(bad),expected['sha256']],capture_output=True,text=True)
    assert rejected.returncode!=0
    print(json.dumps({'writer':expected,'independent_reader':actual,'resume':True,'overwrite_rejected':True,'truncation_rejected':True},sort_keys=True))
