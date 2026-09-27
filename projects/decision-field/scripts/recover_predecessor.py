#!/usr/bin/env python3
"""Recover the exact attached v0.3 archive without overwriting another version.

Usage: python recover_predecessor.py ARCHIVE [DESTINATION]
Default destination: projects/decision-field/predecessor/v0_3.
No network access, guessed source, or current-directory cleanup is performed.
"""
from __future__ import annotations
import argparse
import hashlib
from pathlib import Path, PurePosixPath
import shutil
import tempfile
import zipfile

EXPECTED = "91a25de44269e6f54d45c5744b38af09eef2daf25b8fedbdab79ca0b4a018292"
PREFIX = "decision_field_v0_3_package"

def recover(archive: Path, destination: Path) -> None:
    data = archive.read_bytes()
    if hashlib.sha256(data).hexdigest() != EXPECTED:
        raise ValueError("Archive hash mismatch; do not substitute another version")
    if destination.exists():
        raise FileExistsError(f"Destination already exists: {destination}")
    destination.parent.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory(prefix="df-recovery-", dir=destination.parent) as temp:
        staged = Path(temp) / "source"
        staged.mkdir()
        with zipfile.ZipFile(archive) as z:
            for info in z.infolist():
                path = PurePosixPath(info.filename)
                if path.is_absolute() or ".." in path.parts or not path.parts or path.parts[0] != PREFIX:
                    raise ValueError("Unexpected archive path")
                if (info.external_attr >> 16) & 0o170000 == 0o120000:
                    raise ValueError("Archive symlinks are not accepted")
                relative = Path(*path.parts[1:])
                target = staged / relative
                if info.is_dir():
                    target.mkdir(parents=True, exist_ok=True)
                else:
                    target.parent.mkdir(parents=True, exist_ok=True)
                    target.write_bytes(z.read(info))
        staged.rename(destination)
    print(f"Recovered exact v0.3 source to {destination}")

def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("archive", type=Path)
    parser.add_argument("destination", nargs="?", type=Path,
                        default=Path(__file__).resolve().parents[1] / "predecessor" / "v0_3")
    args = parser.parse_args()
    recover(args.archive, args.destination)

if __name__ == "__main__":
    main()
