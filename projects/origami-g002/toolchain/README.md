# Origami-local numeric RMALC fork

This directory is the 1.3.0 package's project-local `3.1.0-c23-origami-numeric` source, derived from DnD commit `4282676da043cca93b1622532d94ae419d9795ab`. Origami's build script uses this directory explicitly. It does not replace or extend the repository-root RMALC by implication.

The fork adds floating-point values and a host callback ABI for Origami's checked numeric buffers and scalar functions. The root language/VM remains the canonical DnD implementation. Compare, test and decide any future upstreaming separately. See `../README.md` for the current evidence boundary.
