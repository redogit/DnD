#!/usr/bin/env python3
"""Generate a snapshot-enabled successor; never rewrite the frozen predecessor."""
from pathlib import Path
import hashlib
import sys

SOURCE_SHA256 = '279c4e6cf1ac8b9f9896625bd761d9aa3ed9f89e865c8b47c0061fa28030a34b'

def prepare(source: Path, output: Path) -> None:
    data = source.read_bytes()
    if hashlib.sha256(data).hexdigest() != SOURCE_SHA256:
        raise ValueError('unexpected predecessor bytes; refuse silent source migration')
    text = data.decode('utf-8')
    before = 'template <typename T>\nclass DecisionFieldEngine {'
    private = 'private:\n    DomainPolicy<T> policy_;'
    if text.count(before) != 1 or text.count(private) != 1:
        raise ValueError('snapshot API insertion anchors are not unique')
    text = text.replace(before, '#include "decision_field/detail/checkpoint_types.inc"\n\n' + before)
    text = text.replace(private, '#include "decision_field/detail/checkpoint_methods.inc"\n\n' + private)
    output = output / 'decision_field'
    output.mkdir(parents=True, exist_ok=True)
    (output / 'decision_field.hpp').write_bytes(text.encode('utf-8'))
    (output / 'native_checkpoint_available.hpp').write_text('#pragma once\n#include "decision_field/decision_field.hpp"\n',encoding='utf-8')
    (output / 'GENERATED_SOURCE.sha256').write_text(hashlib.sha256(text.encode()).hexdigest() + '\n',encoding='ascii')

if __name__ == '__main__':
    if len(sys.argv) != 3:
        raise SystemExit('usage: prepare_native_engine.py ORIGINAL_ENGINE.hpp OUTPUT_DIRECTORY')
    prepare(Path(sys.argv[1]), Path(sys.argv[2]))
