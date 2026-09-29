# 13D Origami specification validation

Audit: 2026-09-27. Source originals are preserved byte-for-byte. Statuses apply to scoped claims, not entire research programs. Unknown is not a declaration of impossibility.

{'verified': 8, 'falsified': 11, 'unknown': 17}

The updated app uses exact ideal Miura geometry, a passive ABCD circuit estimate, and a reversible normalized 13D vector rotation with retained remainder. These are explicit reduced models; the specification is not established as production-ready.

## Critical electrical correction

AD633 PDIP: 1 X1, 2 X2, 3 Y1, 4 Y2, 5 −VS, 6 Z, 7 W, 8 +VS. SOIC: 1 Y1, 2 Y2, 3 −VS, 4 Z, 5 W, 6 +VS, 7 X1, 8 X2. The package must be selected before layout. Z is the summing input. [Manufacturer datasheet](https://www.analog.com/media/en/technical-documentation/data-sheets/ad633.pdf).

## Claim ledger

### C01 — FALSIFIED

The compact state-vector grouping agrees with the explicit dimension list.

**Result:** The compact groups are 3+4+4+2; the enumerated groups are 3 spatial + 4 kinematic + 5 spectral + 1 coupling. Both total 13, but their memberships disagree.

**Scope:** The proposed assembly described by SPEC-13D-ORRLA-2026-V5.5, §2

**Method:** Count the named entries in each group; preserve the explicit D1–D13 list.

**Limits:** Documentary or mathematical evidence only; no assembled-device validation.

Sources: [Original specification, V5.5](13D_Origami_Circuit_Full_Specification.txt)

### C02 — VERIFIED

The document defines six functional circuit layers.

**Result:** Six assignments are explicitly present and are reproduced in the layer guide.

**Scope:** Document content only, not actual fabrication or circuit function.

**Method:** Compare the submitted claim with the source and the cited primary documentation; no physical sample was tested.

**Limits:** Material thicknesses and electrical coupling for a manufacturable six-layer stack remain unspecified.

Sources: [Original specification, V5.5](13D_Origami_Circuit_Full_Specification.txt)

### C03 — UNKNOWN

The central multiplier/gyrator core produces a gravitational funnel, metric warping or an event horizon.

**Result:** No metric, field equations, analog-equivalence model or experiment is supplied. These remain visualization metaphors.

**Scope:** The proposed assembly described by SPEC-13D-ORRLA-2026-V5.5, §1, §5

**Method:** Compare the submitted claim with the source and the cited primary documentation; no physical sample was tested.

**Limits:** Documentary or mathematical evidence only; no assembled-device validation.

Sources: [Original specification, V5.5](13D_Origami_Circuit_Full_Specification.txt)

### C04 — UNKNOWN

Folding produces the claimed semantic rotations without delamination.

**Result:** No operational semantic transform, material stress model or delamination test is provided.

**Scope:** The proposed assembly described by SPEC-13D-ORRLA-2026-V5.5, §1

**Method:** Compare the submitted claim with the source and the cited primary documentation; no physical sample was tested.

**Limits:** Documentary or mathematical evidence only; no assembled-device validation.

Sources: [Original specification, V5.5](13D_Origami_Circuit_Full_Specification.txt), [Minco flex-circuit engineering FAQs](https://www.minco.com/resource-center/circuit-faqs/)

### C05 — UNKNOWN

The 13 named channels establish 13 independent physical or mechanical degrees of freedom.

**Result:** A named state vector is admissible, but its feasible configuration dimension is not established. Ideal Miura folding couples its crease angles.

**Scope:** The proposed assembly described by SPEC-13D-ORRLA-2026-V5.5, §2

**Method:** Compare the submitted claim with the source and the cited primary documentation; no physical sample was tested.

**Limits:** This does not prohibit a constrained 13-channel state representation.

Sources: [Original specification, V5.5](13D_Origami_Circuit_Full_Specification.txt), [Schenk & Guest, Geometry of Miura-folded metamaterials (2013)](https://www.pnas.org/doi/10.1073/pnas.1217998110)

### C06 — FALSIFIED

A general linear 13D state can be recovered from only three projected coordinates.

**Result:** A 3×13 matrix has nullity at least 10. Two inputs differing in a hidden coordinate collide under the displayed projection. The update retains those ten coordinates.

**Scope:** The general linear lossless-reduction interpretation of the source, without a restricted input family or retained side data.

**Method:** Rank–nullity and the executable collision/reconstruction checks in tests/engineering.test.cjs.

**Limits:** Restricted low-dimensional families and retained remainder are different cases.

Sources: [Original specification, V5.5](13D_Origami_Circuit_Full_Specification.txt), [Lundholm & Svensson, Clifford algebra, geometric algebra, and applications](https://www.math.lmu.de/~lundholm/clifford.pdf)

### C07 — VERIFIED

AD633 is a four-quadrant analog multiplier.

**Result:** Its nominal transfer is W=(X1−X2)(Y1−Y2)/(10 V)+Z.

**Scope:** Manufacturer component property within datasheet conditions.

**Method:** Compare the submitted claim with the source and the cited primary documentation; no physical sample was tested.

**Limits:** Documentary or mathematical evidence only; no assembled-device validation.

Sources: [Analog Devices AD633 Rev K, pp. 3 and 5](https://www.analog.com/media/en/technical-documentation/data-sheets/ad633.pdf)

### C08 — FALSIFIED

The AD633 can directly process the specified 2.4–5.8 GHz microwave buses.

**Result:** Its specified small-signal bandwidth is 1 MHz. A baseband/control role would require a different circuit architecture.

**Scope:** Direct microwave-processing interpretation of the specified AD633 core.

**Method:** Compare the submitted claim with the source and the cited primary documentation; no physical sample was tested.

**Limits:** Documentary or mathematical evidence only; no assembled-device validation.

Sources: [Analog Devices AD633 Rev K, pp. 3 and 5](https://www.analog.com/media/en/technical-documentation/data-sheets/ad633.pdf)

### C09 — FALSIFIED

The given AD633 pin assignments are correct.

**Result:** PDIP pins 5 and 6 are swapped: 5 is −VS, 6 is Z. SOIC uses a different complete mapping; package selection is required.

**Scope:** The proposed assembly described by SPEC-13D-ORRLA-2026-V5.5, §10

**Method:** Compare the submitted claim with the source and the cited primary documentation; no physical sample was tested.

**Limits:** Correct pinouts appear in the report; this is not a PCB layout sign-off.

Sources: [Analog Devices AD633 Rev K, pp. 3 and 5](https://www.analog.com/media/en/technical-documentation/data-sheets/ad633.pdf)

### C10 — VERIFIED

±15 V is an allowed nominal AD633 supply.

**Result:** The rated test supply is ±15 V; the operating range is ±8 to ±18 V.

**Scope:** Static supply rating, not dynamic bias or microwave performance.

**Method:** Compare the submitted claim with the source and the cited primary documentation; no physical sample was tested.

**Limits:** Documentary or mathematical evidence only; no assembled-device validation.

Sources: [Analog Devices AD633 Rev K, pp. 3 and 5](https://www.analog.com/media/en/technical-documentation/data-sheets/ad633.pdf)

### C11 — VERIFIED

MAVR-000120-14110P identifies a MACOM GaAs tuning varactor.

**Result:** The manufacturer lists the part; the suffix selects pocket tape. Capacitance and parasitics still require a bias-dependent model.

**Scope:** The proposed assembly described by SPEC-13D-ORRLA-2026-V5.5, §6, §7

**Method:** Compare the submitted claim with the source and the cited primary documentation; no physical sample was tested.

**Limits:** Component identity does not validate the proposed tuning mechanism.

Sources: [MACOM MAVR-000120-1411 Rev V8](https://cdn.macom.com/datasheets/MAVR-000120-1411.pdf)

### C12 — VERIFIED

Pyralux FR9111R has the named copper/polyimide constituents.

**Result:** Table 2 lists double-sided 35 µm copper, 25 µm acrylic adhesive, and 25 µm Kapton. The source omits adhesive and does not define the full six-layer construction.

**Scope:** Product identity and listed laminate constituents.

**Method:** Compare the submitted claim with the source and the cited primary documentation; no physical sample was tested.

**Limits:** FR9110R electrical typicals must not be silently assigned to this assembly.

Sources: [Pyralux FR manufacturer datasheet, Table 2](https://www.qnityelectronics.com/content/dam/electronics/amer/us/en/electronics/public/documents/en/EI-10113-Pyralux-FR-CCL-Data-Sheet.pdf)

### C13 — UNKNOWN

The proposed substrate tolerates repeated 180° folds without fatigue.

**Result:** No cycle count, bend radius, total construction or durability evidence establishes this guarantee.

**Scope:** The proposed assembly described by SPEC-13D-ORRLA-2026-V5.5, §3

**Method:** Compare the submitted claim with the source and the cited primary documentation; no physical sample was tested.

**Limits:** Documentary or mathematical evidence only; no assembled-device validation.

Sources: [Pyralux FR manufacturer datasheet, Table 2](https://www.qnityelectronics.com/content/dam/electronics/amer/us/en/electronics/public/documents/en/EI-10113-Pyralux-FR-CCL-Data-Sheet.pdf), [Minco flex-circuit engineering FAQs](https://www.minco.com/resource-center/circuit-faqs/)

### C14 — UNKNOWN

Murata LBR-series GHz ceramic resonators are a verified BOM entry.

**Result:** The checked official ceramic-resonator lineup does not verify this exact series/role. An exact part number and microwave model are needed.

**Scope:** The proposed assembly described by SPEC-13D-ORRLA-2026-V5.5, §7

**Method:** Compare the submitted claim with the source and the cited primary documentation; no physical sample was tested.

**Limits:** Failed identification is not proof that a product never existed; no substitute has been selected.

Sources: [Murata CERALOCK product lineup](https://www.murata.com/en-global/products/timingdevice/ceralock/overview/lineup/lineupgen)

### C15 — UNKNOWN

DuPont Intex 5000 is the specified qualified RF crease ink.

**Result:** The exact designation was not verified. Related Intexar PE874 documentation does not qualify the named ink or a GHz folding hinge.

**Scope:** The proposed assembly described by SPEC-13D-ORRLA-2026-V5.5, §7

**Method:** Compare the submitted claim with the source and the cited primary documentation; no physical sample was tested.

**Limits:** PE874 is a related manufacturer document, not an admitted replacement or proof of nonexistence.

Sources: [DuPont Intexar PE874 manufacturer datasheet](https://www.ccieurolam.com/wp-content/uploads/PE874-TDS.pdf)

### C16 — UNKNOWN

A 52 µm trace width guarantees 50 Ω ±2 Ω for this assembly.

**Result:** Required dielectric separations, adhesive/coverlay, permittivity and complete conductor geometry are missing. Width alone cannot establish the guarantee.

**Scope:** The proposed assembly described by SPEC-13D-ORRLA-2026-V5.5, §7

**Method:** Compare the submitted claim with the source and the cited primary documentation; no physical sample was tested.

**Limits:** No exact stack impedance has been computed or measured.

Sources: [Rogers RF design tools](https://www.rogerscorp.com/advanced-electronics-solutions/tools), [Original specification, V5.5](13D_Origami_Circuit_Full_Specification.txt)

### C17 — UNKNOWN

The 250 µm keepout and 12.5 µm score depth are production-qualified.

**Result:** These are proposed dimensions. There is no process tolerance, cross-section, damage inspection or hinge qualification.

**Scope:** The proposed assembly described by SPEC-13D-ORRLA-2026-V5.5, §7

**Method:** Compare the submitted claim with the source and the cited primary documentation; no physical sample was tested.

**Limits:** Documentary or mathematical evidence only; no assembled-device validation.

Sources: [Original specification, V5.5](13D_Origami_Circuit_Full_Specification.txt), [Minco flex-circuit engineering FAQs](https://www.minco.com/resource-center/circuit-faqs/)

### C18 — UNKNOWN

The assembly meets >10 MΩ DC isolation.

**Result:** This is an acceptance target without a test voltage, environment or measured result.

**Scope:** The proposed assembly described by SPEC-13D-ORRLA-2026-V5.5, §7

**Method:** Compare the submitted claim with the source and the cited primary documentation; no physical sample was tested.

**Limits:** Documentary or mathematical evidence only; no assembled-device validation.

Sources: [Original specification, V5.5](13D_Origami_Circuit_Full_Specification.txt)

### C19 — FALSIFIED

The literal supplied netlist is a complete executable S21 simulation.

**Result:** The transmission lines lack four node terminals, and the diode model, active subcircuit, ports, bias and analysis are missing. The update supplies a separate passive reduction.

**Scope:** The proposed assembly described by SPEC-13D-ORRLA-2026-V5.5, §6

**Method:** Compare literal declarations to documented ngspice syntax; inspect missing model definitions.

**Limits:** The repaired reference netlist has not been executed by ngspice in this environment.

Sources: [ngspice manual, transmission lines and component definitions](https://ngspice.sourceforge.io/docs/ngspice-manual.pdf), [Original specification, V5.5](13D_Origami_Circuit_Full_Specification.txt)

### C20 — FALSIFIED

The supplied HTML calculates physical S21 in real time.

**Result:** The HTML only interpolates frequency and polarization with sin(θ); it contains no scattering/network solver.

**Scope:** The proposed assembly described by SPEC-13D-ORRLA-2026-V5.5, original HTML

**Method:** Inspect the frequency/polarization assignments and all JavaScript in the original file.

**Limits:** Documentary or mathematical evidence only; no assembled-device validation.

Sources: [Original HTML prototype](13d_origami_circuit_system.html)

### C21 — UNKNOWN

Mechanical folding establishes a 2.4→5.8 GHz operating transition.

**Result:** No measured sweep, electromagnetic solution or fold-to-component law supports this transition. The source values remain visible as targets.

**Scope:** The proposed assembly described by SPEC-13D-ORRLA-2026-V5.5, §6

**Method:** Compare the submitted claim with the source and the cited primary documentation; no physical sample was tested.

**Limits:** Documentary or mathematical evidence only; no assembled-device validation.

Sources: [Original specification, V5.5](13D_Origami_Circuit_Full_Specification.txt), [MACOM MAVR-000120-1411 Rev V8](https://cdn.macom.com/datasheets/MAVR-000120-1411.pdf)

### C22 — FALSIFIED

Insertion loss <−1.5 dB expresses a low-loss ceiling.

**Result:** For IL=−20log10|S21|, a 1.5 dB ceiling is IL<1.5 dB, equivalently S21_dB>−1.5 dB with equal real matched references.

**Scope:** The conventional positive insertion-loss interpretation of the stated low-loss checkout criterion.

**Method:** Apply the logarithmic amplitude definition; an example |S21|=0.5 has positive loss 6.0206 dB.

**Limits:** Documentary or mathematical evidence only; no assembled-device validation.

Sources: [Original specification, V5.5](13D_Origami_Circuit_Full_Specification.txt), [MathWorks ABCD-to-S parameter conversion](https://www.mathworks.com/help/rf/ref/abcd2s.html)

### C23 — UNKNOWN

Maximum compression produces a measured 90° microwave polarization rotation.

**Result:** The prototype computes 90 sin(θ). No field, polarization measurement or calibration supports the hardware result.

**Scope:** The proposed assembly described by SPEC-13D-ORRLA-2026-V5.5, §7

**Method:** Compare the submitted claim with the source and the cited primary documentation; no physical sample was tested.

**Limits:** Documentary or mathematical evidence only; no assembled-device validation.

Sources: [Original specification, V5.5](13D_Origami_Circuit_Full_Specification.txt), [Original HTML prototype](13d_origami_circuit_system.html)

### C24 — UNKNOWN

The specified mesh and grounding geometry guarantee 60 dB shielding.

**Result:** No assembly-level measurement supports this. Express the proposed target as shielding effectiveness ≥60 dB with reference, incidence and polarization defined.

**Scope:** The proposed assembly described by SPEC-13D-ORRLA-2026-V5.5, §8

**Method:** Compare the submitted claim with the source and the cited primary documentation; no physical sample was tested.

**Limits:** The envelope in the scene is a coarse guide; it is not a mesh or shielding solver.

Sources: [Parker Chomerics shielding theory](https://discover.parker.com/chomerics-tech-info-emi-shielding-theory), [Original specification, V5.5](13D_Origami_Circuit_Full_Specification.txt)

### C25 — FALSIFIED

Inverse-square decay is exponential as a function of radius.

**Result:** r^−2 and exp(−kr) are different functions of r. Substituting a logarithmic spiral can make r^−2 exponential in angle, which is a different variable.

**Scope:** The proposed assembly described by SPEC-13D-ORRLA-2026-V5.5, §4

**Method:** Compare logarithmic derivatives: −2/r versus constant −k.

**Limits:** Documentary or mathematical evidence only; no assembled-device validation.

Sources: [Original specification, V5.5](13D_Origami_Circuit_Full_Specification.txt)

### C26 — UNKNOWN

Resistors and a phase-inverted route establish retrocausal signals and −1.5 V automatically.

**Result:** No negative supply network, transfer function or causal experiment establishes either behavior. Polarity inversion alone does not reverse time.

**Scope:** The proposed assembly described by SPEC-13D-ORRLA-2026-V5.5, §4

**Method:** Compare the submitted claim with the source and the cited primary documentation; no physical sample was tested.

**Limits:** Documentary or mathematical evidence only; no assembled-device validation.

Sources: [Original specification, V5.5](13D_Origami_Circuit_Full_Specification.txt)

### C27 — FALSIFIED

The ordinary Hopf map is an invertible full-state 13D→3D encoder.

**Result:** The ordinary Hopf map is S³→S² with circle fibers: distinct inputs share an image. The source specifies no different reversible construction.

**Scope:** The ordinary Hopf-map interpretation; not every possible custom construction.

**Method:** Compare the submitted claim with the source and the cited primary documentation; no physical sample was tested.

**Limits:** A retained fiber coordinate or other side data changes the encoding problem.

Sources: [Lyons, An Elementary Introduction to the Hopf Fibration](https://nilesjohnson.net/hopf-articles/Lyons_Elem-intro-Hopf-fibration.pdf), [Original specification, V5.5](13D_Origami_Circuit_Full_Specification.txt)

### C28 — UNKNOWN

Mathematical reversibility establishes zero entropy generation in this powered circuit.

**Result:** No thermodynamic model is supplied. Logical invertibility alone does not imply thermodynamic reversibility; a positive resistor with nonzero current dissipates I²R.

**Scope:** The proposed assembly described by SPEC-13D-ORRLA-2026-V5.5, §5

**Method:** Compare the submitted claim with the source and the cited primary documentation; no physical sample was tested.

**Limits:** Ideal reversible-computation limits do not certify this proposed active analog device.

Sources: [Bennett, Notes on Landauer’s principle and reversible computation](https://arxiv.org/abs/physics/0210005), [Original specification, V5.5](13D_Origami_Circuit_Full_Specification.txt)

### C29 — FALSIFIED

An exceptional point alone forces a static, nonvolatile latch.

**Result:** Counterexample: at γ=k>0, H=[[iγ,k],[k,−iγ]] is defective with H²=0, yet exp(−iHt)=I−iHt evolves with time. No retention mechanism is specified.

**Scope:** The asserted general implication from exceptional-point operation to static nonvolatile memory.

**Method:** Exact 2×2 matrix multiplication and exponential truncation.

**Limits:** A specially designed memory with additional mechanisms is not excluded.

Sources: [Original specification, V5.5](13D_Origami_Circuit_Full_Specification.txt)

### C30 — FALSIFIED

The original HTML prototype implements the specification’s Cl(13,0) multivector propagation.

**Result:** Cl(13,0) has 2¹³=8,192 basis blades; its vector subspace has 13. The original has neither rotor algebra nor multivector propagation.

**Scope:** Original supplied HTML implementation only; not all possible interpretations of the proposed architecture.

**Method:** Count ordered basis subsets; inspect original implementation.

**Limits:** The updated normalized vector-plane rotations are only grade-1 rotor action.

Sources: [Lundholm & Svensson, Clifford algebra, geometric algebra, and applications](https://www.math.lmu.de/~lundholm/clifford.pdf), [Original HTML prototype](13d_origami_circuit_system.html)

### C31 — UNKNOWN

A unit rotor establishes material permittivity tracking and lossless RF behavior.

**Result:** Norm preservation of normalized coordinates is mathematical; material response, bandwidth, noise and dielectric modulation need separate evidence.

**Scope:** The proposed assembly described by SPEC-13D-ORRLA-2026-V5.5, §9

**Method:** Compare the submitted claim with the source and the cited primary documentation; no physical sample was tested.

**Limits:** Documentary or mathematical evidence only; no assembled-device validation.

Sources: [Lundholm & Svensson, Clifford algebra, geometric algebra, and applications](https://www.math.lmu.de/~lundholm/clifford.pdf), [Original specification, V5.5](13D_Origami_Circuit_Full_Specification.txt)

### C32 — UNKNOWN

The original three-state Lorenz feed establishes hyperchaos.

**Result:** The code implements the conventional three-state Lorenz equations and presents no Lyapunov-spectrum evidence for the stronger label. It is labeled Lorenz attractor in the app.

**Scope:** The proposed assembly described by SPEC-13D-ORRLA-2026-V5.5, original HTML

**Method:** Compare the submitted claim with the source and the cited primary documentation; no physical sample was tested.

**Limits:** No Lyapunov spectrum is computed here.

Sources: [Original HTML prototype](13d_origami_circuit_system.html)

### C33 — UNKNOWN

The specification is ready for production execution.

**Result:** Its approval label is not a test result. Circuit models, BOM identities, stack geometry, manufacturing tolerances and physical acceptance evidence remain incomplete.

**Scope:** The proposed assembly described by SPEC-13D-ORRLA-2026-V5.5, document header

**Method:** Compare the submitted claim with the source and the cited primary documentation; no physical sample was tested.

**Limits:** Documentary or mathematical evidence only; no assembled-device validation.

Sources: [Original specification, V5.5](13D_Origami_Circuit_Full_Specification.txt)

### V01 — VERIFIED

The updated ideal Miura tessellation preserves rigid parallelogram facets.

**Result:** Edge lengths, diagonals, angles, area, closure and planarity are preserved by the coordinate construction; bounded numerical sweeps corroborate the identities.

**Scope:** Zero-thickness, one-parameter mathematical sheet; arbitrary visualization length scale.

**Method:** Analytic local edges u=(L,0,±h), v=(±s,W,0): |u|=a, |v|=b, u·v=±ab cosγ. Run tests/engineering.test.cjs and research/verify_miura.py.

**Limits:** No finite-thickness collision, stress, fatigue, laminate or material validation.

Sources: [Schenk & Guest, Geometry of Miura-folded metamaterials (2013)](https://www.pnas.org/doi/10.1073/pnas.1217998110)

### V02 — VERIFIED

The updated normalized 13D plane rotation is recoverable when the remainder is retained.

**Result:** The 2×2 rotation block is orthogonal. Negating its angle restores the full vector; the app retains all 13 rotated coordinates and the ten-coordinate remainder.

**Scope:** Finite dimensionless vectors, declared plane/angle, and retained full output.

**Method:** Algebra RᵀR=I plus all 78 distinct coordinate-plane round trips in tests/engineering.test.cjs.

**Limits:** This is not a full multivector simulator, physical energy conservation or a lossless 3D-only image.

Sources: [Lundholm & Svensson, Clifford algebra, geometric algebra, and applications](https://www.math.lmu.de/~lundholm/clifford.pdf)

### V03 — VERIFIED

The explicit passive reduction produces a notch near 3.9181 GHz at its default L/C.

**Result:** For L=2.2 nH and C=0.75 pF, 1/(2π√LC)=3.918123848 GHz. The shunt series LC shorts its junction ideally; the supplied branch is a transmission notch.

**Scope:** Only the documented ideal passive topology, 50 Ω references and default components.

**Method:** ABCD calculation independently checked by nodal admittance at 1,501 frequencies; tests also check the exact-notch limit.

**Limits:** No varactor, gain, parasitic, EM, fold-dependent or measured-device behavior is admitted.

Sources: [MathWorks ABCD-to-S parameter conversion](https://www.mathworks.com/help/rf/ref/abcd2s.html)

## Reproduction and remaining work

Run `node tests/model.test.cjs`, `node tests/engineering.test.cjs`, `node tests/static.test.cjs`, the linkedom interaction harness, and the two Python research checks. The Python RF check is independent of the JavaScript cascade. Test evidence is not hardware certification.

Needed for the next physical validation: a complete package-specific schematic with validated models; a specified six-layer cross-section; a fold-dependent EM/circuit model or measured sweep; calibrated S-parameters; and bend/insulation/shielding coupons with explicit acceptance conditions. Browser/GPU visual QA and an actual ngspice execution are not claimed.
