# RMAL 3.1 WordDNA Triplex — vocabulary and native extension

**Status:** native C23 semantic carrier extension; source-language promotion is not implied.

This catalog keeps executable RMAL, parsed carriers, recovered/target semantics, and the WordDNA native extension separate.

'NATIVE API EXECUTABLE != RMAL SOURCE KEYWORD'

## A. Executable RMAL 3.1 source vocabulary

~~~text
MODULE
CONST
LET / let
STATE
SET
fn
if / else
while
return
print
assert / ASSERT
REQUIRE
STOP
~~~

Values: 'nil', 'int64', 'bool', 'string'.

Expression operators:

~~~text
! -
* / %
+ -
< <= > >=
== !=
&& ||
function-call
~~~

## B. Parsed carrier vocabulary

These are parser-preserved carriers unless separately implemented:

~~~text
EXPORT IMPORT EXTERN SURFACE TARGET
TYPE TRAIT CLASS RELATION RELATE OPERATOR CONFIG PROFILE CONTEXT
CLAIM EVIDENCE RULE LANGUAGE ENTITY ATTRIBUTE BOUNDARY
PRESERVE ALLOW DENY FORBID OBLIGATION INVARIANT REMAINDER
COUNTERPROBE VERIFY CONTINUE TRACE
FIND PATH LENS SURVIVE
TAKE GOAL SCOPE BOUND
GENERATE FIT CONSTRAIN MUTATE ROTATE COMPOSE
CLASSIFY BUILD ARCHIVE REPEAT
~~~

'PARSED_CARRIER_CURRENT != EXECUTABLE_CURRENT'

## C. Existing semantic vocabulary used here

Reversible-carrier algebra already named by the RMAL contract:

~~~text
IDENTITY COMPOSE INVERSE ROTATE FOLD FLIP UNFOLD
RECONSTRUCT COMPARE_INVARIANTS
~~~

RMALKDVMLLL compact vocabulary:

~~~text
TAKE GOAL SCOPE PRESERVE ALLOW FORBID BOUND
GENERATE FIT CONSTRAIN MUTATE ROTATE COMPOSE
VERIFY COUNTERPROBE CLASSIFY BUILD ARCHIVE REPEAT
~~~

Decision Field operations relevant when a strict list must become a branch:

~~~text
RESOLVE
PROVE_IRRELEVANT
MERGE_EQUIVALENT
SPLIT_ALIAS
EXPOSE_DEPENDENCY
REPLACE_WITH_SUFFICIENT_STATISTIC
ROTATE_FIELD
REJECT_LOSSY_MERGE
~~~

This extension does not promote target-only operations into RMAL bytecode.

## D. WordDNA native semantic vocabulary

These names describe the C23 carrier; they are not new lexer keywords in this increment:

~~~text
WordDNA
SurfaceStrand
SurfaceAtom
LearnedStrand
LearnedFeature
PairMap
Pair
OccurrenceID
SemanticObjectID
SupportsIDs
ClarityIDs
TriplexWordDNA
LinguisticNext
ComputationalNext
EpistemicNext
Fold
FoldContact
Unfold
Refold
PIVProjection
~~~

Fold relations:

~~~text
PAIR STEM LOOP HINGE BRIDGE MIRROR CONTACT CONSTRAINT
~~~

## E. Exact surface identity

Canonical storage is exact UTF-8 bytes. Native atom records are:

~~~text
(byte_offset, byte_length, unicode_scalar)
~~~

No Unicode normalization occurs. Canonically equivalent byte strings remain distinct source objects until an explicit transform relates them.

Current atomization is Unicode-scalar segmentation, **not** Unicode extended grapheme-cluster segmentation. Full UAX #29 segmentation is an explicit replaceable layer.

'UTF8 BYTE != UNICODE SCALAR != GRAPHEME CLUSTER != GLYPH'

## F. Learned strand

Learned features are appendable UTF-8 labels. Pair records bind a surface span to one learned feature with:

~~~text
surface_start
surface_count
feature_index
relation
confidence_permille
~~~

Learning changes annotations and pairings, not canonical surface bytes.

'WORD_IDENTITY != CURRENT_INTERPRETATION'
'GENERATE != VERIFY != ADMIT'

## G. Four ID roles

Every WordDNA carries:

~~~text
OccurrenceID
SemanticObjectID
SupportsIDs[]
ClarityIDs[]
~~~

Occurrence identity is not collapsed into semantic identity. Repeated support and clarity occurrences are retained rather than silently deduplicated.

## H. Triple directed list

One TriplexWordDNA owns WordDNA occurrences. Each entry stores exactly three 32-bit successor indices:

~~~text
LINGUISTIC
COMPUTATIONAL
EPISTEMIC
~~~

Each axis permits at most one successor and one predecessor and rejects cycles. Predecessors are derived by scan instead of stored in each node.

Canonical link storage is therefore '3 * uint32_t' per occurrence. Reverse lookup is O(n). This deliberately trades reverse-traversal time for minimal connection storage; a rebuildable reverse index can be added later without changing canonical links.

'STRICT CHAIN != BRANCH'
'BRANCH -> Decision Field / explicit graph carrier'

## I. Folding

A fold is a derived carrier over immutable WordDNA. Fold contacts relate two valid surface spans. Folding never rewrites canonical UTF-8.

~~~text
WordDNA -> FoldContact* -> Fold -> Unfold -> exact original UTF-8 bytes
~~~

Bounded invariant:

'UNFOLD(FOLD(x)).UTF8 == x.UTF8'

This proves exact source-byte round trip for the declared carrier only. It does not establish a linguistic, biological, or universal folding law.

## J. PIV_FONT boundary

PIV_FONT remains a human-visible carrier. No font files are embedded. 'rmal_word_dna_render_piv' emits a textual projection such as:

~~~text
PIV_FONT|WordDNA ... [U+0063:c][U+00E9:é][U+1F9EC:🧬]
~~~

'SOURCE IDENTITY != RENDERING'
'REPRESENTABLE != VISIBLE != DISTINGUISHABLE != UNDERSTOOD'

## K. Ownership and lifecycle

~~~text
UTF8
 -> WordDNA
 -> learned features / pair map
 -> TriplexWordDNA
 -> Fold
 -> execute / trace / evidence
 -> residual / learn
 -> refold
~~~

TriplexWordDNA owns a word after successful insertion. Fold borrows its WordDNA and must not outlive it.

## L. Claim ceiling

This increment establishes bounded native behavior for valid UTF-8/exact-byte preservation, scalar atom offsets, four ID roles, learned span pairing, three strict directed list axes, cycle rejection, reversible fold contacts, and a PIV textual hook.

It does not establish full grapheme segmentation, an automatic learning algorithm, biological equivalence to DNA, new executable RMAL keywords, universal semantic folding, or evidence promotion from compilation.
