#!/usr/bin/env sh
set -eu
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
CLANG=${CLANG:-clang}
"$CLANG" --target=wasm32 -std=c2x -Oz -ffreestanding -fno-builtin -nostdlib \
  -Wl,--no-entry -Wl,--export-memory \
  -Wl,--export=sx_input_ptr -Wl,--export=sx_input_capacity \
  -Wl,--export=sx_output_ptr -Wl,--export=sx_output_len \
  -Wl,--export=sx_error_code -Wl,--export=sx_build \
  -Wl,--initial-memory=22020096 -Wl,--max-memory=33554432 \
  -Wl,--strip-all \
  -o "$ROOT/web/serveexcel.wasm" "$ROOT/core/serveexcel.c"
wc -c "$ROOT/web/serveexcel.wasm"
