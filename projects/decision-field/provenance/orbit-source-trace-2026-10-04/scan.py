import subprocess, hashlib, json, io, zipfile, pathlib, sys
TARGET='c390a188b2d3e167e18dca776cdb0945f3e534c0e45c7a5066b1a9aebe31ad60'
KEYS=[TARGET,'occurrence-710993e5d9','contentidentity-4ec137ac01','final_loop_run','AINV-C6C1E9095108DC89','BIND-C4A839B279AD514C']
report={'target':TARGET,'repositories':[],'archives':[],'matches':[],'references':[],'errors':[]}
seen=set()
def scan(data,loc,depth=0):
 h=hashlib.sha256(data).hexdigest()
 if h==TARGET:
  report['matches'].append({'location':loc,'sha256':h,'bytes':len(data)})
  pathlib.Path('investigation/recovered-exact').write_bytes(data)
 hits=[k for k in KEYS if k.encode() in data]
 if hits: report['references'].append({'location':loc,'sha256':h,'keys':hits})
 if data[:4]==b'PK\x03\x04' and h not in seen:
  seen.add(h)
  if depth>=10: report['errors'].append({'location':loc,'error':'nesting limit'});return
  try:
   with zipfile.ZipFile(io.BytesIO(data)) as z:
    members=[x for x in z.infolist() if not x.is_dir()]
    report['archives'].append({'location':loc,'sha256':h,'members':len(members)})
    for x in members:
     if x.file_size>200_000_000: report['errors'].append({'location':loc+'!'+x.filename,'error':'member size limit'});continue
     scan(z.read(x),loc+'!'+x.filename,depth+1)
  except Exception as e: report['errors'].append({'location':loc,'error':str(e)})
for repo in sys.argv[1:]:
 rows=subprocess.check_output(['git','-C',repo,'rev-list','--objects','--all']).decode().splitlines()
 paths=dict(x.split(' ',1) if ' ' in x else (x,'') for x in rows)
 p=subprocess.Popen(['git','-C',repo,'cat-file','--batch'],stdin=subprocess.PIPE,stdout=subprocess.PIPE)
 count=0
 for oid,path in paths.items():
  p.stdin.write((oid+'\n').encode());p.stdin.flush()
  header=p.stdout.readline().decode().split();size=int(header[2]);data=p.stdout.read(size);p.stdout.read(1)
  if header[1]=='blob':count+=1;scan(data,repo+':'+oid+':'+path)
 p.stdin.close();p.wait()
 report['repositories'].append({'repository':repo,'head':subprocess.check_output(['git','-C',repo,'rev-parse','HEAD']).decode().strip(),'refs':subprocess.check_output(['git','-C',repo,'for-each-ref','--format=%(refname)']).decode().splitlines(),'commits':int(subprocess.check_output(['git','-C',repo,'rev-list','--all','--count'])),'blobs':count})
for path in pathlib.Path('downloads').rglob('*'):
 if path.is_file():scan(path.read_bytes(),str(path))
pathlib.Path('investigation/SCAN.json').write_text(json.dumps(report,indent=2)+'\n')
print(json.dumps({'repositories':[(x['repository'],x['commits'],x['blobs']) for x in report['repositories']],'archives':len(report['archives']),'matches':report['matches'],'references':len(report['references']),'errors':report['errors']},indent=2))
