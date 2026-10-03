#include "rmal/word_dna.h"

#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RMAL_DNA_NONE UINT32_MAX

typedef struct { char *name; } Feature;

typedef struct {
    size_t surface_start;
    size_t surface_count;
    size_t feature_index;
    uint16_t confidence_permille;
    char *relation;
} Pair;

typedef struct {
    size_t left_start;
    size_t left_count;
    size_t right_start;
    size_t right_count;
    RmalDnaFoldRelation relation;
} FoldContact;

struct RmalWordDna {
    uint64_t occurrence_id;
    uint64_t semantic_object_id;
    char *utf8;
    size_t utf8_size;
    RmalWordDnaAtom *atoms;
    size_t atom_count;

    uint64_t *support_ids;
    size_t support_id_count;
    size_t support_id_capacity;

    uint64_t *clarity_ids;
    size_t clarity_id_count;
    size_t clarity_id_capacity;

    Feature *features;
    size_t feature_count;
    size_t feature_capacity;

    Pair *pairs;
    size_t pair_count;
    size_t pair_capacity;
};

struct RmalWordDnaFold {
    const RmalWordDna *word;
    FoldContact *contacts;
    size_t contact_count;
    size_t contact_capacity;
};

typedef struct {
    RmalWordDna *word;
    uint32_t next[RMAL_DNA_AXIS_COUNT];
} TriplexEntry;

struct RmalTriplexWordDna {
    TriplexEntry *entries;
    size_t count;
    size_t capacity;
};

typedef struct {
    char *data;
    size_t size;
    size_t capacity;
} TextBuffer;

static RmalStatus ok_status(void) {
    RmalStatus st = {.ok = true};
    return st;
}

static RmalStatus error_status(const char *format, ...) {
    RmalStatus st = {.ok = false};
    va_list ap;
    va_start(ap, format);
    vsnprintf(st.message, sizeof st.message, format, ap);
    va_end(ap);
    return st;
}

static char *copy_bytes(const char *data, size_t count) {
    char *copy = malloc(count + 1);
    if (!copy) return NULL;
    if (count) memcpy(copy, data, count);
    copy[count] = '\0';
    return copy;
}

static bool grow_array(void **data, size_t element_size, size_t *capacity,
                       size_t required) {
    if (*capacity >= required) return true;
    size_t next = *capacity ? *capacity : 4;
    while (next < required) {
        if (next > SIZE_MAX / 2) return false;
        next *= 2;
    }
    if (element_size && next > SIZE_MAX / element_size) return false;
    void *grown = realloc(*data, next * element_size);
    if (!grown) return false;
    *data = grown;
    *capacity = next;
    return true;
}

static bool utf8_decode_one(const unsigned char *s, size_t remaining,
                            uint32_t *scalar, size_t *width) {
    if (!remaining) return false;
    const unsigned char b0 = s[0];

    if (b0 <= 0x7f) {
        *scalar = b0;
        *width = 1;
        return true;
    }

    if (b0 >= 0xc2 && b0 <= 0xdf) {
        if (remaining < 2 || (s[1] & 0xc0) != 0x80) return false;
        *scalar = ((uint32_t)(b0 & 0x1f) << 6)
                | (uint32_t)(s[1] & 0x3f);
        *width = 2;
        return true;
    }

    if (b0 >= 0xe0 && b0 <= 0xef) {
        if (remaining < 3 || (s[1] & 0xc0) != 0x80 ||
            (s[2] & 0xc0) != 0x80) return false;
        if (b0 == 0xe0 && s[1] < 0xa0) return false;
        if (b0 == 0xed && s[1] >= 0xa0) return false;
        *scalar = ((uint32_t)(b0 & 0x0f) << 12)
                | ((uint32_t)(s[1] & 0x3f) << 6)
                | (uint32_t)(s[2] & 0x3f);
        *width = 3;
        return true;
    }

    if (b0 >= 0xf0 && b0 <= 0xf4) {
        if (remaining < 4 || (s[1] & 0xc0) != 0x80 ||
            (s[2] & 0xc0) != 0x80 || (s[3] & 0xc0) != 0x80) return false;
        if (b0 == 0xf0 && s[1] < 0x90) return false;
        if (b0 == 0xf4 && s[1] >= 0x90) return false;
        *scalar = ((uint32_t)(b0 & 0x07) << 18)
                | ((uint32_t)(s[1] & 0x3f) << 12)
                | ((uint32_t)(s[2] & 0x3f) << 6)
                | (uint32_t)(s[3] & 0x3f);
        *width = 4;
        return true;
    }

    return false;
}

static bool scan_utf8(const char *data, size_t count,
                      RmalWordDnaAtom **atoms_out, size_t *atom_count_out) {
    size_t offset = 0;
    size_t atom_count = 0;
    size_t capacity = 0;
    RmalWordDnaAtom *atoms = NULL;

    while (offset < count) {
        uint32_t scalar = 0;
        size_t width = 0;
        if (!utf8_decode_one((const unsigned char *)data + offset,
                             count - offset, &scalar, &width)) {
            free(atoms);
            return false;
        }
        if (!grow_array((void **)&atoms, sizeof *atoms, &capacity,
                        atom_count + 1)) {
            free(atoms);
            return false;
        }
        atoms[atom_count++] = (RmalWordDnaAtom){
            .byte_offset = offset,
            .byte_length = width,
            .unicode_scalar = scalar
        };
        offset += width;
    }

    *atoms_out = atoms;
    *atom_count_out = atom_count;
    return true;
}

static bool valid_utf8_text(const char *text) {
    if (!text || !*text) return false;
    RmalWordDnaAtom *atoms = NULL;
    size_t count = 0;
    const bool ok = scan_utf8(text, strlen(text), &atoms, &count) && count > 0;
    free(atoms);
    return ok;
}

RmalWordDna *rmal_word_dna_create(const char *utf8, uint64_t occurrence_id,
                                  uint64_t semantic_object_id,
                                  RmalStatus *status) {
    if (!utf8) {
        if (status) *status = error_status("word surface is null");
        return NULL;
    }
    return rmal_word_dna_create_n(utf8, strlen(utf8), occurrence_id,
                                  semantic_object_id, status);
}

RmalWordDna *rmal_word_dna_create_n(const char *utf8, size_t byte_count,
                                    uint64_t occurrence_id,
                                    uint64_t semantic_object_id,
                                    RmalStatus *status) {
    if (!utf8 && byte_count) {
        if (status) *status = error_status("word surface is null");
        return NULL;
    }
    if (!byte_count) {
        if (status) *status = error_status("word surface is empty");
        return NULL;
    }

    RmalWordDnaAtom *atoms = NULL;
    size_t atom_count = 0;
    if (!scan_utf8(utf8, byte_count, &atoms, &atom_count)) {
        if (status) *status = error_status("word surface is not valid UTF-8");
        return NULL;
    }

    RmalWordDna *word = calloc(1, sizeof *word);
    if (!word) {
        free(atoms);
        if (status) *status = error_status("allocation failed");
        return NULL;
    }

    word->utf8 = copy_bytes(utf8, byte_count);
    if (!word->utf8) {
        free(atoms);
        free(word);
        if (status) *status = error_status("allocation failed");
        return NULL;
    }

    word->occurrence_id = occurrence_id;
    word->semantic_object_id = semantic_object_id;
    word->utf8_size = byte_count;
    word->atoms = atoms;
    word->atom_count = atom_count;
    if (status) *status = ok_status();
    return word;
}

void rmal_word_dna_free(RmalWordDna *word) {
    if (!word) return;
    free(word->utf8);
    free(word->atoms);
    free(word->support_ids);
    free(word->clarity_ids);
    for (size_t i = 0; i < word->feature_count; ++i)
        free(word->features[i].name);
    free(word->features);
    for (size_t i = 0; i < word->pair_count; ++i)
        free(word->pairs[i].relation);
    free(word->pairs);
    free(word);
}

uint64_t rmal_word_dna_occurrence_id(const RmalWordDna *word) {
    return word ? word->occurrence_id : 0;
}

uint64_t rmal_word_dna_semantic_object_id(const RmalWordDna *word) {
    return word ? word->semantic_object_id : 0;
}

const char *rmal_word_dna_utf8(const RmalWordDna *word) {
    return word ? word->utf8 : NULL;
}

size_t rmal_word_dna_utf8_size(const RmalWordDna *word) {
    return word ? word->utf8_size : 0;
}

size_t rmal_word_dna_atom_count(const RmalWordDna *word) {
    return word ? word->atom_count : 0;
}

bool rmal_word_dna_atom_at(const RmalWordDna *word, size_t index,
                           RmalWordDnaAtom *out) {
    if (!word || !out || index >= word->atom_count) return false;
    *out = word->atoms[index];
    return true;
}

static RmalStatus append_id(uint64_t **ids, size_t *count,
                            size_t *capacity, uint64_t id) {
    if (!grow_array((void **)ids, sizeof **ids, capacity, *count + 1))
        return error_status("allocation failed");
    (*ids)[(*count)++] = id;
    return ok_status();
}

RmalStatus rmal_word_dna_add_support_id(RmalWordDna *word,
                                        uint64_t support_id) {
    if (!word) return error_status("word is null");
    return append_id(&word->support_ids, &word->support_id_count,
                     &word->support_id_capacity, support_id);
}

RmalStatus rmal_word_dna_add_clarity_id(RmalWordDna *word,
                                        uint64_t clarity_id) {
    if (!word) return error_status("word is null");
    return append_id(&word->clarity_ids, &word->clarity_id_count,
                     &word->clarity_id_capacity, clarity_id);
}

size_t rmal_word_dna_support_id_count(const RmalWordDna *word) {
    return word ? word->support_id_count : 0;
}

size_t rmal_word_dna_clarity_id_count(const RmalWordDna *word) {
    return word ? word->clarity_id_count : 0;
}

bool rmal_word_dna_support_id_at(const RmalWordDna *word, size_t index,
                                 uint64_t *out) {
    if (!word || !out || index >= word->support_id_count) return false;
    *out = word->support_ids[index];
    return true;
}

bool rmal_word_dna_clarity_id_at(const RmalWordDna *word, size_t index,
                                 uint64_t *out) {
    if (!word || !out || index >= word->clarity_id_count) return false;
    *out = word->clarity_ids[index];
    return true;
}

RmalStatus rmal_word_dna_add_feature(RmalWordDna *word, const char *feature,
                                     size_t *feature_index) {
    if (!word) return error_status("word is null");
    if (!valid_utf8_text(feature))
        return error_status("feature must be nonempty valid UTF-8");
    if (!grow_array((void **)&word->features, sizeof *word->features,
                    &word->feature_capacity, word->feature_count + 1))
        return error_status("allocation failed");

    char *name = copy_bytes(feature, strlen(feature));
    if (!name) return error_status("allocation failed");

    const size_t index = word->feature_count++;
    word->features[index].name = name;
    if (feature_index) *feature_index = index;
    return ok_status();
}

size_t rmal_word_dna_feature_count(const RmalWordDna *word) {
    return word ? word->feature_count : 0;
}

const char *rmal_word_dna_feature_at(const RmalWordDna *word, size_t index) {
    if (!word || index >= word->feature_count) return NULL;
    return word->features[index].name;
}

RmalStatus rmal_word_dna_pair_feature(RmalWordDna *word,
                                      size_t surface_start,
                                      size_t surface_count,
                                      size_t feature_index,
                                      const char *relation,
                                      uint16_t confidence_permille) {
    if (!word) return error_status("word is null");
    if (!surface_count || surface_start >= word->atom_count ||
        surface_count > word->atom_count - surface_start)
        return error_status("surface span is outside the word");
    if (feature_index >= word->feature_count)
        return error_status("feature index is outside the learned strand");
    if (confidence_permille > 1000)
        return error_status("confidence must be in 0..1000 permille");
    if (!valid_utf8_text(relation))
        return error_status("pair relation must be nonempty valid UTF-8");
    if (!grow_array((void **)&word->pairs, sizeof *word->pairs,
                    &word->pair_capacity, word->pair_count + 1))
        return error_status("allocation failed");

    char *relation_copy = copy_bytes(relation, strlen(relation));
    if (!relation_copy) return error_status("allocation failed");

    word->pairs[word->pair_count++] = (Pair){
        .surface_start = surface_start,
        .surface_count = surface_count,
        .feature_index = feature_index,
        .confidence_permille = confidence_permille,
        .relation = relation_copy
    };
    return ok_status();
}

size_t rmal_word_dna_pair_count(const RmalWordDna *word) {
    return word ? word->pair_count : 0;
}

bool rmal_word_dna_pair_at(const RmalWordDna *word, size_t index,
                           RmalWordDnaPairView *out) {
    if (!word || !out || index >= word->pair_count) return false;
    const Pair *p = &word->pairs[index];
    *out = (RmalWordDnaPairView){
        .surface_start = p->surface_start,
        .surface_count = p->surface_count,
        .feature_index = p->feature_index,
        .confidence_permille = p->confidence_permille,
        .relation = p->relation
    };
    return true;
}

static bool text_reserve(TextBuffer *buffer, size_t extra) {
    if (extra > SIZE_MAX - buffer->size - 1) return false;
    const size_t required = buffer->size + extra + 1;
    if (buffer->capacity >= required) return true;

    size_t next = buffer->capacity ? buffer->capacity : 128;
    while (next < required) {
        if (next > SIZE_MAX / 2) return false;
        next *= 2;
    }

    char *grown = realloc(buffer->data, next);
    if (!grown) return false;
    buffer->data = grown;
    buffer->capacity = next;
    return true;
}

static bool text_appendf(TextBuffer *buffer, const char *format, ...) {
    va_list ap;
    va_start(ap, format);
    va_list copy;
    va_copy(copy, ap);
    const int n = vsnprintf(NULL, 0, format, copy);
    va_end(copy);
    if (n < 0 || !text_reserve(buffer, (size_t)n)) {
        va_end(ap);
        return false;
    }

    vsnprintf(buffer->data + buffer->size,
              buffer->capacity - buffer->size, format, ap);
    va_end(ap);
    buffer->size += (size_t)n;
    return true;
}

static bool text_append_bytes(TextBuffer *buffer,
                              const char *data, size_t count) {
    if (!text_reserve(buffer, count)) return false;
    memcpy(buffer->data + buffer->size, data, count);
    buffer->size += count;
    buffer->data[buffer->size] = '\0';
    return true;
}

char *rmal_word_dna_render_piv(const RmalWordDna *word) {
    if (!word) return NULL;

    TextBuffer buffer = {0};
    if (!text_appendf(&buffer,
                      "PIV_FONT|WordDNA occurrence=%" PRIu64
                      " semantic=%" PRIu64
                      " atoms=%zu supports=%zu clarity=%zu ",
                      word->occurrence_id, word->semantic_object_id,
                      word->atom_count, word->support_id_count,
                      word->clarity_id_count)) {
        free(buffer.data);
        return NULL;
    }

    for (size_t i = 0; i < word->atom_count; ++i) {
        const RmalWordDnaAtom *atom = &word->atoms[i];
        /* Keep U+0000 visible without embedding a C-string terminator. */
        const char *display = atom->unicode_scalar == 0
                            ? "\\0" : word->utf8 + atom->byte_offset;
        const size_t display_size = atom->unicode_scalar == 0
                                  ? 2 : atom->byte_length;
        if (!text_appendf(&buffer, "[U+%04" PRIX32 ":", atom->unicode_scalar) ||
            !text_append_bytes(&buffer, display, display_size) ||
            !text_appendf(&buffer, "]")) {
            free(buffer.data);
            return NULL;
        }
    }

    if (!text_appendf(&buffer, "\n")) {
        free(buffer.data);
        return NULL;
    }

    return buffer.data;
}

RmalWordDnaFold *rmal_word_dna_fold_create(const RmalWordDna *word,
                                           RmalStatus *status) {
    if (!word) {
        if (status) *status = error_status("word is null");
        return NULL;
    }

    RmalWordDnaFold *fold = calloc(1, sizeof *fold);
    if (!fold) {
        if (status) *status = error_status("allocation failed");
        return NULL;
    }

    fold->word = word;
    if (status) *status = ok_status();
    return fold;
}

void rmal_word_dna_fold_free(RmalWordDnaFold *fold) {
    if (!fold) return;
    free(fold->contacts);
    free(fold);
}

RmalStatus rmal_word_dna_fold_add_contact(RmalWordDnaFold *fold,
                                          size_t left_start,
                                          size_t left_count,
                                          size_t right_start,
                                          size_t right_count,
                                          RmalDnaFoldRelation relation) {
    if (!fold || !fold->word) return error_status("fold is null");

    const size_t atom_count = fold->word->atom_count;
    if (!left_count || !right_count ||
        left_start >= atom_count || right_start >= atom_count ||
        left_count > atom_count - left_start ||
        right_count > atom_count - right_start)
        return error_status("fold contact span is outside the word");

    if (left_start == right_start && left_count == right_count)
        return error_status("fold contact cannot pair a span with itself");

    if (relation < RMAL_DNA_FOLD_PAIR ||
        relation > RMAL_DNA_FOLD_CONSTRAINT)
        return error_status("fold relation is invalid");

    if (!grow_array((void **)&fold->contacts, sizeof *fold->contacts,
                    &fold->contact_capacity, fold->contact_count + 1))
        return error_status("allocation failed");

    fold->contacts[fold->contact_count++] = (FoldContact){
        .left_start = left_start,
        .left_count = left_count,
        .right_start = right_start,
        .right_count = right_count,
        .relation = relation
    };
    return ok_status();
}

size_t rmal_word_dna_fold_contact_count(const RmalWordDnaFold *fold) {
    return fold ? fold->contact_count : 0;
}

bool rmal_word_dna_fold_validate(const RmalWordDnaFold *fold) {
    if (!fold || !fold->word) return false;
    const size_t atom_count = fold->word->atom_count;

    for (size_t i = 0; i < fold->contact_count; ++i) {
        const FoldContact *contact = &fold->contacts[i];
        if (!contact->left_count || !contact->right_count ||
            contact->left_start >= atom_count ||
            contact->right_start >= atom_count ||
            contact->left_count > atom_count - contact->left_start ||
            contact->right_count > atom_count - contact->right_start ||
            (contact->left_start == contact->right_start &&
             contact->left_count == contact->right_count) ||
            contact->relation < RMAL_DNA_FOLD_PAIR ||
            contact->relation > RMAL_DNA_FOLD_CONSTRAINT)
            return false;
    }

    return true;
}

char *rmal_word_dna_unfold_utf8(const RmalWordDnaFold *fold,
                                size_t *byte_count) {
    if (!fold || !fold->word) return NULL;
    if (byte_count) *byte_count = fold->word->utf8_size;
    return copy_bytes(fold->word->utf8, fold->word->utf8_size);
}

bool rmal_word_dna_fold_roundtrip_exact(const RmalWordDnaFold *fold) {
    if (!rmal_word_dna_fold_validate(fold)) return false;

    size_t byte_count = 0;
    char *copy = rmal_word_dna_unfold_utf8(fold, &byte_count);
    if (!copy) return false;

    const bool exact =
        byte_count == fold->word->utf8_size &&
        memcmp(copy, fold->word->utf8, byte_count) == 0;
    free(copy);
    return exact;
}

RmalTriplexWordDna *rmal_triplex_word_dna_create(void) {
    return calloc(1, sizeof(RmalTriplexWordDna));
}

void rmal_triplex_word_dna_free(RmalTriplexWordDna *triplex) {
    if (!triplex) return;
    for (size_t i = 0; i < triplex->count; ++i)
        rmal_word_dna_free(triplex->entries[i].word);
    free(triplex->entries);
    free(triplex);
}

RmalStatus rmal_triplex_word_dna_add(RmalTriplexWordDna *triplex,
                                     RmalWordDna *word, size_t *index) {
    if (!triplex || !word)
        return error_status("triplex and word are required");

    if (triplex->count >= RMAL_DNA_NONE)
        return error_status("triplex reached uint32 index capacity");

    if (!grow_array((void **)&triplex->entries, sizeof *triplex->entries,
                    &triplex->capacity, triplex->count + 1))
        return error_status("allocation failed");

    const size_t entry_index = triplex->count++;
    triplex->entries[entry_index] = (TriplexEntry){
        .word = word,
        .next = {RMAL_DNA_NONE, RMAL_DNA_NONE, RMAL_DNA_NONE}
    };

    if (index) *index = entry_index;
    return ok_status();
}

size_t rmal_triplex_word_dna_count(const RmalTriplexWordDna *triplex) {
    return triplex ? triplex->count : 0;
}

RmalWordDna *rmal_triplex_word_dna_at(const RmalTriplexWordDna *triplex,
                                      size_t index) {
    if (!triplex || index >= triplex->count) return NULL;
    return triplex->entries[index].word;
}

bool rmal_triplex_word_dna_predecessor(const RmalTriplexWordDna *triplex,
                                       RmalDnaAxis axis,
                                       size_t to_index,
                                       size_t *from_index) {
    if (!triplex || axis < 0 || axis >= RMAL_DNA_AXIS_COUNT ||
        to_index >= triplex->count)
        return false;

    for (size_t i = 0; i < triplex->count; ++i) {
        if (triplex->entries[i].next[axis] == to_index) {
            if (from_index) *from_index = i;
            return true;
        }
    }

    return false;
}

static bool axis_reaches(const RmalTriplexWordDna *triplex,
                         RmalDnaAxis axis,
                         size_t start, size_t target) {
    size_t current = start;
    for (size_t steps = 0; steps < triplex->count; ++steps) {
        if (current == target) return true;
        const uint32_t next = triplex->entries[current].next[axis];
        if (next == RMAL_DNA_NONE) return false;
        if (next >= triplex->count) return true;
        current = next;
    }
    return true;
}

RmalStatus rmal_triplex_word_dna_link(RmalTriplexWordDna *triplex,
                                      RmalDnaAxis axis,
                                      size_t from_index,
                                      size_t to_index) {
    if (!triplex) return error_status("triplex is null");
    if (axis < 0 || axis >= RMAL_DNA_AXIS_COUNT)
        return error_status("axis is invalid");
    if (from_index >= triplex->count || to_index >= triplex->count)
        return error_status("link index is outside the triplex");
    if (from_index == to_index)
        return error_status("self-link is not a list edge");

    if (triplex->entries[from_index].next[axis] != RMAL_DNA_NONE)
        return error_status("source already has a successor on this axis");

    size_t predecessor = 0;
    if (rmal_triplex_word_dna_predecessor(triplex, axis,
                                          to_index, &predecessor))
        return error_status("target already has a predecessor on this axis");

    if (axis_reaches(triplex, axis, to_index, from_index))
        return error_status("link would create an axis cycle");

    triplex->entries[from_index].next[axis] = (uint32_t)to_index;
    return ok_status();
}

bool rmal_triplex_word_dna_next(const RmalTriplexWordDna *triplex,
                                RmalDnaAxis axis,
                                size_t from_index,
                                size_t *to_index) {
    if (!triplex || axis < 0 || axis >= RMAL_DNA_AXIS_COUNT ||
        from_index >= triplex->count)
        return false;

    const uint32_t next = triplex->entries[from_index].next[axis];
    if (next == RMAL_DNA_NONE) return false;
    if (to_index) *to_index = next;
    return true;
}

static bool axis_has_cycle(const RmalTriplexWordDna *triplex,
                           RmalDnaAxis axis, size_t start) {
    size_t slow = start;
    size_t fast = start;

    for (;;) {
        const uint32_t slow_next = triplex->entries[slow].next[axis];
        if (slow_next == RMAL_DNA_NONE) return false;
        if (slow_next >= triplex->count) return true;
        slow = slow_next;

        for (int step = 0; step < 2; ++step) {
            const uint32_t fast_next = triplex->entries[fast].next[axis];
            if (fast_next == RMAL_DNA_NONE) return false;
            if (fast_next >= triplex->count) return true;
            fast = fast_next;
        }

        if (slow == fast) return true;
    }
}

bool rmal_triplex_word_dna_validate(const RmalTriplexWordDna *triplex) {
    if (!triplex) return false;

    for (int axis_value = 0;
         axis_value < RMAL_DNA_AXIS_COUNT;
         ++axis_value) {
        const RmalDnaAxis axis = (RmalDnaAxis)axis_value;

        for (size_t i = 0; i < triplex->count; ++i) {
            const uint32_t next = triplex->entries[i].next[axis];
            if (next != RMAL_DNA_NONE && next >= triplex->count)
                return false;

            size_t predecessor_count = 0;
            for (size_t j = 0; j < triplex->count; ++j)
                if (triplex->entries[j].next[axis] == i)
                    ++predecessor_count;
            if (predecessor_count > 1) return false;

            if (axis_has_cycle(triplex, axis, i)) return false;
        }
    }

    return true;
}
