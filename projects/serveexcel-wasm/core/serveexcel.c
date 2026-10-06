#include <stddef.h>
#include <stdint.h>

#define SX_INPUT_CAPACITY  (4u * 1024u * 1024u)
#define SX_OUTPUT_CAPACITY (16u * 1024u * 1024u)
#define SX_MAX_COLUMNS 256u
#define SX_MAX_ENTRIES 16u

#define SX_OK 0u
#define SX_ERR_INPUT 1u
#define SX_ERR_RANGE 2u
#define SX_ERR_OUTPUT 3u
#define SX_ERR_SCHEMA 4u

static uint8_t sx_input[SX_INPUT_CAPACITY];
static uint8_t sx_output[SX_OUTPUT_CAPACITY];
static uint32_t sx_out_len_value = 0;
static uint32_t sx_error_value = SX_OK;

typedef struct {
    const uint8_t *p;
    const uint8_t *end;
} Reader;

typedef struct {
    const uint8_t *name;
    uint32_t name_len;
    uint8_t declared_type;
} Column;

typedef struct {
    const char *name;
    uint32_t name_len;
    uint32_t local_offset;
    uint32_t crc32;
    uint32_t size;
} ZipEntry;

typedef struct {
    ZipEntry *entry;
    uint32_t crc;
    uint32_t data_start;
    int active;
} ZipPart;

static Column columns[SX_MAX_COLUMNS];
static ZipEntry entries[SX_MAX_ENTRIES];
static uint32_t entry_count = 0;

static uint32_t cstr_len(const char *s) {
    uint32_t n = 0;
    while (s[n]) ++n;
    return n;
}

static int raw_bytes(const void *data, uint32_t n) {
    if (n > SX_OUTPUT_CAPACITY - sx_out_len_value) {
        sx_error_value = SX_ERR_OUTPUT;
        return 0;
    }
    const uint8_t *p = (const uint8_t *)data;
    for (uint32_t i = 0; i < n; ++i) sx_output[sx_out_len_value + i] = p[i];
    sx_out_len_value += n;
    return 1;
}

static int raw_u16(uint16_t v) {
    uint8_t b[2] = {(uint8_t)(v & 255u), (uint8_t)((v >> 8) & 255u)};
    return raw_bytes(b, 2);
}

static int raw_u32(uint32_t v) {
    uint8_t b[4] = {
        (uint8_t)(v & 255u),
        (uint8_t)((v >> 8) & 255u),
        (uint8_t)((v >> 16) & 255u),
        (uint8_t)((v >> 24) & 255u)
    };
    return raw_bytes(b, 4);
}

static uint32_t crc32_step(uint32_t crc, uint8_t b) {
    crc ^= b;
    for (uint32_t i = 0; i < 8u; ++i) {
        uint32_t mask = (uint32_t)-(int32_t)(crc & 1u);
        crc = (crc >> 1) ^ (0xEDB88320u & mask);
    }
    return crc;
}

static int zip_begin(ZipPart *part, const char *name) {
    if (entry_count >= SX_MAX_ENTRIES) {
        sx_error_value = SX_ERR_RANGE;
        return 0;
    }
    ZipEntry *e = &entries[entry_count++];
    e->name = name;
    e->name_len = cstr_len(name);
    e->local_offset = sx_out_len_value;
    e->crc32 = 0;
    e->size = 0;

    if (!raw_u32(0x04034B50u) || !raw_u16(20u) || !raw_u16(0x0008u) ||
        !raw_u16(0u) || !raw_u16(0u) || !raw_u16(0u) ||
        !raw_u32(0u) || !raw_u32(0u) || !raw_u32(0u) ||
        !raw_u16((uint16_t)e->name_len) || !raw_u16(0u) ||
        !raw_bytes(name, e->name_len)) return 0;

    part->entry = e;
    part->crc = 0xFFFFFFFFu;
    part->data_start = sx_out_len_value;
    part->active = 1;
    return 1;
}

static int part_bytes(ZipPart *part, const void *data, uint32_t n) {
    if (!part->active) return 0;
    if (!raw_bytes(data, n)) return 0;
    const uint8_t *p = (const uint8_t *)data;
    for (uint32_t i = 0; i < n; ++i) part->crc = crc32_step(part->crc, p[i]);
    return 1;
}

static int part_text(ZipPart *part, const char *text) {
    return part_bytes(part, text, cstr_len(text));
}

static int part_u32_dec(ZipPart *part, uint32_t v) {
    char buf[10];
    uint32_t n = 0;
    if (!v) buf[n++] = '0';
    else {
        char rev[10];
        uint32_t r = 0;
        while (v) {
            rev[r++] = (char)('0' + (v % 10u));
            v /= 10u;
        }
        while (r) buf[n++] = rev[--r];
    }
    return part_bytes(part, buf, n);
}

static int part_u64_hex(ZipPart *part, uint64_t v) {
    static const char hex[] = "0123456789abcdef";
    char out[16];
    for (int i = 15; i >= 0; --i) {
        out[i] = hex[v & 15u];
        v >>= 4;
    }
    return part_bytes(part, out, 16u);
}

static int part_hex_bytes(ZipPart *part, const uint8_t *p, uint32_t n) {
    static const char hex[] = "0123456789abcdef";
    char pair[2];
    for (uint32_t i = 0; i < n; ++i) {
        pair[0] = hex[(p[i] >> 4) & 15u];
        pair[1] = hex[p[i] & 15u];
        if (!part_bytes(part, pair, 2u)) return 0;
    }
    return 1;
}

static int is_hex_ascii(uint8_t b) {
    return (b >= '0' && b <= '9') || (b >= 'a' && b <= 'f') || (b >= 'A' && b <= 'F');
}

static int part_xml_bytes(ZipPart *part, const uint8_t *p, uint32_t n) {
    static const char hex[] = "0123456789ABCDEF";
    for (uint32_t i = 0; i < n; ++i) {
        uint8_t b = p[i];
        if (b == '&') { if (!part_text(part, "&amp;")) return 0; }
        else if (b == '<') { if (!part_text(part, "&lt;")) return 0; }
        else if (b == '>') { if (!part_text(part, "&gt;")) return 0; }
        else if (b == '"') { if (!part_text(part, "&quot;")) return 0; }
        else if (b == '\'') { if (!part_text(part, "&apos;")) return 0; }
        else if (b < 0x20u && b != 0x09u && b != 0x0Au && b != 0x0Du) {
            char esc[7] = {'_', 'x', '0', '0', hex[(b >> 4) & 15u], hex[b & 15u], '_'};
            if (!part_bytes(part, esc, 7u)) return 0;
        } else if (b == '_' && i + 6u < n && p[i + 1u] == 'x' &&
                   is_hex_ascii(p[i + 2u]) && is_hex_ascii(p[i + 3u]) &&
                   is_hex_ascii(p[i + 4u]) && is_hex_ascii(p[i + 5u]) &&
                   p[i + 6u] == '_') {
            if (!part_text(part, "_x005F_")) return 0;
        } else {
            if (!part_bytes(part, &b, 1u)) return 0;
        }
    }
    return 1;
}

static int zip_end(ZipPart *part) {
    if (!part->active) return 0;
    ZipEntry *e = part->entry;
    e->size = sx_out_len_value - part->data_start;
    e->crc32 = part->crc ^ 0xFFFFFFFFu;
    part->active = 0;
    return raw_u32(0x08074B50u) && raw_u32(e->crc32) && raw_u32(e->size) && raw_u32(e->size);
}

static int zip_finish(void) {
    uint32_t central_start = sx_out_len_value;
    for (uint32_t i = 0; i < entry_count; ++i) {
        ZipEntry *e = &entries[i];
        if (!raw_u32(0x02014B50u) || !raw_u16(20u) || !raw_u16(20u) ||
            !raw_u16(0x0008u) || !raw_u16(0u) || !raw_u16(0u) || !raw_u16(0u) ||
            !raw_u32(e->crc32) || !raw_u32(e->size) || !raw_u32(e->size) ||
            !raw_u16((uint16_t)e->name_len) || !raw_u16(0u) || !raw_u16(0u) ||
            !raw_u16(0u) || !raw_u16(0u) || !raw_u32(0u) || !raw_u32(e->local_offset) ||
            !raw_bytes(e->name, e->name_len)) return 0;
    }
    uint32_t central_size = sx_out_len_value - central_start;
    return raw_u32(0x06054B50u) && raw_u16(0u) && raw_u16(0u) &&
           raw_u16((uint16_t)entry_count) && raw_u16((uint16_t)entry_count) &&
           raw_u32(central_size) && raw_u32(central_start) && raw_u16(0u);
}

static int r_u8(Reader *r, uint8_t *out) {
    if (r->p >= r->end) return 0;
    *out = *r->p++;
    return 1;
}

static int r_u32(Reader *r, uint32_t *out) {
    if ((uint32_t)(r->end - r->p) < 4u) return 0;
    *out = (uint32_t)r->p[0] | ((uint32_t)r->p[1] << 8) |
           ((uint32_t)r->p[2] << 16) | ((uint32_t)r->p[3] << 24);
    r->p += 4;
    return 1;
}

static int r_bytes(Reader *r, uint32_t n, const uint8_t **out) {
    if (n > (uint32_t)(r->end - r->p)) return 0;
    *out = r->p;
    r->p += n;
    return 1;
}

static uint64_t fnv1a64(const uint8_t *p, uint32_t n) {
    uint64_t h = 1469598103934665603ull;
    for (uint32_t i = 0; i < n; ++i) {
        h ^= p[i];
        h *= 1099511628211ull;
    }
    return h;
}

static int cell_ref(ZipPart *part, uint32_t col0, uint32_t row1) {
    char letters[4];
    uint32_t n = 0;
    uint32_t x = col0 + 1u;
    do {
        uint32_t rem = (x - 1u) % 26u;
        letters[n++] = (char)('A' + rem);
        x = (x - 1u) / 26u;
    } while (x && n < 4u);
    while (n) {
        if (!part_bytes(part, &letters[--n], 1u)) return 0;
    }
    return part_u32_dec(part, row1);
}

static int write_content_types(void) {
    ZipPart p;
    if (!zip_begin(&p, "[Content_Types].xml")) return 0;
    if (!part_text(&p,
        "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>"
        "<Types xmlns=\"http://schemas.openxmlformats.org/package/2006/content-types\">"
        "<Default Extension=\"rels\" ContentType=\"application/vnd.openxmlformats-package.relationships+xml\"/>"
        "<Default Extension=\"xml\" ContentType=\"application/xml\"/>"
        "<Override PartName=\"/xl/workbook.xml\" ContentType=\"application/vnd.openxmlformats-officedocument.spreadsheetml.sheet.main+xml\"/>"
        "<Override PartName=\"/xl/worksheets/sheet1.xml\" ContentType=\"application/vnd.openxmlformats-officedocument.spreadsheetml.worksheet+xml\"/>"
        "<Override PartName=\"/docProps/core.xml\" ContentType=\"application/vnd.openxmlformats-package.core-properties+xml\"/>"
        "<Override PartName=\"/docProps/app.xml\" ContentType=\"application/vnd.openxmlformats-officedocument.extended-properties+xml\"/>"
        "</Types>")) return 0;
    return zip_end(&p);
}

static int write_root_rels(void) {
    ZipPart p;
    if (!zip_begin(&p, "_rels/.rels")) return 0;
    if (!part_text(&p,
        "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>"
        "<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">"
        "<Relationship Id=\"rId1\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/officeDocument\" Target=\"xl/workbook.xml\"/>"
        "<Relationship Id=\"rId2\" Type=\"http://schemas.openxmlformats.org/package/2006/relationships/metadata/core-properties\" Target=\"docProps/core.xml\"/>"
        "<Relationship Id=\"rId3\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/extended-properties\" Target=\"docProps/app.xml\"/>"
        "<Relationship Id=\"rId4\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/customXml\" Target=\"customXml/item1.xml\"/>"
        "</Relationships>")) return 0;
    return zip_end(&p);
}

static int write_core(const uint8_t *report, uint32_t report_len) {
    ZipPart p;
    if (!zip_begin(&p, "docProps/core.xml")) return 0;
    if (!part_text(&p,
        "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>"
        "<cp:coreProperties xmlns:cp=\"http://schemas.openxmlformats.org/package/2006/metadata/core-properties\" "
        "xmlns:dc=\"http://purl.org/dc/elements/1.1/\"><dc:title>")) return 0;
    if (!part_xml_bytes(&p, report, report_len)) return 0;
    if (!part_text(&p, "</dc:title><dc:creator>ServeExcel Wasm successor</dc:creator></cp:coreProperties>")) return 0;
    return zip_end(&p);
}

static int write_app(void) {
    ZipPart p;
    if (!zip_begin(&p, "docProps/app.xml")) return 0;
    if (!part_text(&p,
        "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>"
        "<Properties xmlns=\"http://schemas.openxmlformats.org/officeDocument/2006/extended-properties\" "
        "xmlns:vt=\"http://schemas.openxmlformats.org/officeDocument/2006/docPropsVTypes\">"
        "<Application>ServeExcel Wasm</Application></Properties>")) return 0;
    return zip_end(&p);
}

static int write_workbook(const uint8_t *sheet, uint32_t sheet_len) {
    ZipPart p;
    if (!zip_begin(&p, "xl/workbook.xml")) return 0;
    if (!part_text(&p,
        "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>"
        "<workbook xmlns=\"http://schemas.openxmlformats.org/spreadsheetml/2006/main\" "
        "xmlns:r=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships\"><sheets><sheet name=\"")) return 0;
    if (!part_xml_bytes(&p, sheet, sheet_len)) return 0;
    if (!part_text(&p, "\" sheetId=\"1\" r:id=\"rId1\"/></sheets></workbook>")) return 0;
    return zip_end(&p);
}

static int write_workbook_rels(void) {
    ZipPart p;
    if (!zip_begin(&p, "xl/_rels/workbook.xml.rels")) return 0;
    if (!part_text(&p,
        "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>"
        "<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">"
        "<Relationship Id=\"rId1\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/worksheet\" Target=\"worksheets/sheet1.xml\"/>"
        "</Relationships>")) return 0;
    return zip_end(&p);
}

static int write_inline_string_cell(ZipPart *p, uint32_t col0, uint32_t row1,
                                    const uint8_t *bytes, uint32_t n) {
    if (!part_text(p, "<c r=\"")) return 0;
    if (!cell_ref(p, col0, row1)) return 0;
    if (!part_text(p, "\" t=\"inlineStr\"><is><t xml:space=\"preserve\">")) return 0;
    if (!part_xml_bytes(p, bytes, n)) return 0;
    return part_text(p, "</t></is></c>");
}

static int write_worksheet(Reader *cell_reader, uint32_t rows, uint32_t cols) {
    ZipPart p;
    if (!zip_begin(&p, "xl/worksheets/sheet1.xml")) return 0;
    if (!part_text(&p,
        "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>"
        "<worksheet xmlns=\"http://schemas.openxmlformats.org/spreadsheetml/2006/main\"><sheetData><row r=\"1\">")) return 0;
    for (uint32_t c = 0; c < cols; ++c) {
        if (!write_inline_string_cell(&p, c, 1u, columns[c].name, columns[c].name_len)) return 0;
    }
    if (!part_text(&p, "</row>")) return 0;

    for (uint32_t r = 0; r < rows; ++r) {
        if (!part_text(&p, "<row r=\"")) return 0;
        if (!part_u32_dec(&p, r + 2u) || !part_text(&p, "\">")) return 0;
        for (uint32_t c = 0; c < cols; ++c) {
            uint8_t tag = 0;
            uint32_t n = 0;
            const uint8_t *bytes = 0;
            if (!r_u8(cell_reader, &tag) || !r_u32(cell_reader, &n) || !r_bytes(cell_reader, n, &bytes)) {
                sx_error_value = SX_ERR_INPUT;
                return 0;
            }
            if (tag == 0u) continue;
            if (tag == 1u || tag == 5u) {
                if (!write_inline_string_cell(&p, c, r + 2u, bytes, n)) return 0;
            } else if (tag == 2u) {
                if (!part_text(&p, "<c r=\"")) return 0;
                if (!cell_ref(&p, c, r + 2u) || !part_text(&p, "\"><v>")) return 0;
                if (!part_bytes(&p, bytes, n) || !part_text(&p, "</v></c>")) return 0;
            } else if (tag == 3u || tag == 4u) {
                if (!part_text(&p, "<c r=\"")) return 0;
                if (!cell_ref(&p, c, r + 2u) || !part_text(&p, "\" t=\"b\"><v>")) return 0;
                if (!part_text(&p, tag == 4u ? "1" : "0") || !part_text(&p, "</v></c>")) return 0;
            } else {
                sx_error_value = SX_ERR_SCHEMA;
                return 0;
            }
        }
        if (!part_text(&p, "</row>")) return 0;
    }
    if (!part_text(&p, "</sheetData></worksheet>")) return 0;
    return zip_end(&p);
}

static int provenance_word(ZipPart *p, uint32_t occurrence,
                           const char *role, const uint8_t *bytes, uint32_t n,
                           uint32_t row, uint32_t col) {
    if (!part_text(p, "<worddna occurrence-id=\"")) return 0;
    if (!part_u32_dec(p, occurrence) || !part_text(p, "\" semantic-object-id=\"")) return 0;
    if (!part_u64_hex(p, fnv1a64(bytes, n)) || !part_text(p, "\" local-id-policy=\"fnv1a64-exact-bytes\" role=\"")) return 0;
    if (!part_text(p, role) || !part_text(p, "\" byte-count=\"")) return 0;
    if (!part_u32_dec(p, n)) return 0;
    if (row) {
        if (!part_text(p, "\" row=\"")) return 0;
        if (!part_u32_dec(p, row)) return 0;
    }
    if (col) {
        if (!part_text(p, "\" col=\"")) return 0;
        if (!part_u32_dec(p, col)) return 0;
    }
    if (!part_text(p, "\" bytes-hex=\"")) return 0;
    if (!part_hex_bytes(p, bytes, n)) return 0;
    return part_text(p, "\"/>");
}

static int write_provenance(const uint8_t *report, uint32_t report_len,
                            const uint8_t *sheet, uint32_t sheet_len,
                            Reader cell_reader, uint32_t rows, uint32_t cols) {
    ZipPart p;
    if (!zip_begin(&p, "customXml/item1.xml")) return 0;
    if (!part_text(&p,
        "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>"
        "<serveexcel-provenance xmlns=\"urn:redogit:serveexcel:worddna:1\" "
        "canonical-input=\"exact-length-bounded-utf8\" projection=\"xlsx-xml-plus-hex\">")) return 0;
    uint32_t occ = 1u;
    if (!provenance_word(&p, occ++, "report", report, report_len, 0u, 0u)) return 0;
    if (!provenance_word(&p, occ++, "sheet", sheet, sheet_len, 0u, 0u)) return 0;
    for (uint32_t c = 0; c < cols; ++c) {
        if (!provenance_word(&p, occ++, "column", columns[c].name, columns[c].name_len, 1u, c + 1u)) return 0;
    }
    for (uint32_t r = 0; r < rows; ++r) {
        for (uint32_t c = 0; c < cols; ++c) {
            uint8_t tag = 0;
            uint32_t n = 0;
            const uint8_t *bytes = 0;
            if (!r_u8(&cell_reader, &tag) || !r_u32(&cell_reader, &n) || !r_bytes(&cell_reader, n, &bytes)) {
                sx_error_value = SX_ERR_INPUT;
                return 0;
            }
            if (tag == 1u || tag == 5u) {
                if (!provenance_word(&p, occ++, tag == 1u ? "cell-string" : "cell-bytes",
                                     bytes, n, r + 2u, c + 1u)) return 0;
            }
        }
    }
    if (!part_text(&p,
        "<claim-boundary>WORD_IDENTITY != CURRENT_INTERPRETATION; SOURCE_IDENTITY != RENDERING; SUCCESSOR != REWRITTEN_PREDECESSOR</claim-boundary>"
        "</serveexcel-provenance>")) return 0;
    return zip_end(&p);
}

uintptr_t sx_input_ptr(void) { return (uintptr_t)sx_input; }
uint32_t sx_input_capacity(void) { return SX_INPUT_CAPACITY; }
uintptr_t sx_output_ptr(void) { return (uintptr_t)sx_output; }
uint32_t sx_output_len(void) { return sx_out_len_value; }
uint32_t sx_error_code(void) { return sx_error_value; }

uint32_t sx_build(uint32_t input_len) {
    sx_out_len_value = 0u;
    sx_error_value = SX_OK;
    entry_count = 0u;
    if (input_len > SX_INPUT_CAPACITY || input_len < 24u) {
        sx_error_value = SX_ERR_INPUT;
        return 0u;
    }

    Reader r = {sx_input, sx_input + input_len};
    const uint8_t *magic = 0;
    uint32_t report_len = 0, sheet_len = 0, cols = 0, rows = 0;
    if (!r_bytes(&r, 4u, &magic) || magic[0] != 'S' || magic[1] != 'X' || magic[2] != '0' || magic[3] != '1' ||
        !r_u32(&r, &report_len) || !r_u32(&r, &sheet_len) || !r_u32(&r, &cols) || !r_u32(&r, &rows)) {
        sx_error_value = SX_ERR_INPUT;
        return 0u;
    }
    if (!cols || cols > SX_MAX_COLUMNS || rows > 100000u || report_len > 4096u || sheet_len > 128u) {
        sx_error_value = SX_ERR_RANGE;
        return 0u;
    }

    const uint8_t *report = 0, *sheet = 0;
    if (!r_bytes(&r, report_len, &report) || !r_bytes(&r, sheet_len, &sheet)) {
        sx_error_value = SX_ERR_INPUT;
        return 0u;
    }

    for (uint32_t c = 0; c < cols; ++c) {
        uint8_t type = 0;
        uint32_t name_len = 0;
        const uint8_t *name = 0;
        if (!r_u8(&r, &type) || !r_u32(&r, &name_len) || !name_len || !r_bytes(&r, name_len, &name)) {
            sx_error_value = SX_ERR_SCHEMA;
            return 0u;
        }
        columns[c].declared_type = type;
        columns[c].name = name;
        columns[c].name_len = name_len;
    }

    Reader cells = r;
    Reader validate = r;
    for (uint32_t i = 0; i < rows * cols; ++i) {
        uint8_t tag = 0;
        uint32_t n = 0;
        const uint8_t *bytes = 0;
        if (!r_u8(&validate, &tag) || !r_u32(&validate, &n) || !r_bytes(&validate, n, &bytes)) {
            sx_error_value = SX_ERR_INPUT;
            return 0u;
        }
        if (tag > 5u) {
            sx_error_value = SX_ERR_SCHEMA;
            return 0u;
        }
    }
    if (validate.p != validate.end) {
        sx_error_value = SX_ERR_INPUT;
        return 0u;
    }

    if (!write_content_types() || !write_root_rels() || !write_core(report, report_len) ||
        !write_app() || !write_workbook(sheet, sheet_len) || !write_workbook_rels() ||
        !write_worksheet(&cells, rows, cols) || !write_provenance(report, report_len, sheet, sheet_len, r, rows, cols) ||
        !zip_finish()) {
        if (sx_error_value == SX_OK) sx_error_value = SX_ERR_OUTPUT;
        return 0u;
    }
    return sx_out_len_value;
}
