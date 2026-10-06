# ServeExcel Wasm successor

A minimal successor to the historical `BanalityOfSeeking/ServeExcel` server path.

## Runtime path

```text
array<object>
  -> infer union schema from supplied data
  -> fluent ServeExcelBuilder
  -> length-bounded SX01 input
  -> freestanding C core compiled by LLVM/Clang to WebAssembly
  -> standards-compliant XLSX ZIP/XML bytes
  -> browser download or host-owned bytes
```

No HTTP listener, URL command grammar, report registry, responder hierarchy, Dataflow blocks, EPPlus, .NET, or C# are required.

## Build and verify

```sh
./tools/build.sh
node tests/test.mjs
python3 tests/verify_xlsx.py tests/out/nul.xlsx tests/out/shape.xlsx
unzip -t tests/out/nul.xlsx
```

For the browser demo:

```sh
python3 -m http.server 8080 --directory web
# open http://127.0.0.1:8080 in Firefox or Chrome/Chromium
```

The browser host uses only standard `WebAssembly`, `TextEncoder`, `Blob`, object URLs, and module APIs. Browser storage, Canvas, WebGL, WebGPU, and framework code are omitted because this generation path does not require them.

## Fluent builder

```js
import { serveExcel } from "./serveexcel.js";

await serveExcel()
  .report("People")
  .sheet("Data")
  .data([
    { name: "Ada", score: 98 },
    { name: "Grace", active: true, note: "schema expands from data" }
  ])
  .file("people.xlsx")
  .save();
```

Columns are the first-seen union of object keys. Column type is inferred from non-null values, while each cell retains its actual scalar tag so a changing/mixed shape is not coerced through URL text commands.

## WordDNA boundary

Canonical supplied UTF-8 bytes are length-bounded and never normalized before the C core consumes them. Spreadsheet XML is necessarily a rendering projection: XML-illegal control bytes such as U+0000 are emitted as Excel-style `_x0000_` text. `customXml/item1.xml` carries a reversible hex projection with occurrence IDs and local exact-byte fingerprints, so the original bytes remain recoverable even when the visible spreadsheet representation cannot contain the code point literally.

The local `semantic-object-id` convention is FNV-1a-64 over exact bytes. It is a compact project-local key, not a collision-proof/global semantic identity claim and does not redefine the parent RMAL WordDNA API.

## Bounded limits

- one worksheet per workbook in this increment;
- 256 columns;
- 100,000 rows;
- 4 MiB encoded input buffer;
- 16 MiB XLSX output buffer;
- ZIP entries are stored (no compression) to keep the core dependency-free;
- schema inference is in the thin JavaScript host; XLSX construction is in the C/Wasm core.

These are explicit implementation bounds, not spreadsheet-format limits.

## Measured local evidence

On the 2026-10-05 local run, `serveexcel.wasm` was 9,038 bytes; the full uncompressed browser payload was 16,997 bytes (6,586 bytes as the sum of individually gzip-9-compressed assets). Node fixtures, ZIP/XML validation, exact embedded-NUL provenance recovery, and an `artifact_tool` workbook import all passed. Chromium executed the exact JS/Wasm bytes in-engine and produced an XLSX stream. Firefox execution was unavailable because its browser binary is not installed in the test environment; it is not recorded as passed.

See `evidence.md` for the bounded evidence record.
