import { readFile, writeFile, mkdir } from "node:fs/promises";
import { fileURLToPath } from "node:url";
import { dirname, resolve } from "node:path";
import { serveExcel } from "../web/serveexcel.js";

const here = dirname(fileURLToPath(import.meta.url));
const root = resolve(here, "..");
const wasm = new Uint8Array(await readFile(resolve(root, "web/serveexcel.wasm")));
const out = resolve(here, "out");
await mkdir(out, { recursive: true });

const nul = "alpha\0omega";
const a = await serveExcel()
  .wasm(wasm)
  .report("NUL fixture")
  .sheet("Data")
  .data([
    { id: 1, text: nul, active: true },
    { id: 2, text: "literal _x0000_ token", active: false }
  ])
  .bytes();
await writeFile(resolve(out, "nul.xlsx"), a);

const bBuilder = serveExcel()
  .wasm(wasm)
  .report("Shape fixture")
  .sheet("Changed shape")
  .data([
    { name: "Ada", score: 98 },
    { name: "Grace", active: true, note: "later column" }
  ]);
const schema = bBuilder.schema();
if (schema.map(x => x.name).join(",") !== "name,score,active,note") {
  throw new Error(`schema union failed: ${JSON.stringify(schema)}`);
}
const b = await bBuilder.bytes();
await writeFile(resolve(out, "shape.xlsx"), b);

console.log(JSON.stringify({nul_xlsx_bytes: a.length, shape_xlsx_bytes: b.length, schema}, null, 2));
