#include "rmal/word_dna.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(x) do { if (!(x)) { fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #x); return 1; } } while (0)

int main(void) {
    RmalStatus st = {0};
    const char surface[] = "caf\xC3\xA9\xF0\x9F\xA7\xAC";
    RmalWordDna *word = rmal_word_dna_create(surface, 10, 20, &st);
    CHECK(word && st.ok);
    CHECK(rmal_word_dna_utf8_size(word) == strlen(surface));
    CHECK(rmal_word_dna_atom_count(word) == 5);

    RmalWordDnaAtom atom = {0};
    CHECK(rmal_word_dna_atom_at(word, 3, &atom));
    CHECK(atom.unicode_scalar == 0x00e9 && atom.byte_length == 2);
    CHECK(rmal_word_dna_atom_at(word, 4, &atom));
    CHECK(atom.unicode_scalar == 0x1f9ec && atom.byte_length == 4);

    CHECK(rmal_word_dna_add_support_id(word, 700).ok);
    CHECK(rmal_word_dna_add_clarity_id(word, 800).ok);
    uint64_t id = 0;
    CHECK(rmal_word_dna_support_id_at(word, 0, &id) && id == 700);
    CHECK(rmal_word_dna_clarity_id_at(word, 0, &id) && id == 800);

    size_t root = 0, marker = 0;
    CHECK(rmal_word_dna_add_feature(word, "ROOT:CAFÉ", &root).ok);
    CHECK(rmal_word_dna_add_feature(word, "DNA:MARKER", &marker).ok);
    CHECK(rmal_word_dna_pair_feature(word, 0, 4, root, "LEXEME", 1000).ok);
    CHECK(rmal_word_dna_pair_feature(word, 4, 1, marker, "SYMBOL", 900).ok);
    CHECK(rmal_word_dna_pair_count(word) == 2);

    char *piv = rmal_word_dna_render_piv(word);
    CHECK(piv && strstr(piv, "PIV_FONT|WordDNA") && strstr(piv, "U+1F9EC"));
    free(piv);

    RmalWordDnaFold *fold = rmal_word_dna_fold_create(word, &st);
    CHECK(fold && st.ok);
    CHECK(rmal_word_dna_fold_add_contact(fold, 0, 1, 4, 1, RMAL_DNA_FOLD_BRIDGE).ok);
    CHECK(rmal_word_dna_fold_validate(fold));
    CHECK(rmal_word_dna_fold_roundtrip_exact(fold));
    size_t unfolded_n = 0;
    char *unfolded = rmal_word_dna_unfold_utf8(fold, &unfolded_n);
    CHECK(unfolded && unfolded_n == strlen(surface));
    CHECK(memcmp(unfolded, surface, unfolded_n) == 0);
    free(unfolded);
    rmal_word_dna_fold_free(fold);

    const unsigned char invalid[] = {0xf0, 0x28, 0x8c, 0x28};
    CHECK(!rmal_word_dna_create_n((const char *)invalid, sizeof invalid, 1, 1, &st));
    CHECK(!st.ok);

    /* Length-bounded U+0000 must not truncate the C-string PIV projection. */
    const char nul_surface[] = {'A', '\0', 'B'};
    RmalWordDna *nul_word = rmal_word_dna_create_n(
        nul_surface, sizeof nul_surface, 40, 50, &st);
    CHECK(nul_word && st.ok);
    CHECK(rmal_word_dna_utf8_size(nul_word) == sizeof nul_surface);
    CHECK(memcmp(rmal_word_dna_utf8(nul_word), nul_surface, sizeof nul_surface) == 0);
    piv = rmal_word_dna_render_piv(nul_word);
    CHECK(piv && strcmp(piv,
        "PIV_FONT|WordDNA occurrence=40 semantic=50 atoms=3 supports=0 clarity=0 "
        "[U+0041:A][U+0000:\\0][U+0042:B]\n") == 0);
    free(piv);
    CHECK(rmal_word_dna_occurrence_id(nul_word) == 40);
    CHECK(rmal_word_dna_semantic_object_id(nul_word) == 50);
    CHECK(rmal_word_dna_utf8_size(nul_word) == sizeof nul_surface);
    CHECK(memcmp(rmal_word_dna_utf8(nul_word), nul_surface, sizeof nul_surface) == 0);
    CHECK(rmal_word_dna_atom_count(nul_word) == 3);
    for (size_t i = 0; i < sizeof nul_surface; ++i) {
        CHECK(rmal_word_dna_atom_at(nul_word, i, &atom));
        CHECK(atom.byte_offset == i && atom.byte_length == 1);
        CHECK(atom.unicode_scalar == (uint32_t)nul_surface[i]);
    }
    fold = rmal_word_dna_fold_create(nul_word, &st);
    CHECK(fold && st.ok);
    CHECK(rmal_word_dna_fold_roundtrip_exact(fold));
    unfolded = rmal_word_dna_unfold_utf8(fold, &unfolded_n);
    CHECK(unfolded && unfolded_n == sizeof nul_surface);
    CHECK(memcmp(unfolded, nul_surface, unfolded_n) == 0);
    free(unfolded);
    rmal_word_dna_fold_free(fold);
    rmal_word_dna_free(nul_word);

    RmalTriplexWordDna *t = rmal_triplex_word_dna_create();
    CHECK(t);
    size_t a = 0, b = 0, c = 0;
    CHECK(rmal_triplex_word_dna_add(t, word, &a).ok);
    word = NULL;
    CHECK(rmal_triplex_word_dna_add(t, rmal_word_dna_create("fold", 11, 21, &st), &b).ok);
    CHECK(rmal_triplex_word_dna_add(t, rmal_word_dna_create("know", 12, 22, &st), &c).ok);

    CHECK(rmal_triplex_word_dna_link(t, RMAL_DNA_AXIS_LINGUISTIC, a, b).ok);
    CHECK(rmal_triplex_word_dna_link(t, RMAL_DNA_AXIS_LINGUISTIC, b, c).ok);
    CHECK(rmal_triplex_word_dna_link(t, RMAL_DNA_AXIS_COMPUTATIONAL, a, c).ok);
    CHECK(rmal_triplex_word_dna_link(t, RMAL_DNA_AXIS_EPISTEMIC, c, b).ok);
    CHECK(!rmal_triplex_word_dna_link(t, RMAL_DNA_AXIS_LINGUISTIC, c, a).ok);
    CHECK(rmal_triplex_word_dna_validate(t));

    size_t next = 99, prev = 99;
    CHECK(rmal_triplex_word_dna_next(t, RMAL_DNA_AXIS_COMPUTATIONAL, a, &next) && next == c);
    CHECK(rmal_triplex_word_dna_predecessor(t, RMAL_DNA_AXIS_EPISTEMIC, b, &prev) && prev == c);

    const char nfd[] = "e\xCC\x81";
    RmalWordDna *decomposed = rmal_word_dna_create(nfd, 30, 30, &st);
    CHECK(decomposed && rmal_word_dna_atom_count(decomposed) == 2);
    CHECK(rmal_word_dna_utf8_size(decomposed) == 3);
    rmal_word_dna_free(decomposed);

    rmal_triplex_word_dna_free(t);
    puts("PASS WordDNA: exact UTF-8, scalar atoms, IDs, learned pairing, reversible fold, three list axes, PIV projection");
    return 0;
}
