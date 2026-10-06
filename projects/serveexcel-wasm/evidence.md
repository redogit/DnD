# ServeExcel Wasm successor — local evidence

Date: 2026-10-05

## Toolchain

- Clang/LLVM: 17.0.0
- LLD: 17.0.0
- Node: 22.16.0
- Python: 3.13.5

## Build

`./tools/build.sh`

- `web/serveexcel.wasm`: 9,038 bytes
- SHA-256: `260ed79a27d54221f181bfb41f782b9d5fa8b8190b173662ed455bbd8a449a2c`
- complete browser payload, uncompressed (`index.html + style.css + serveexcel.js + serveexcel.wasm`): 16,997 bytes
- gzip -9 component sum: 6,586 bytes
- Wasm configured initial linear memory: 22,020,096 bytes
- Wasm configured maximum linear memory: 33,554,432 bytes

## Fixture execution

`node tests/test.mjs`

- `nul.xlsx`: 6,023 bytes
- `shape.xlsx`: 6,439 bytes
- inferred changed schema: `name, score, active, note`

`python3 tests/verify_xlsx.py tests/out/null.xlsx tests/out/shape.xlsx`

PASS:
- ZIP CRCs are valid;
- all required package XML parts parse;
- embedded `alpha\00omega` is projected into worksheet XML and recovered byte-for-byte from WordDNA provenance.
- changing input object shape produces the expected union schema.

`artifact_tool` import probe:

- workbook opened successfully;
- `Changed shape!A1:D3` read as headers `name, score, active, note` and the expected two data rows.

## Browser engine probe

Chromium headless executed the exact `web/serveexcel.js` module and `web/serveexcel.wasm` bytes directly in-engine:

- generated XLSX: 6,399 bytes;
- first four bytes: `PK\\x03\\x04`;
- dynamic union schema probe: `a, b`.

The environment blocks localhost browser navigation, so this probe instantiated the module from a data URL instead of using the demo page server.

Firefox execution is **not verified in this environment** because the Playwright Firefox executable is not installed. No Firefox failure was observed; the run was unavailable.

## Claim ceiling

`BUILD_PASS != ALL_BROWSER_PASS`
`XLSX_STRUCTURE_PASS != EXCEL_FEATURE_COMPLETENESS`
`WORDDNA_PROVENANCE_ROUNDTRIP != VISIBLE_CELL_NUL_IDENTITY
`SUCCESSOR != REWRITTEN_PREDECESSOR`
