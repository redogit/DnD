#!/usr/bin/env bash
# Linux audit fallback for the imported source; not the release Zig/C23 build.
set -euo pipefail

cd "$(dirname "$0")/.."
major="$(gcc -dumpfullversion -dumpversion | cut -d. -f1)"
if [[ "$major" != 13 ]]; then
  echo "This audit fallback was checked with GCC 13; found GCC $major" >&2
  exit 2
fi

python3 - <<'PY'
from pathlib import Path
names = ['core.rmal', 'models.rmal', 'exploration.rmal',
         'centering.rmal', 'centering_run.rmal', 'dispatch.rmal']
source = '\n'.join((Path('program') / name).read_text(encoding='utf-8') for name in names)
assert Path('program/origami.rmal').read_text(encoding='utf-8') == source, 'combined RMAL source is stale'
data = source.encode('utf-8') + b'\0'
header = '/* Generated from program/origami.rmal by scripts/build.py. */\nstatic const char origami_program[] = {\n'
header += '\n'.join(','.join('0x%02x' % x for x in data[i:i+24]) + ',' for i in range(0, len(data), 24)) + '\n};\n'
assert Path('native/origami_program.h').read_text(encoding='ascii') == header, 'embedded RMAL source is stale'
PY

mkdir -p build/linux
flags=(-std=c2x -D__STDC_VERSION__=202311L -w -O2 -ffp-contract=off -Itoolchain/include)
gcc "${flags[@]}" toolchain/src/main.c toolchain/src/rmal.c -lm -o build/linux/rmalc
gcc "${flags[@]}" native/main.c toolchain/src/rmal.c -lm -o build/linux/origami
build/linux/rmalc selfcheck
build/linux/rmalc check program/origami.rmal
echo "GCC 13 Linux audit build complete; run the test sequence in README.md."
