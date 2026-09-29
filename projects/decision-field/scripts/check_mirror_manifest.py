#!/usr/bin/env python3
"""Check staged source identity, not compiler or runtime correctness."""
import argparse
import hashlib
import json
from pathlib import Path

def check(root: Path) -> int:
    manifest = json.loads((root / 'projects/decision-field/evidence/v0_7/SOURCE_MANIFEST.json').read_text())
    if manifest.get('schema') != 'decision-field/mirror-source-manifest/v1':
        raise ValueError('wrong Mirror source manifest')
    files = manifest['files']
    if len(files) < 13:
        raise ValueError('incomplete Mirror source manifest')
    for name, expected in files.items():
        rel = Path(name)
        if rel.is_absolute() or '..' in rel.parts:
            raise ValueError('unsafe source path')
        raw = (root / rel).read_bytes()
        actual = {'bytes': len(raw), 'sha256': hashlib.sha256(raw).hexdigest(),
                  'git_blob': hashlib.sha1(b'blob ' + str(len(raw)).encode() + b'\0' + raw).hexdigest()}
        if actual != expected:
            raise ValueError('source identity mismatch: ' + name)
    return len(files)

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--root', type=Path, default=Path(__file__).resolve().parents[3])
    args = parser.parse_args()
    print(f'PASS {check(args.root)} source identities; MANIFEST_MATCH != RUNTIME_TEST')
