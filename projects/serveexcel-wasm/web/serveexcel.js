const encoder = new TextEncoder();

class BinaryWriter {
  constructor() { this.chunks = []; this.length = 0; }
  raw(bytes) {
    const b = bytes instanceof Uint8Array ? bytes : new Uint8Array(bytes);
    this.chunks.push(b); this.length += b.length; return this;
  }
  u8(v) { return this.raw(Uint8Array.of(v & 0xff)); }
  u32(v) {
    const b = new Uint8Array(4);
    new DataView(b.buffer).setUint32(0, v >>> 0, true);
    return this.raw(b);
  }
  finish() {
    const out = new Uint8Array(this.length);
    let at = 0;
    for (const c of this.chunks) { out.set(c, at); at += c.length; }
    return out;
  }
}

function normalizeSheetName(name) {
  const s = String(name ?? "Sheet1").replace(/[\[\]:*?\\/]/g, "_").slice(0, 31);
  return s || "Sheet1";
}

function normalizeFileName(name) {
  const s = String(name || "report.xlsx");
  return s.toLowerCase().endsWith(".xlsx") ? s : `${s}.xlsx`;
}

function inferType(values) {
  let seen = 0, numbers = 0, booleans = 0, strings = 0;
  for (const v of values) {
    if (v === null || v === undefined) continue;
    ++seen;
    if (typeof v === "number" && Number.isFinite(v)) ++numbers;
    else if (typeof v === "boolean") ++booleans;
    else ++strings;
  }
  if (!seen || strings || (numbers && booleans)) return 0; // mixed/string
  if (numbers === seen) return 1;
  if (booleans === seen) return 2;
  return 0;
}

function schemaFor(rows) {
  const names = [];
  const seen = new Set();
  for (const row of rows) {
    if (!row || typeof row !== "object" || Array.isArray(row)) {
      throw new TypeError("ServeExcel data must be an array of plain objects");
    }
    for (const key of Object.keys(row)) {
      if (!seen.has(key)) { seen.add(key); names.push(key); }
    }
  }
  if (!names.length) throw new TypeError("ServeExcel requires at least one data column");
  return names.map(name => ({ name, type: inferType(rows.map(r => r[name])) }));
}

async function instantiateWasm(source) {
  if (source instanceof Uint8Array || source instanceof ArrayBuffer) {
    const bytes = source instanceof Uint8Array ? source : new Uint8Array(source);
    return (await WebAssembly.instantiate(bytes, {})).instance;
  }
  const url = source || new URL("./serveexcel.wasm", import.meta.url);
  const response = await fetch(url);
  if (!response.ok) throw new Error(`Unable to load ServeExcel Wasm: HTTP ${response.status}`);
  if (WebAssembly.instantiateStreaming) {
    try { return (await WebAssembly.instantiateStreaming(response.clone(), {})).instance; }
    catch { /* file servers may not send application/wasm */ }
  }
  return (await WebAssembly.instantiate(await response.arrayBuffer(), {})).instance;
}

function encodeValue(writer, value) {
  if (value === null || value === undefined) return writer.u8(0).u32(0);
  if (typeof value === "string") {
    const b = encoder.encode(value); return writer.u8(1).u32(b.length).raw(b);
  }
  if (typeof value === "number" && Number.isFinite(value)) {
    const b = encoder.encode(String(value)); return writer.u8(2).u32(b.length).raw(b);
  }
  if (typeof value === "boolean") return writer.u8(value ? 4 : 3).u32(0);
  if (value instanceof Uint8Array) return writer.u8(5).u32(value.length).raw(value);
  if (value instanceof Date) {
    const b = encoder.encode(value.toISOString()); return writer.u8(1).u32(b.length).raw(b);
  }
  const b = encoder.encode(String(value)); return writer.u8(1).u32(b.length).raw(b);
}

export class ServeExcelBuilder {
  constructor() {
    this._report = "Report";
    this._sheet = "Sheet1";
    this._file = "report.xlsx";
    this._rows = [];
    this._wasm = null;
  }
  report(name) { this._report = String(name); return this; }
  sheet(name) { this._sheet = normalizeSheetName(name); return this; }
  file(name) { this._file = normalizeFileName(name); return this; }
  data(rows) {
    if (!Array.isArray(rows)) throw new TypeError("data(rows) requires an array");
    this._rows = rows;
    return this;
  }
  wasm(source) { this._wasm = source; return this; }
  schema() { return schemaFor(this._rows); }
  encode() {
    const schema = this.schema();
    const report = encoder.encode(this._report);
    const sheet = encoder.encode(this._sheet);
    const w = new BinaryWriter();
    w.raw(encoder.encode("SX01"))
      .u32(report.length).u32(sheet.length).u32(schema.length).u32(this._rows.length)
      .raw(report).raw(sheet);
    for (const col of schema) {
      const name = encoder.encode(col.name);
      w.u8(col.type).u32(name.length).raw(name);
    }
    for (const row of this._rows) {
      for (const col of schema) encodeValue(w, row[col.name]);
    }
    return w.finish();
  }
  async bytes() {
    const input = this.encode();
    const instance = await instantiateWasm(this._wasm);
    const e = instance.exports;
    const capacity = e.sx_input_capacity();
    if (input.length > capacity) throw new RangeError(`encoded input ${input.length} exceeds Wasm input capacity ${capacity}`);
    new Uint8Array(e.memory.buffer, e.sx_input_ptr(), input.length).set(input);
    const n = e.sx_build(input.length);
    if (!n) throw new Error(`ServeExcel Wasm build failed with code ${e.sx_error_code()}`);
    return new Uint8Array(e.memory.buffer, e.sx_output_ptr(), n).slice();
  }
  async save() {
    if (typeof document === "undefined") throw new Error("save() requires a browser; use bytes() in non-browser hosts");
    const bytes = await this.bytes();
    const url = URL.createObjectURL(new Blob([bytes], {type: "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"}));
    try {
      const a = document.createElement("a");
      a.href = url; a.download = this._file; a.style.display = "none";
      document.body.append(a); a.click(); a.remove();
    } finally {
      setTimeout(() => URL.revokeObjectURL(url), 0);
    }
    return bytes.length;
  }
}

export const serveExcel = () => new ServeExcelBuilder();
