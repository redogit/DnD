#ifndef RMAL_WORD_DNA_H
#define RMAL_WORD_DNA_H

#include "rmal/rmal.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    RMAL_DNA_AXIS_LINGUISTIC = 0,
    RMAL_DNA_AXIS_COMPUTATIONAL = 1,
    RMAL_DNA_AXIS_EPISTEMIC = 2,
    RMAL_DNA_AXIS_COUNT = 3
} RmalDnaAxis;

typedef enum {
    RMAL_DNA_FOLD_PAIR = 0,
    RMAL_DNA_FOLD_STEM,
    RMAL_DNA_FOLD_LOOP,
    RMAL_DNA_FOLD_HINGE,
    RMAL_DNA_FOLD_BRIDGE,
    RMAL_DNA_FOLD_MIRROR,
    RMAL_DNA_FOLD_CONTACT,
    RMAL_DNA_FOLD_CONSTRAINT
} RmalDnaFoldRelation;

typedef struct {
    size_t byte_offset;
    size_t byte_length;
    uint32_t unicode_scalar;
} RmalWordDnaAtom;

typedef struct {
    size_t surface_start;
    size_t surface_count;
    size_t feature_index;
    uint16_t confidence_permille;
    const char *relation;
} RmalWordDnaPairView;

typedef struct RmalWordDna RmalWordDna;
typedef struct RmalWordDnaFold RmalWordDnaFold;
typedef struct RmalTriplexWordDna RmalTriplexWordDna;

/* Surface bytes are immutable and are never normalized. */
RmalWordDna *rmal_word_dna_create(const char *utf8, uint64_t occurrence_id,
                                  uint64_t semantic_object_id, RmalStatus *status);
RmalWordDna *rmal_word_dna_create_n(const char *utf8, size_t byte_count,
                                    uint64_t occurrence_id,
                                    uint64_t semantic_object_id,
                                    RmalStatus *status);
void rmal_word_dna_free(RmalWordDna *word);

uint64_t rmal_word_dna_occurrence_id(const RmalWordDna *word);
uint64_t rmal_word_dna_semantic_object_id(const RmalWordDna *word);
const char *rmal_word_dna_utf8(const RmalWordDna *word);
size_t rmal_word_dna_utf8_size(const RmalWordDna *word);
size_t rmal_word_dna_atom_count(const RmalWordDna *word);
bool rmal_word_dna_atom_at(const RmalWordDna *word, size_t index,
                           RmalWordDnaAtom *out);

RmalStatus rmal_word_dna_add_support_id(RmalWordDna *word, uint64_t support_id);
RmalStatus rmal_word_dna_add_clarity_id(RmalWordDna *word, uint64_t clarity_id);
size_t rmal_word_dna_support_id_count(const RmalWordDna *word);
size_t rmal_word_dna_clarity_id_count(const RmalWordDna *word);
bool rmal_word_dna_support_id_at(const RmalWordDna *word, size_t index, uint64_t *out);
bool rmal_word_dna_clarity_id_at(const RmalWordDna *word, size_t index, uint64_t *out);

RmalStatus rmal_word_dna_add_feature(RmalWordDna *word, const char *feature,
                                     size_t *feature_index);
size_t rmal_word_dna_feature_count(const RmalWordDna *word);
const char *rmal_word_dna_feature_at(const RmalWordDna *word, size_t index);
RmalStatus rmal_word_dna_pair_feature(RmalWordDna *word, size_t surface_start,
                                      size_t surface_count, size_t feature_index,
                                      const char *relation,
                                      uint16_t confidence_permille);
size_t rmal_word_dna_pair_count(const RmalWordDna *word);
bool rmal_word_dna_pair_at(const RmalWordDna *word, size_t index,
                           RmalWordDnaPairView *out);

/* Human-visible projection hook. No PIV font bytes are embedded. */
char *rmal_word_dna_render_piv(const RmalWordDna *word);

/* Fold borrows word; the word must outlive the fold. */
RmalWordDnaFold *rmal_word_dna_fold_create(const RmalWordDna *word,
                                           RmalStatus *status);
void rmal_word_dna_fold_free(RmalWordDnaFold *fold);
RmalStatus rmal_word_dna_fold_add_contact(RmalWordDnaFold *fold,
                                          size_t left_start, size_t left_count,
                                          size_t right_start, size_t right_count,
                                          RmalDnaFoldRelation relation);
size_t rmal_word_dna_fold_contact_count(const RmalWordDnaFold *fold);
bool rmal_word_dna_fold_validate(const RmalWordDnaFold *fold);
char *rmal_word_dna_unfold_utf8(const RmalWordDnaFold *fold, size_t *byte_count);
bool rmal_word_dna_fold_roundtrip_exact(const RmalWordDnaFold *fold);

RmalTriplexWordDna *rmal_triplex_word_dna_create(void);
void rmal_triplex_word_dna_free(RmalTriplexWordDna *triplex);
/* Triplex owns word after a successful add. */
RmalStatus rmal_triplex_word_dna_add(RmalTriplexWordDna *triplex,
                                     RmalWordDna *word, size_t *index);
size_t rmal_triplex_word_dna_count(const RmalTriplexWordDna *triplex);
RmalWordDna *rmal_triplex_word_dna_at(const RmalTriplexWordDna *triplex,
                                      size_t index);
RmalStatus rmal_triplex_word_dna_link(RmalTriplexWordDna *triplex,
                                      RmalDnaAxis axis,
                                      size_t from_index, size_t to_index);
bool rmal_triplex_word_dna_next(const RmalTriplexWordDna *triplex,
                                RmalDnaAxis axis, size_t from_index,
                                size_t *to_index);
bool rmal_triplex_word_dna_predecessor(const RmalTriplexWordDna *triplex,
                                       RmalDnaAxis axis, size_t to_index,
                                       size_t *from_index);
bool rmal_triplex_word_dna_validate(const RmalTriplexWordDna *triplex);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
