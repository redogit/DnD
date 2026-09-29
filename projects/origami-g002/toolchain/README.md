# Origami-local numeric RMALC fork

This directory is the 1.3.0 package's project-local `3.1.0-c23-origami-numeric` source, derived from DnD commit `4282676da043cca93b1622532d94ae419d9795ab`. Origami's build script uses this directory explicitly. It does not replace or extend the repository-root RMALC by implication.

The fork adds floating-point values and a host callback ABI for Origami's checked numeric buffers and scalar functions. The root language/VM remains the canonical DnD implementation. Compare, test and decide any future upstreaming separately. See `../README.md` for the current evidence boundary.

`CURRENT.md` and `README_COMPILER.md` are preserved upstream snapshots. Their standalone Windows script paths are absent from this isolated source import; their CMake commands cover compiler checks rather than the Origami host and runner. Build Origami using `../scripts/build.py` with Zig, or use the explicitly non-official GCC 13 Linux audit fallback in `../scripts/build-gcc13-audit.sh`. Their historical upstream Windows CI receipts do not certify Origami 1.3.0 runtime execution on Windows.
