#!/usr/bin/env python3
import binascii
import sys
import zipfile
import xml.etree.ElementTree as ET
from pathlib import Path

EXPECTED_PARTS = {
    "[Content_Types].xml", "_rels/.rels", "docProps/core.xml", "docProps/app.xml",
    "xl/workbook.xml", "xl/_rels/workbook.xml.rels", "xl/worksheets/sheet1.xml", "customXml/item1.xml"
}

def verify(path: Path):
    with zipfile.ZipFile(path) as z:
        bad = z.testzip()
        if bad:
            raise AssertionError(f"CRC failure in {bad}")
        names = set(z.namelist())
        missing = EXPECTED_PARTS - names
        if missing:
            raise AssertionError(f"missing XLSX parts: {sorted(missing)}")
        for name in EXPECTED_PARTS:
            ET.fromstring(z.read(name))
        return z.read("xl/worksheets/sheet1.xml"), z.read("customXml/item1.xml")

nul_sheet, nul_prov = verify(Path(sys.argv[1]))
shape_sheet, shape_prov = verify(Path(sys.argv[2]))
root = ET.fromstring(nul_prov)
ns = {"p": "urn:redogit:serveexcel:worddna:1"}
entries = root.findall("p:worddna", ns)
needle = b"alpha\x00omega"
found = False
for e in entries:
    raw = binascii.unhexlify(e.attrib["bytes-hex"])
    if raw == needle:
        found = True
        assert e.attrib["byte-count"] == str(len(needle))
        break
if not found:
    raise AssertionError("exact embedded-NUL bytes not recovered from WordDNA provenance")
if b"_x0000_" not in nul_sheet:
    raise AssertionError("worksheet projection did not encode embedded NUL")
for header in (b"name", b"score", b"active", b"note"):
    if header not in shape_sheet:
        raise AssertionError(f"shape-derived header missing: {header!r}")
print("PASS: ZIP CRC/XML structure, exact embedded-NUL provenance round-trip, changing schema")
