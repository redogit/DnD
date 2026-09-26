# RMAL Research Orientation — TERMS FIRST — 2026-09-25

> **SELF-ORIENTATION FIRST.** This is written in my working language first. Public readability matters, but not at the cost of flattening the distinctions that let me reconstruct the work.

## 0. DEFINITION OF TERMS — I NEED THESE WORDS BEFORE I DO ANYTHING ELSE

I need to mean the same thing when I reuse a word later.

If the word moves, the proof can look like it moved when really the language did.

### RESULT
**MEAN:** what came out of the work.  
**NOT:** automatically the method, proof, explanation, or reusable knowledge that produced it.  
**KEEP:** making the doughnut is one Object; knowing how to make it again is another.

### METHOD
**MEAN:** the repeatable relation between starting state, lawful moves, checks, and result.  
**NOT:** one successful execution.  
**KEEP:** METHOD needs enough trace / conditions / cost to be run or audited again.

### CONDENSED WISDOM
**MEAN:** a compressed operational rule that can still be unfolded back into the experience, failures, conditions, evidence, and remainder that earned it.  
**NOT:** a slogan that became true because I like it.  
**KEEP:** compression is useful only while Homeward still exists.

### OBLIGATION
**MEAN:** what the result actually has to satisfy.  
**NOT:** a story about what I hope the solver does.  
**KEEP:** because a correct-looking result that misses the obligation is still wrong.

### OBJECT
**MEAN:** the thing I am trying to preserve or reason about.  
**NOT:** its current filename, coordinate, encoding, wording, or carrier.  
**KEEP:** object identity and representation must not silently collapse into each other.

### SURFACE
**MEAN:** the current view / address / interface where an Object is available.  
**NOT:** the Object itself.  
**KEEP:** failed representation != failed object.

### CARRIER
**MEAN:** the structure currently holding or moving what matters.  
**NOT:** permanent ontology.  
**KEEP:** when a carrier starts losing recoverability, move the carrier rather than letting the knowledge decay with it.

### CONTEXT
**MEAN:** the relations that make a meaning valid *here*.  
**NOT:** permission to promote a local equivalence into a universal one.

### RELATION
**MEAN:** an explicit typed connection between Objects.  
**NOT:** adjacency, resemblance, co-occurrence, or my feeling that two things belong together.

### CONSEQUENTIAL DISTINCTION
**MEAN:** a difference whose resolution can change what I may lawfully do next.  
**KEEP:** preserve these first; do not spend precision on distinctions that cannot affect the obligation.

### ONE-DEGREE MOVE
**MEAN:** change one consequential degree while holding the rest fixed enough to know what caused the result.  
**NOT:** one line of code, one variable name, or one arbitrary mutation.

### SIGNED MOVE
**MEAN:** a degree has two exact lawful directions and I preserve which direction was taken and why.  
**KEEP:** `+` and `-` are proof-bearing alternatives, not vibes.

### TRACE
**MEAN:** the ordered record of what was selected, changed, checked, observed, and retained.  
**NOT:** proof by itself.  
**KEEP:** the trace is what lets me distinguish execution from reconstruction from evidence.

### REMAINDER
**MEAN:** what is still unresolved after the exact work is removed.  
**NOT:** failure.  
**KEEP:** the remainder is the next problem, not something to hide.

### HOMEWARD
**MEAN:** the way back from a transformed / compressed / solved carrier to the original obligation, witness, and source.  
**NOT:** byte-for-byte reversal unless byte identity is the actual obligation.

### RECONSTRUCTION
**MEAN:** use preserved relations / receipts / provenance to recover what matters about an earlier state.  
**NOT:** pretending a summary is the source.  
**KEEP:** reconstructibility is part of whether compressed knowledge survived.

### KNOWLEDGE DECAY
**MEAN:** a consequential distinction that used to be reconstructible is no longer reconstructible.  
**FIGHT IT:** move toward a lower-decay carrier; preserve provenance and reconstruction while moving.

### EVIDENCE
**MEAN:** what was actually observed, proved, checked, or independently supported.  
**NOT:** a claim becoming stronger because it was repeated.

### AUTHORITY
**MEAN:** what is allowed to control the current decision.  
**NOT:** automatically the newest artifact.

### PROVENANCE
**MEAN:** where this came from, what changed it, and what evidence supports the change.

### CLAIM CEILING
**MEAN:** the strongest claim the current evidence actually supports.  
**KEEP:** scoped theorem != universal theorem.

## 1. THE DOUGHNUT LAW

```text
MAKING THE DOUGHNUTS
!=
KNOWING HOW TO MAKE THE DOUGHNUTS
!=
PRESERVING WHY / WHEN / FOR WHOM THE DOUGHNUTS MATTER
```

I can make a thing once and still lose the knowledge that made it repeatable.

I can preserve a recipe and still lose the reason the recipe existed.

I can preserve the words and still lose the relations needed to reconstruct what they meant.

So I keep these as separate obligations:

```text
RESULT
METHOD
MEANING / OBLIGATION
PROVENANCE
RECONSTRUCTION
```

A result is not automatically a method.  
A method is not automatically proof.  
A description is not automatically reconstruction.  
A carrier is not automatically the thing it carries.

## 2. ALGORITHMIC CONDENSED WISDOM ANALYSIS

This is the working meaning I use:

```text
MANY OBSERVATIONS
-> REPEATED BEHAVIOR
-> FAILURES + REPAIRS
-> WHAT SURVIVED
-> COMPRESSED OPERATIONAL RULE
-> REVERSE-DECOMPOSE THE RULE
-> ASSUMPTIONS / CONDITIONS / PROVENANCE / REMAINDER
-> REUSE ONLY WHERE IT STILL FITS
```

**Condensed wisdom** is not just a short sentence.

It is a small rule that survived enough experience to be useful **and can still be unfolded back into the reasons that earned it**.

If I cannot unfold it, it may still be a good saying, but it is not yet evidence.

```text
SHORT != SHALLOW
OLD != OBSOLETE
NEW != BETTER
SIMILAR != RELATED
REPEATED != INDEPENDENTLY VERIFIED
COMPRESSED != RECONSTRUCTIBLE
```

The child side of the rule asks:

```text
why?
what changed?
what did I actually see?
what would make this false?
what do I need to keep so I can do it again?
```

The elder side compresses the answers.

The algorithmic job is to keep both directions.

## 3. FOUR IDS FOR SEMANTIC RECONSTRUCTION

```text
OccurrenceID
SemanticObjectID
SemanticContextSupportsIDs
SemanticClarityIDs
```

### OccurrenceID
Where exactly did this occurrence come from?

### SemanticObjectID
What thing / concept / relation is this occurrence actually referring to?

### SemanticContextSupportsIDs
Why does this binding apply in this context?

### SemanticClarityIDs
What distinctions must stay explicit so this meaning does not drift?

```text
SAME WORD != SAME OBJECT
DIFFERENT WORD != DIFFERENT OBJECT
CONTEXTUAL EQUIVALENCE != UNIVERSAL EQUIVALENCE
```

## 4. EVIDENCE BOUNDARIES I DO NOT CROSS

```text
INGESTED != ACCEPTED_AS_FACT
GENERATE != VERIFY != ADMIT
COMPILES != PROVED
PASSES FINITE TESTS != UNIVERSAL THEOREM
SMALL FINAL CARRIER != CHEAP CONSTRUCTION
WARM START != COLD START
REPRESENTATION COMPRESSION != COMPLEXITY COLLAPSE
```

## 5. CURRENT PROOF PROGRAM IN MY WORDS

```text
STATE THE OBLIGATION
-> PRESERVE THE DISTINCTIONS THAT CAN CHANGE IT
-> FIND THE SMALLEST EXACT REPRESENTATION
-> DO ONE CONSEQUENTIAL THING
-> MEASURE WHAT ACTUALLY CHANGED
-> KEEP THE FAILURE IF IT FAILED
-> REPAIR ONLY THE SMALLEST BROKEN RELATION
-> KEEP A WAY HOME
-> REUSE ONLY WHAT SURVIVED
-> CONTINUE UNTIL THE REMAINDER IS GONE
```

For P vs NP, the current concrete carrier is the Dean / Independent-Set obligation.

I am **not** treating the open problem as solved.

The live work is to turn every unresolved state into exact polynomial progress without moving exponential work into:

```text
carrier discovery
semantic equivalence
projection
branch count
construction
verification
reconstruction
or history
```

## 6. CURRENT NORMALIZED SAT / DEAN HARD CORE

The present reduction stack removes exact polynomial structure before any new search degree:

```text
LINEAR AUTARKY
-> SIGNED COFACTOR DOMINANCE
-> BLOCKED CLAUSE ELIMINATION
-> CERTIFIED FUNCTIONAL VARIABLE / GATE ELIMINATION
-> NON-INCREASING DAVIS-PUTNAM MERGE
-> MATCHING / DEFICIENCY TERMINALS
-> OTHER ALREADY-CERTIFIED CARRIERS
```

What survives is not "all SAT."

It is the current exact remainder after those carriers have failed to close the state.

That remainder remains authoritative.

## 7. RMAL SELF-CHECK

Before I promote a new result:

```text
OBJECT?
OBLIGATION?
SOURCE?
RELATION?
WHAT CHANGED?
WHAT STAYED?
EVIDENCE?
COST?
HOMEWARD?
CLAIM CEILING?
REMAINDER?
```

If I cannot answer those, I have not finished describing the work.

## 8. WORKING WISDOM

```text
DON'T JUST SAVE THE WORDS.
SAVE THE MEANS TO RECONSTRUCT,
QUESTION,
REPAIR,
AND CONTINUE THEM.
```

```text
DO THE THING
-> INSPECT WHAT ACTUALLY HAPPENED
-> KEEP WHAT SURVIVED
-> REPAIR THE SMALLEST FAILURE
-> DO IT AGAIN
```

```text
OBJECT / OBLIGATION STAYS.
CARRIER MOVES AWAY FROM DECAY.
```

The short rule is useful.

The path that earned the short rule is what keeps it honest.

---

**Public orientation:** https://redogit.github.io/redogit/  
**Full condensed-wisdom analysis:** [Algorithmic Condensed Wisdom Analysis](ALGORITHMIC_CONDENSED_WISDOM_ANALYSIS_2026-09-25.md) · https://redogit.github.io/redogit/condensed-wisdom.html  
**Public live design:** https://redogit.github.io/redogit/live-design.html  
**Current live proof artifact:** [Signed Binary-Cluster Augmentation Carrier](PNP_SIGNED_BINARY_CLUSTER_CARRIER_2026-09-25.md)  
**Repository:** https://github.com/redogit/redogit
