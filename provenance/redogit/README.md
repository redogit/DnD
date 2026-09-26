# redogit

> **About me:** [PUBLIC ABOUT ME](https://redogit.github.io/redogit/about.html) — fire, carriers, music/language, REDOGIT, and current work
> **Newest integrated work:** [PUBLIC RECENT WORK](https://redogit.github.io/redogit/recent-work.html) · [source](RECENT_WORK_2026-09-25.md)
> **Start here — terms before claims:** [PUBLIC RMAL ORIENTATION](https://redogit.github.io/redogit/) · [source](RMAL_RESEARCH_ORIENTATION_2026-09-25.md)
> **Algorithmic method / learned rules:** [PUBLIC ALGORITHMIC CONDENSED WISDOM](https://redogit.github.io/redogit/condensed-wisdom.html) · [source](ALGORITHMIC_CONDENSED_WISDOM_ANALYSIS_2026-09-25.md)
> **Current live design:** [PUBLIC RMAL P vs NP — LIVE DESIGN](https://redogit.github.io/redogit/live-design.html) · [source](RMAL_PNP_LIVE_DESIGN_2026-09-25.md)

> **Conscience64 public projection — updated 2026-09-25:** the generated public testbed remains isolated, and **Musilanguage Studio is now the one explicitly restored public Play application** at https://redogit.github.io/conscience64/play/musilanguage/. The rest of `play/`, repository internals, private-origin carriers, and historical predecessor music pages are not thereby published. Public projection != canonical authority.


## About me

I did not get here by starting with complexity theory, reading the P versus NP problem, and deciding I was going to prove `P = NP`.

I got here sideways.

I’m a data specialist, reverse engineer, builder, and mostly a random guy with a knack for ideation, imagination, pattern matching, and being stubbornly honest when something does **not** work. I usually take a problem apart until I can see what actually has to remain true, strip away the names that are not doing any work, keep the relations that are, and then ask what the smallest next change has to be.

The first clue did not come from P versus NP at all. It came from practical education-sector algorithms. I was trying to serialize algorithms and their data properly. I found that when I stripped away the domain semantics and kept a minimal representation of the data, the relations, and the transforms it went through, the same machinery could survive outside the problem it was written for.

My own rough description at the time was basically:

```text
minimal representation
+ algorithm
+ stripped semantics
+ relations between the data
+ the transforms it went through
```

That was the beginning.

I was not asking, “How do I prove P = NP?”

I was asking things like:

```text
What was this solver actually for?
What obligation was it supposed to satisfy?
What did it learn by solving one case?
Can I describe why and how it worked faithfully enough to reuse it?
What is still unresolved?
What is the smallest thing I have to change next?
```

The direction that kept surviving was:

```text
OBLIGATION
→ MINIMAL REPRESENTATION
→ RELATIONS
→ TRANSFORMS
→ RESULT
→ WHY THE RESULT IS VALID
→ WHAT REMAINS
→ REUSE WHAT WAS LEARNED
```

I kept finding that the names of things could change while the relational behavior stayed useful. So I started treating the **reason for the solver + the knowledge needed to describe it faithfully** as part of the thing being solved, not as documentation added afterward.

The housing / Dean problem came later. I had my own messy formulation about selecting students under exclusions, capacity, and an output requirement. I treated it as a practical obligation: get the requested valid result, preserve the constraints that actually matter, and do not silently lose the reason the answer is valid.

Only later did that line up with Independent Set, NP-completeness, the Dean-style example, and eventually Stephen Cook’s formal statement of the P versus NP problem.

I did not begin by reading Cook’s paper. I read it **after** following this path far enough that the formal language started looking uncomfortably familiar. Cook defines NP through a polynomial-time checking relation and asks whether every problem with that kind of efficiently checkable witness also has a deterministic polynomial-time solution. That is very close to the boundary I had reached from the other direction: start with the obligation and the witness/output, preserve exactly what makes it valid, and ask whether the path that produces it can be made as disciplined as the path that checks it.

Stephen Cook’s official Clay problem description is here:

[The P Versus NP Problem — Stephen Cook](https://www.claymath.org/wp-content/uploads/2022/06/pvsnp.pdf)

The weird part for me is that getting this far was not mostly calculation.

The calculations matter. The code matters. The proofs of the small pieces matter. The searches matter. But the route here was mostly **deductive reasoning over the mathematics of the problem**:

```text
state the obligation
→ preserve the exact distinctions that can change it
→ derive what follows
→ change one degree
→ repair only what that change damages
→ keep the negative result if it fails
→ reconstruct the way back
→ continue
```

That is how I ended up working with things I did not set out to study: Independent Set, matching, matroids, gammoids, series-parallel graphs, GF(2), odd-cycle structure, roof duality, fixed points, proof certificates, codecs, and complexity bounds.

I did not pick those subjects because I wanted a collection of advanced words. They kept appearing because the previous step forced the next one.

That distinction matters to me.

I am not claiming that I have solved P versus NP. I have a proof program, a growing set of scoped exact results, a lot of preserved failures, and a much narrower universal remainder than I started with. Until the universal polynomial step is actually proved, `P ?= NP` remains open.

If this does eventually close, the story will not be that I sat down knowing the right mathematics and calculated my way to an answer.

It will be that I kept asking what the problem was actually obligated to do, refused to throw away failed information, kept representations separate from truth, changed one thing at a time, and followed the deductions wherever they went.

That is also how I work outside mathematics.

```text
DO THE THING
→ INSPECT WHAT ACTUALLY HAPPENED
→ KEEP WHAT SURVIVED
→ REPAIR THE SMALLEST FAILURE
→ DO IT AGAIN
```

That is REDOGIT.

## Musilanguage Studio — public again

[Open Musilanguage Studio](https://redogit.github.io/conscience64/play/musilanguage/)

Musilanguage is now one public application rather than separate current single / radio / Word Forge surfaces.

```text
language / symbols / emoji / exact UTF-8
→ MUSIC64
→ 64 arrangement profiles
→ guitar / bass / keys-harp / strings / lead / drums
→ tempo / meter / mode / tonic / motif transform
→ reversible variations
→ playback / MIDI / WAV / session
→ preserved history inside the app
```

The older **Carry the Fire**, **Musilanguage Radio**, and **Word Forge / MUSIC64** surfaces remain preserved in repository and Git history. The unified Studio carries their lineage forward rather than erasing it.

```text
ONE PUBLIC APP != DELETION OF HISTORY
LANGUAGE-TO-MUSIC != SEMANTIC TRUTH
STYLE PROFILE != CULTURAL AUTHENTICITY
INSTRUMENT MIX != SOURCE MUTATION
```

## September 25, 2026 — P vs NP proof-program checkpoint

The current P-versus-NP research line is being carried forward as an **exact obligation-preserving proof program**, not as a solved claim. The working Dean/Independent-Set obligation is used as the concrete NP-complete carrier.

Current proof architecture:

```text
ORIGINAL OBLIGATION
→ four-ID semantic catalog
→ bounded-arity exact facts
→ one signed degree of change
→ minimal dynamic repair
→ Homeward reconstruction
→ verified residual
```

Each semantic object/relation is tracked through four distinct ID roles:

```text
OccurrenceID
SemanticObjectID
SemanticContextSupportsIDs
SemanticClarityIDs
```

The active anti-decay rule is:

```text
OBJECT / OBLIGATION STAYS
CARRIER MAY MOVE
MOVE AWAY FROM IRREVERSIBLE KNOWLEDGE LOSS
```

The current one-degree progress carrier maintains a certified interval

```text
L <= alpha(G) <= U
```

for the graph independence number. A positive move is an exact augmentation `L -> L+1`; a negative move is a certified upper-bound improvement `U -> U-1`. Inclusion-minimal augmentation gives an exact +1 move whenever a larger independent set is available, but finding such a move is not yet proved polynomial on arbitrary graphs. Existing exact scoped negative carriers include LP/dual and odd-cycle support-frame bounds.

The semantic-event/storage side has been narrowed substantially: under a fixed finite bounded-arity vocabulary, the number of possible authoritative semantic facts is polynomial in the catalog domain size. This controls storage/fixed-point event growth, but **does not by itself prove obligation completeness**.

Current universal proof obligation:

```text
For every unresolved Dean / Independent-Set state with L < q <= U,
produce in polynomial total cost either:

  (+) a certified exact augmentation L -> L+1

or

  (-) a certified exact upper-bound improvement U -> U-1

while preserving the original obligation, provenance, exactness,
Knowledge-Decay protections, and Homeward witness reconstruction.
```

If that signed-progress theorem is proved uniformly, the gap `U-L` can shrink at most `n` times, yielding a polynomial exact Independent-Set solver and therefore `P = NP`.



Current refinement: relative to a present independent cohort `S`, the candidate-to-`S` support graph defines a **transversal matroid**: a candidate subset is matroid-independent exactly when it can be matched into `S`. A +1 augmentation exists exactly when this matroid has a circuit that is also independent in the candidate-candidate conflict graph. Every such circuit has support deficiency exactly one and replaces its supported members of `S` with one additional candidate. The uniform-transversal special case recovers ordinary size-`k` Stable Set, so this positive augmentation subproblem is itself a genuine hardness locus rather than a hidden matching shortcut.

A new scoped exact carrier is now recorded in [Signed Binary-Cluster Augmentation Carrier](PNP_SIGNED_BINARY_CLUSTER_CARRIER_2026-09-25.md). When the support matroid has an exact binary GF(2) representation of rank r and the candidate conflicts form exactly r disjoint cliques, one arbitrary rainbow transversal either is already dependent or becomes a basis. In basis coordinates, a zero same-color diagonal coordinate gives an immediate positive circuit; otherwise an off-diagonal color-dependency digraph controls the result. A shortest directed cycle constructs a singular rainbow transversal and therefore an exact +1 augmentation, while an acyclic dependency digraph makes every rainbow transversal unit-triangular and certifies that no +1 augmentation exists. This is polynomial inside its stated scope. A separate exhaustive canonical check covered 262,160 rank-2/rank-3 configurations with zero mismatches; that finite check supports the implementation logic but is not the proof.

The same carrier now has an exact **log-cluster-defect extension**. Candidate-conflict vertices that obstruct cluster structure are retained as an explicit exception carrier Z rather than semantically deleted. Stable choices A⊆Z are enumerated, chosen exceptions are contracted in the binary support matroid, and the remaining cluster carrier is solved exactly when its nonempty clique count is at least its residual matroid rank. With |Z|=O(log n), Cluster Vertex Deletion plus the 2^|Z| exact exception branches remains polynomial. Branches with fewer residual cliques than residual rank remain explicitly UNRESOLVED.

The support-side admission is now broader and more concrete. The candidate-to-current-cohort support relation is a transversal matroid, hence a gammoid. Classical matroid theory identifies the **binary gammoids** exactly with graphic matroids of series-parallel networks. Using only the original bipartite support graph, matroid rank remains a maximum-matching computation, including after tracked contractions/deletions. This allows a polynomial one-degree loop/coloop/parallel/series reduction that either constructs a series-parallel graphic carrier or leaves an exact nonbinary support residual. In the binary case, augmentation circuits become ordinary cycles of the reconstructed series-parallel graph. When this carrier fails, the next minimal representability obstruction is a U2,4 minor; the next declared repair degree is GF(2) -> GF(3), with no assumption that the binary rainbow-circuit theorem transfers automatically.

Two adjacent repair directions are now explicitly saturated. Moving the support representation from GF(2) to GF(3) repairs the local U2,4 representability obstruction but does not preserve the binary rainbow-circuit / monochromatic-cocircuit progress theorem; U2,4 is itself the standard nonbinary counterexample. Strong base orderability is also not the missing ingredient, because transversal matroids and gammoids already have it. The live binary-support frontier is therefore restated graphically: after series-parallel reconstruction, +1 augmentation is exactly a simple support-cycle whose edge set is independent in the candidate-conflict graph.

That graphic frontier now has a new exact conflict-side carrier. In each biconnected series-parallel support block, anchoring a non-tree support edge converts a candidate support-cycle into a directed two-terminal SP path. Candidate-conflict edge pairs become forbidden marker pairs on that DAG. When those forbidden pairs are hierarchical/non-crossing in the induced reachability order, the classical polynomial Path-Avoiding-Forbidden-Pairs reductions solve the branch exactly. More generally, build a crossing graph whose vertices are forbidden pairs and whose edges record interlacing pairs; if this crossing graph has a vertex cover of O(log n), enumerate only the two lawful endpoint exclusions for those modulator pairs and solve the remaining hierarchical instance. This is an exact polynomial carrier under the stated guard and is distinct from the earlier small conflict-vertex-cover theorem.

The same SP path branch now has two more exact representations. **Choice-CNF** assigns one Boolean variable to each parallel-composition decision; every support edge is a partial choice signature, and every candidate-conflict pair compiles to one clause forbidding the simultaneous signatures. Safe path existence is exactly Choice-CNF satisfiability, so recognized 2-SAT, Horn, dual-Horn, and beta-acyclic instances reuse their existing polynomial solvers and certificates. Separately, mutually disjoint forbidden pairs satisfying the classical skew-symmetry condition form another polynomial island with a true signed dual: a safe path exists iff there is no forbidden-pair F-cut. These are scoped carrier additions, not a universal SAT reduction.

The binary-transversal support class has now been tightened further using Edmonds' forest-presentation theorem. A circuit in a bipartite forest presentation is exactly a connected selected-candidate/support tree in which every touched support has degree two into the circuit. This exposes a canonical hard core: arbitrary CNF SAT reduces to a radius-two forest presentation with one root candidate, one support per clause, literal-occurrence leaves, same-clause exclusion, and complementary-literal exclusion. A conflict-free support circuit exists exactly when the CNF is satisfiable. A 1,500-instance bounded differential panel had zero mismatches. This is a scope calibration, not a P!=NP claim: it shows that the unresolved difficulty can live entirely in semantic consistency among occurrence choices even when the support carrier is almost trivial.

That clause-star core now has an exact matching-based reduction lane as well. Matching autarkies satisfy every clause they touch and therefore remove those clauses without changing the untouched remainder. The largest matching-lean kernel is polynomially computable. On that kernel, maximal deficiency collapses to the visible value k = clauses - variables. Szeider's exact SAT algorithm runs in O(2^k n^3), so k=O(log n) is another polynomial terminal with either a satisfying assignment or a regular resolution refutation. The unresolved residue is therefore sharper: a matching-lean kernel with superlogarithmic deficiency, where assigning either polarity can lower deficiency but retaining both signed children still costs exponential branching mass.

A second exact merge lane is now admitted before any binary branch: non-increasing Davis-Putnam elimination. If resolving out one variable produces no more clauses than the current formula, the two truth directions are merged into one equisatisfiable successor and the variable is genuinely removed; repeated use is polynomial because variables strictly decrease and clause count never exceeds the starting bound. After alternating this with matching-autarky normalization, the unresolved core is matching-lean, high-deficiency, and DP-growth-positive for every remaining variable. Thus the live obstruction is now exact projection growth, not merely "which truth value should I guess?"

The normalization is now stronger again through **linear autarkies**. For the signed clause-variable matrix M, polynomial-time linear programming gives an exact alternative: either there is a nonzero direction z with Mz>=0, yielding a linear autarky and an exact satisfiability-preserving removal, or the residual has full column rank together with a strictly positive clause weighting y satisfying M^T y=0. In that negative-balance case every variable's weighted positive and negative clause incidences cancel exactly, and the ordinary deficiency m-n equals the dimension of the left-nullspace. This balance certificate is not an UNSAT certificate; it records that the polynomial linear-autarky family is saturated and exposes a new exact residual coordinate.

The same normalized core now has a **signed cofactor-dominance** rule before branching or projection. For a variable x, if the x=0 cofactor logically implies the x=1 cofactor, keep only x=1; if the reverse implication holds, keep only x=0. This removes one Boolean degree exactly and needs no projected resolvents. General implication is not assumed cheap, so the move is admitted only with polynomial receipts such as clause subsumption, unit-resolution, or implication checks inside an already certified tractable carrier.

The linearly-lean balance carrier now has an exact qualitative-matrix interpretation too. For A=M(F)^T, the concrete unit-weight matrix is already strict-central: it has a positive null vector. SAT is equivalent to the existence of some matrix B with the **same sign/zero pattern** that becomes noncentral; a separating vector s with s^T B>0 directly reconstructs a satisfying truth assignment. UNSAT is exactly the stronger robustness statement that every same-sign reweighting remains central, i.e. A is sign-central. A satisfiable witness reweighting needs only polynomial-size integer magnitudes, so a polynomial-length one-coordinate reweighting path always exists—but selecting that path in polynomial total work remains open.

The four-ID catalog compresses that normal form without changing its logic: literal occurrences retain OccurrenceIDs, repeated signed literals share SemanticObjectIDs, clause membership is ContextSupport, and positive/negative polarity is linked by one Clarity/COMPLEMENT relation rather than repeated pairwise conflict storage.

The binary-support seam has now narrowed further: a binary support transversal matroid is a binary gammoid, hence has a K4-minor-free graphic realization; equivalently the support circuit problem can be carried by a **series-parallel graph**. A +1 Dean augmentation is therefore a cycle in that support graph whose candidate edges contain no conflict pair. This shifts the remaining difficulty from the support matroid itself to the coupling with the candidate-conflict graph.

The support-carrier discovery cost is now explicitly charged rather than assumed. The original Dean candidate-to-current-cohort support graph gives a transversal-matroid independence oracle through bipartite matching. Seymour's polynomial independence-oracle algorithm can test whether that support matroid is graphic and construct a realizing graph when it is. Since an accepted support matroid is both transversal/gammoid and graphic/binary, the binary-gammoid characterization places it in the K4-minor-free series-parallel carrier directly. A rejection does not answer the Dean problem; it records a NONBINARY_SUPPORT remainder. This avoids silently assuming a cheaply available GF(2) representation.

The rejected support branch is now constructive too. Every active support minor remains a gammoid. Within gammoids, binary and graphic coincide, so one-element deletion or contraction can be guided by repeated polynomial graphic-recognition tests. Continuing the smallest surviving non-graphic move reaches a minor-minimal nonbinary gammoid; Tutte's excluded-minor theorem identifies the terminal obstruction as U2,4. The four surviving elements and the exact delete/contract trace are retained as a Homeward-recoverable support certificate.

The log-conflict-cover carrier is now stronger than its first series-parallel formulation. If the candidate-conflict graph has a vertex cover Z of size O(log n), **no binary/graphic support assumption is needed**: enumerate the signed independent choices A⊆Z, delete only outside candidates conflicting with A, contract A in the Dean support transversal matroid, and test the remaining outside set for dependence using exact matching rank. Because Q-Z has no conflict edges, any residual matroid circuit is automatically candidate-compatible. A direct proof establishes completeness, and a bounded 20,000-instance differential panel returned zero mismatches. The earlier series-parallel cycle carrier remains useful as an alternate exact representation, not as a prerequisite for this theorem.

A coupling-graph calibration caught an important false promotion: the proposed Dean coupling graph J_S is exactly the original graph G, because S is already independent and the candidate-conflict plus candidate-to-S support edges reconstruct all edges of G. In fact max_B(|B|-|N(B)∩S|)=alpha(G)-|S| over conflict-stable B. Therefore bounded-treewidth DP on J_S is correct but is only the standard bounded-treewidth Independent-Set island in renamed coordinates, not a new universal carrier. The separate log-conflict-cover theorem remains genuine because it parameterizes only candidate-candidate conflict structure while handling arbitrary support edges by matching rank.

A further support-side admission theorem removes another hidden representation cost: when the candidate→current-cohort support graph is a bipartite forest, its ordinary 0/1 adjacency matrix is an exact GF(2) representation of the support transversal matroid. Matching a candidate subset into the current cohort is equivalent to linear independence of its adjacency columns; uniqueness of perfect matchings in a forest prevents determinant cancellation. This lets the binary signed carrier operate directly on original Dean relations for that family.

The next one-degree support move is now bounded by an explicit obstruction: a single K2,2 support cycle with two one-sided candidates realizes the nonbinary transversal matroid U(2,4), and deleting any cycle edge changes a previously valid matching. So cyclic support is not silently simplified to the forest carrier; U(2,4)/nonbinary structure is retained as a first-class residual for the next carrier.

That obstruction is now locally closed without changing fields. The bipartite support presentation decomposes into connected components, and the corresponding transversal matroid is their direct sum; every augmentation circuit therefore lives entirely in one support component. Any support component of fixed rank d has circuits of size at most d+1, so exact circuit enumeration is polynomial for constant d. In particular, the U(2,4) rank-2 obstruction is solved by checking conflict-free circuits of size at most three. The live support-side remainder has moved to connected support components of unbounded rank that are outside the existing binary/cluster carriers.

A second exact cluster-conflict carrier now removes duplicate choice entropy without losing identity. Inside one conflict clique, candidates with identical current support neighborhoods are interchangeable for support matching, so they share a derived support-signature coordinate while retaining every original OccurrenceID for Homeward reconstruction. A conflict-free circuit exists iff some full one-per-clique rainbow transversal is support-dependent; therefore only the Cartesian product of distinct support signatures needs to be checked. If the support-signature product is polynomial (equivalently its log entropy is O(log n) under a fixed bound), arbitrary transversal support is decidable exactly by signature enumeration plus bipartite matching.

That entropy carrier is now tighter: within one conflict clique, any candidate whose current support neighborhood strictly contains another same-clique candidate's neighborhood is dominated for augmentation dependence. Replacing the larger neighborhood by the smaller cannot increase support-matching rank, so every dependent rainbow choice has a dependent representative using only inclusion-minimal support neighborhoods. The live enumeration parameter is therefore the product of the minimal support-signature antichain sizes, not raw candidate count or even raw distinct-signature count.


A 2026 external result also aligns two previously separate repair families: vertex-packing LP structure and critical-independent-set structure are equivalent views of the same independence machinery. Accordingly, LP persistency / crown / critical-set extraction are treated as one saturated degree family before moving to independent carriers such as odd-cycle frames, matching/min-cut structure, rank, and signed residual repairs.

**Current mathematical status remains open.** Scoped carrier theorems, finite computations, codecs, semantic compression, and successful special cases are not promoted into a universal P=NP claim.


## September 18, 2026 convergence

The current forward-only convergence pass is governed by [GITHUB_CONVERGENCE_PLAN_2026-09-18.md](GITHUB_CONVERGENCE_PLAN_2026-09-18.md). Its exact-head verification and closure evidence are recorded in [GITHUB_CONVERGENCE_RESULT_2026-09-18.md](GITHUB_CONVERGENCE_RESULT_2026-09-18.md).

Current successor state includes:

- **RMAO** as the active game/world target;
- **RMALKDVMLLL** as the broader VM/link-layer framework;
- **RMAPL / Ω** as the domain-first repair/fitting design surface, with implementation still evidence-gated;
- **Dynamic RMAL** as a bounded dynamic game/state contract surface for the RMAO character system;
- **Super Seraphine** as a fictional magic trickster character whose presentation may vary broadly without gaining server/admin or real-world identity authority;
- **Sproutling** as a fictional game growth-form with no real-age or real-family mapping.

Privacy remains a hard publication gate:

```text
PUBLIC_REPOSITORY != PRIVATE_FAMILY_CONTEXT
GAME_CHARACTER != PRIVATE_PERSON
SPROUTLING != REAL_CHILD
PSEUDONYM != IDENTITY_DISCLOSURE
```

This section succeeds the September 14 snapshot below; it does not rewrite that historical checkpoint.


## September 19, 2026 — Libraries of Libraries successor

The current knowledge/reconstruction successor is **Libraries of Libraries**. This is not an identity claim that I, the repositories, or the projects are a federation. The older files and commits that used federation terminology remain preserved as historical provenance rather than being rewritten.

Current canonical successor surfaces:

- [Libraries of Libraries — RMAL Build](LIBRARIES_OF_LIBRARIES_RMAL_BUILD_2026-09-19.md)
- [Libraries of Libraries — Git Lineage](LIBRARIES_OF_LIBRARIES_GIT_LINEAGE_2026-09-19.md)
- [Compact RMAL form](libraries-of-libraries.rmal)

The active compression is:

`Library-of-Babel problem → identity → Library → Orbit → Libraries of Libraries → fighting point → consequential distinction → Decision Field → RMAL → Master Librarian → human surface → action → observation → learned delta → recovery seed → way back`

The **fighting point** is current successor terminology for the preserve-the-difference boundary: if collapsing two states changes a protected consequence for the current obligation, that distinction must survive. The phrase is not retroactively attributed to older Git history.

`CONNECTION != OWNERSHIP` · `RELATION != AUTHORITY_TRANSFER` · `RECONSTRUCTION != SOURCE`


## September 19, 2026 — Cooperative implementation protocol

Current cooperation/provenance authority is recorded in [COOPERATIVE_IMPLEMENTATION_PROTOCOL_2026-09-19.md](COOPERATIVE_IMPLEMENTATION_PROTOCOL_2026-09-19.md) with a [machine-readable provenance record](COOPERATIVE_IMPLEMENTATION_PROVENANCE_2026-09-19.json).

The governing attribution is:

```text
DESIGN_AUTHORITY = RYAN_MCMILLAN
IMPLEMENTATION_RELATION = HUMAN_AI_COIMPLEMENTATION
CROSS_WRITE_DEFAULT = DENY
```

The governing coordination path is:

```text
REFERENCE -> PROPOSAL -> HANDOFF -> TARGET_DECISION -> TARGET_LOCAL_SUCCESSOR
```

This makes the collaboration explicit without collapsing ownership or authority: Ryan McMillan designed the goals, principles, named frameworks, and cooperation model; implementation and refinement are carried out collaboratively with ChatGPT/OpenAI tooling under Ryan's direction, constraints, corrections, and acceptance gates.

The executable RMAL carrier is maintained additively in `redogit/Other-Projects-` at `docs/RMAL_COOPERATIVE_IMPLEMENTATION_PROTOCOL.rmal`. Compilation validates the carrier syntax and structure; it does not grant scientific truth authority.


## September 19, 2026 — current cooperation/language/evidence continuation

The current synthesis now includes:

- **USDAY-first cooperation** and **Pairity-before-parity** as design-state operating rules;
- the corrected **Interlingua Linguistics Agreement System** successor;
- a fail-closed **private-history method-learning boundary**;
- the compiled/audited RMAL cooperative carrier in `redogit/Other-Projects-`;
- bounded Conscience64 public-source dependency measurements under the continuing publication hold.

The additive RMAL cooperative carrier passed RMALC 2.1.1 `check`, `compile`, and `audit`, with claim ceiling `PARSER_COMPILE_AUDIT_VALIDATION_ONLY`.

```text
REFERENCE -> PROPOSAL -> HANDOFF -> TARGET_DECISION -> TARGET_LOCAL_SUCCESSOR
```

```text
DESIGN != IMPLEMENTATION
AGREEMENT != EVIDENCE
PRIVATE_HISTORY != PUBLIC_EVIDENCE
SOURCE_GRAPH_MEASURED != PUBLICATION_APPROVED
AUDIT_PASS != SEMANTIC_TRUTH
```


## September 20, 2026 — Bidirectional Handoff Pairity

The cross-project cooperation path is now explicitly **two-way** at the coordination layer:

```text
A -> REQUEST -> B -> TARGET DECISION -> RESPONSE -> A
B -> REQUEST -> A -> TARGET DECISION -> RESPONSE -> B
```

See [Bidirectional Handoff Pairity](BIDIRECTIONAL_HANDOFF_PROTOCOL_2026-09-20.md) and its [machine-readable contract](BIDIRECTIONAL_HANDOFF_PROTOCOL_2026-09-20.json).

Each target keeps local authority. A request is not a command, a response is not authority transfer, and a counterproposal creates a new handoff rather than rewriting the prior response.

Current implementations remain asymmetric, but Conscience64 now has one verified structured return runtime for `PRIVATE_METHOD_HANDOFF`: PR #176 links the admitted request packet UOID to a restricted target-local response and append-only response ledger. Generic response classes remain unimplemented. Other-Projects has implemented RMAL handoff/target-acceptance syntax but not the complete structured return packet.


## September 20, 2026 — isolated public testbed

Conscience64 publication authority has narrowed to a generated **public-testbed-only** projection under issue #166. The projection is built solely from `public-testbed/`, carries the exact source revision and deterministic projection identity, rejects private-origin carriers, and is verified against both the `gh-pages` tree and the network edge.

This does **not** publish the Conscience64 repository as a whole and does not grant commercial rights.

`PUBLIC_TESTBED != WHOLE_REPOSITORY` · `PUBLIC_EXPERIMENT != VERIFIED_TRUTH` · `PUBLIC_PROJECTION != COMMERCIAL_LICENSE`
Current verified live source: `redogit/conscience64@9a17f941808993049466e9b75ee8ec99253b5add`.
Current projection commit: `0888f45cb99bd16e225e20bd30005c9de8582551`; projection identity: `e54b69dff1964e218d1f99d7d28f5ace05649723ff677f25840ebe502f10e4a3`.

The public surface now retains six explicit path states (`active`, `tested`, `failed`, `blocked`, `deferred`, `return`) and six method labels (`USDAY`, `Interlingua`, `Pairity`, `Visible paths`, `Wonderment`, `One-degree experiment`). These are navigation/method carriers, not authority or scientific validation.

## Explore the history — 10 doors

The history is intentionally **distributed, independently authoritative, and reconstructible rather than centralized**. The [10-step History Playground](history-playground.html) gives people ten optional doors into the theories, formal results, experiments, tools, games, apps, education, accessibility, philosophy, ethics, coding, failures, and recovered history.

Each door follows:

`FIND → TRY → LEARN → TEACH → LEAVE A TRAIL`

Finishing all ten is not a mastery badge or a completeness claim. It means you explored ten slices and can explain what you found. Start with the [full check-in guide](HISTORY_PLAYGROUND_10_STEP_CHECKIN.md), the [Work Index](WORK_INDEX.md), or the [machine-readable quest manifest](history-playground.json).

## Research + production

The [Research + Production switchboard](research-production.html) separates the work into independently traceable lanes:

`RESEARCH · PRODUCTION · PLAY · TEACH · HISTORY`

The two main loops are deliberately different:

`QUESTION → CLAIM → TEST → EVIDENCE → REMAINDER`

`NEED → BUILD → VERIFY → RELEASE/USE → FEEDBACK → REPAIR`

A research result may inspire a product without becoming a product claim. A successful app, game, or public release does not scientifically validate the theory that inspired it. The current [Research + Production guide](RESEARCH_PRODUCTION.md) and [machine-readable coordination map](research-production-map.json) carry the adaptive-organization, technical-we, meta-coordination, self-modeling, and no-center ideas alongside the existing projects. Their predecessors `RESEARCH_PRODUCTION_FEDERATION.md` and `research-production.json` remain preserved as historical provenance.

The current conversation has also been restructured into an explicit [goal topology](GOAL_TOPOLOGY_2026-09-15.md) with a [machine-readable companion](goal-topology-2026-09-15.json). Each goal keeps its own owner, state, dependency, next action, and evidence boundary so independent lines can move without silently merging authority.

## Active project slice

The current [Active Projects surface](active-projects.html) is the narrower **current/runnable/deployed** projection. It routes status back to the owning repositories instead of making this profile the authority. The predecessor `active-federation.*` files remain preserved as historical provenance.

`ACTIVE != UNIVERSALLY_VALID` · `CONNECTED != MERGED` · `OPEN_PR != ACTIVE_MAIN`

See the [activation notes](ACTIVE_PROJECTS.md) and [machine-readable active registry](active-projects.json). The predecessor `ACTIVE_FEDERATION.md` and `active-federation.json` remain preserved for provenance. Conscience64 and Other-Projects keep their own local active registries.

## Current work — September 14, 2026

### Conscience64 spatial interface

Conscience64 is being unified around a **2.5D / 3D visual grammar**. The main Space Lens now has a capability-adaptive graphics path that prefers **WebGPU**, falls back through **WebGL2 / WebGL**, and retains the existing Canvas2D/evidence surface when GPU APIs are unavailable. The Compass brings the major public and operational surfaces into one spatial navigation layer.

The same depth hierarchy is being carried through Play, Research Analytics, Coordinate Space, and Research Records. Reduced-motion and forced-colors fallbacks remain part of the interface contract. Visual depth is navigation and presentation—not scientific evidence.

[Conscience64](https://github.com/redogit/conscience64) · [Research site](https://redogit.github.io/conscience64/)

### Fuzzball Hidden Game — Alpha 0.1

The new **Fuzzball Hidden Game** is now a verified merged alpha inside Conscience64. The alpha has a deterministic floating-world seed, a triangular explorer, 22 ambient Fuzzballs, and 16 humanoid creatures across four families: Mosswalker, Glasskin, Emberkin, and Duskseer. Humanoids have bounded deterministic wandering and proximity reactions.

The game remains intentionally **unlisted** from the public Play catalog and declares `noindex,nofollow`. Unlisted is a distribution choice, not a security boundary.

Most importantly:

`HISTORICAL_FUZZBALL_CARRIER != NEW_FUZZBALL_HIDDEN_GAME`

The older Fuzzball research carrier is still an unresolved historical recovery problem. Similarity, naming, or new implementation does not establish historical identity.

### Research Analytics

Research Analytics now has a live-ingestion architecture rather than only a demonstration stream: append-only event handling, SSE delivery, explicit event classes, an LLVM event bridge, and CI checks around the event contract.

The analytics model keeps these states distinct:

`OBSERVATION → TESTED → VERIFIED → CONTRADICTION → REVISED → PROMOTED / REOPENED`

Promotion is never automatic. An analytics event can record evidence; the dashboard itself does not validate the scientific claim represented by that event.

### Research method

The working method has become deliberately cyclical:

`working model → outward exploration → independent evidence/testing → return → reconcile → second-pass verification → revised working model`

I try to preserve at least these distinctions:

```text
USER_INPUT != ASSISTANT_SYNTHESIS
REQUESTED != IMPLEMENTED
IMPLEMENTED != VERIFIED
OBSERVATION != INTERPRETATION
REPETITION != VERIFICATION
RETRIEVED != INDEPENDENT_EVIDENCE
BYTE_IDENTITY != SEMANTIC_TRUTH
SOFTWARE_VERIFICATION != SCIENTIFIC_VALIDATION
```

Negative results, interpretation mistakes, changed boundaries, contradictions, and unresolved questions remain first-class records. I also use a two-pass update habit: make the change, then come back through it looking specifically for omissions, confounds, stale links, provenance errors, and overclaims.

## What I am exploring

Current work includes Cross-Carrier reconstruction, Coordinate Space / Float64 transport, Tiny Babel / TBCL, Orbit-style knowledge organization, SPrime finite searches, Hodge-conjecture research aids, P-versus-NP-related computational experiments, human–knowledge working-set models, accessibility-oriented tooling, cooperative systems, games, and research infrastructure.

The Hodge conjecture and P versus NP remain open. A successful program run, transport round trip, visual analogy, finite search, or deployed web application does not change those mathematical claim ceilings.

## REDOGIT

**Do it. Inspect it. Redo it better.**

REDOGIT is my public working rule:

1. start from the actual artifact, code, idea, or failure;
2. preserve the predecessor and provenance;
3. bound the change;
4. build the smallest useful successor;
5. verify what can actually be verified;
6. retain failures and unresolved state;
7. continue from the successor without rewriting history.

See [REDOGIT.md](REDOGIT.md) and the machine-readable [redogit.json](redogit.json).

## All eight repositories

The account-level count remains **eight repositories: seven public and one private placeholder**. New Conscience64 games, research records, analytics surfaces, and browser tools are subprojects inside an existing repository; they do not silently become new repositories.

### 1. [Conscience64](https://github.com/redogit/conscience64)

Research environment and browser interface containing Space Lens / Compass4D, Coordinate Space, Research Analytics, research records, history/restore surfaces, public Play applications, Explorer/MMO work, Computational Chorus, Musilanguage, and the unlisted Fuzzball Hidden Alpha.

The repository preserves explicit boundaries between source integrity, software behavior, interpretation, and independent evidence.

### 2. [Dream to Action](https://github.com/redogit/Dream-To-Action)

A local-first human-directed planning prototype built around a chosen goal, a real barrier, a next action, and an honest review. Human benefit and assistive-technology validation remain separate from software checks.

### 3. [Other Projects](https://github.com/redogit/Other-Projects-)

Multi-project home for ChatGPT/Conscience cooperation, the Human Expression Archive, S1024 Compression Lab, Context Discovery, SPrime Search, Hodge research aids, and other bounded experiments. Subprojects are not counted as additional repositories.

### 4. [MauiBrickBreak](https://github.com/redogit/MauiBrickBreak)

Preserved .NET/MAUI game lineage with reusable game-state, geometry, collision, and behavior-verification successors.

### 5. [FirstNeuralNetwork](https://github.com/redogit/FirstNeuralNetwork)

Preserved neural-network experiments and bounded deterministic successors. Passing finite truth-table tests are software results, not claims of general intelligence.

### 6. [Orbit Engine](https://github.com/redogit/orbit)

An upstream-derived .NET MAUI Graphics engine fork with retained attribution and local REDOGIT repair history. This is distinct from the Orbit Library / knowledge-organization research line.

### 7. [redogit](https://github.com/redogit/redogit)

This profile, the REDOGIT rule and schema, repository accounting, verification helpers, and dated forward-only profile updates.

### 8. Private repository

One repository remains private. It is counted without publishing its identity, URL, code, documents, or internal verification details.

## Current Conscience64 research records

The preserved browser/API registry remains historically bounded while newer records are added forward-only. Current human-readable records include Cross-Carrier Wave, Orbit Library, Tiny Babel / TBCL, Operator Moonshot, Model Experiments, Geometry / 4D / Codecs, Historical Recovery, Hodge Conjecture Research Spine, and Research Analytics.

[Research records](https://github.com/redogit/conscience64/tree/main/research/projects) · [Research Analytics](https://redogit.github.io/conscience64/) · [Coordinate Space](https://redogit.github.io/conscience64/) · [Play](https://redogit.github.io/conscience64/) · [History](https://redogit.github.io/conscience64/)

## Rights and commercial access

Current owner policy: **no third-party commercial access is authorized at this time** for original material controlled by the owner unless a separate written grant explicitly says otherwise.

Public visibility does not itself grant commercial rights. Consumer access to RMAOS MINGX, including the declared Mini ($0.99) and Full / Pro Advanced ($1.00) tiers, is separate from business licensing or commercial exploitation rights.

Earlier versions of this profile used broader “Free Use” wording. The current policy does not pretend to retroactively cancel rights already validly granted under an existing license or permission. MIT-, GPL-, upstream-, third-party-, and other already-applicable terms continue to control the material they cover.

See [COMMERCIAL_ACCESS_POLICY.md](COMMERCIAL_ACCESS_POLICY.md).

`PUBLIC_PROJECTION != COMMERCIAL_ACCESS` · `BUSINESS_ACCESS = DENY_UNLESS_EXPLICITLY_GRANTED`

---

Current navigation: [Libraries of Libraries](LIBRARIES_OF_LIBRARIES_RMAL_BUILD_2026-09-19.md) · [RMAL](libraries-of-libraries.rmal) · [Git lineage](LIBRARIES_OF_LIBRARIES_GIT_LINEAGE_2026-09-19.md) · [Goal Topology](GOAL_TOPOLOGY_2026-09-15.md) · [Active project slice](active-projects.html) · [Research + Production](research-production.html) · [Work Index](WORK_INDEX.md) · [10-Step History Playground](history-playground.html) · [Detailed check-in guide](HISTORY_PLAYGROUND_10_STEP_CHECKIN.md) · [Body-of-work recovery snapshot](BODY_OF_WORK.md) · [Standalone homepage](index.html)
