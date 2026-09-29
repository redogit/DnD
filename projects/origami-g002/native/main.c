/* Native IO/math host. All Origami transformations and models live in RMAL. */
#include <rmal/rmal.h>
#include <errno.h>
#include <math.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#endif
#include "origami_program.h"

#define INPUT_LIMIT (1024u * 1024u)
#define OUTPUT_LIMIT (8u * 1024u * 1024u)
#define NODE_LIMIT 100000u
#define BUFFER_LIMIT 256u
#define CELL_LIMIT 2000000u
#define STEP_LIMIT 100000000u

typedef enum { J_NULL, J_BOOL, J_NUMBER, J_STRING, J_ARRAY, J_OBJECT } JKind;
typedef struct Json Json;
struct Json {
    JKind kind;
    char *key;
    char *string;
    double number;
    bool boolean;
    Json **children;
    size_t count;
    size_t capacity;
};
typedef struct { const unsigned char *p, *end; size_t nodes; char error[256]; } Parser;
typedef struct { char *data; size_t size, capacity; } Text;
typedef struct { double *data; size_t size; } Buffer;
typedef struct {
    const char *command;
    Json *root;
    Buffer buffers[BUFFER_LIMIT];
    size_t buffer_count, cells;
    Text output;
} Host;

static bool append(Text *s, const char *data, size_t n, size_t limit) {
    if (n > limit - s->size) return false;
    size_t need = s->size + n + 1;
    if (need > s->capacity) {
        size_t cap = s->capacity ? s->capacity : 64;
        while (cap < need) cap *= 2;
        if (cap > limit + 1) cap = limit + 1;
        char *next = realloc(s->data, cap);
        if (!next) return false;
        s->data = next; s->capacity = cap;
    }
    memcpy(s->data + s->size, data, n);
    s->size += n; s->data[s->size] = 0;
    return true;
}
static char *copy_string(const char *s) {
    size_t n = strlen(s) + 1;
    char *r = malloc(n);
    if (r) memcpy(r, s, n);
    return r;
}
static void json_free(Json *j) {
    if (!j) return;
    for (size_t k = 0; k < j->count; k++) json_free(j->children[k]);
    free(j->children); free(j->key); free(j->string); free(j);
}
static bool parse_error(Parser *p, const char *s) {
    if (!p->error[0]) snprintf(p->error, sizeof p->error, "%s", s);
    return false;
}
static void space(Parser *p) {
    while (p->p < p->end && (*p->p == ' ' || *p->p == '\t' || *p->p == '\n' || *p->p == '\r')) p->p++;
}
static bool codepoint(Text *t, unsigned c) {
    char b[4]; size_t n;
    if (c == 0 || c > 0x10ffff || (c >= 0xd800 && c <= 0xdfff)) return false;
    if (c <= 0x7f) { b[0] = (char)c; n = 1; }
    else if (c <= 0x7ff) { b[0] = (char)(0xc0 | (c >> 6)); b[1] = (char)(0x80 | (c & 63)); n = 2; }
    else if (c <= 0xffff) { b[0] = (char)(0xe0 | (c >> 12)); b[1] = (char)(0x80 | ((c >> 6) & 63)); b[2] = (char)(0x80 | (c & 63)); n = 3; }
    else { b[0] = (char)(0xf0 | (c >> 18)); b[1] = (char)(0x80 | ((c >> 12) & 63)); b[2] = (char)(0x80 | ((c >> 6) & 63)); b[3] = (char)(0x80 | (c & 63)); n = 4; }
    return append(t, b, n, INPUT_LIMIT);
}
static bool hex4(Parser *p, unsigned *out) {
    if ((size_t)(p->end - p->p) < 4) return false;
    unsigned n = 0;
    for (int k = 0; k < 4; k++) {
        unsigned char c = *p->p++; unsigned d;
        if (c >= '0' && c <= '9') d = c - '0';
        else if (c >= 'a' && c <= 'f') d = c - 'a' + 10;
        else if (c >= 'A' && c <= 'F') d = c - 'A' + 10;
        else return false;
        n = n * 16 + d;
    }
    *out = n; return true;
}
static char *parse_string(Parser *p) {
    Text t = {0};
    if (p->p == p->end || *p->p++ != '"') return NULL;
    while (p->p < p->end) {
        unsigned c = *p->p++;
        if (c == '"') {
            if (!t.data) t.data = copy_string("");
            if (!t.data) parse_error(p, "Out of memory");
            return t.data;
        }
        if (c < 32) break;
        if (c == '\\') {
            if (p->p == p->end) break;
            c = *p->p++;
            if (c == 'u') {
                if (!hex4(p, &c)) break;
                if (c >= 0xd800 && c <= 0xdbff) {
                    unsigned lo;
                    if ((size_t)(p->end-p->p) < 6 || p->p[0] != '\\' || p->p[1] != 'u') break;
                    p->p += 2;
                    if (!hex4(p, &lo) || lo < 0xdc00 || lo > 0xdfff) break;
                    c = 0x10000 + (c-0xd800)*1024 + lo-0xdc00;
                }
            } else if (c == 'b') c = '\b';
            else if (c == 'f') c = '\f';
            else if (c == 'n') c = '\n';
            else if (c == 'r') c = '\r';
            else if (c == 't') c = '\t';
            else if (c != '"' && c != '\\' && c != '/') break;
        } else if (c >= 0x80) {
            unsigned remain, min;
            if (c >= 0xc2 && c <= 0xdf) { remain = 1; min = 0x80; c &= 31; }
            else if (c >= 0xe0 && c <= 0xef) { remain = 2; min = 0x800; c &= 15; }
            else if (c >= 0xf0 && c <= 0xf4) { remain = 3; min = 0x10000; c &= 7; }
            else break;
            if ((size_t)(p->end-p->p) < remain) break;
            bool valid = true;
            for (unsigned k = 0; k < remain; k++) {
                unsigned v = *p->p++;
                if ((v & 0xc0) != 0x80) { valid = false; break; }
                c = (c << 6) | (v & 63);
            }
            if (!valid || c < min) break;
        }
        if (!codepoint(&t, c)) break;
    }
    free(t.data);
    parse_error(p, "Invalid JSON string (valid Unicode without NUL is required)");
    return NULL;
}
static bool child_add(Json *j, Json *child) {
    if (j->count == j->capacity) {
        size_t cap = j->capacity ? j->capacity * 2 : 8;
        Json **next = realloc(j->children, cap * sizeof *next);
        if (!next) return false;
        j->children = next; j->capacity = cap;
    }
    j->children[j->count++] = child; return true;
}
static int key_compare(const void *a, const void *b) {
    return strcmp((*(Json *const *)a)->key, (*(Json *const *)b)->key);
}
static Json *parse_value(Parser *p, unsigned depth) {
    if (depth > 64 || p->nodes++ >= NODE_LIMIT) { parse_error(p, "JSON nesting or node budget exceeded"); return NULL; }
    space(p);
    if (p->p == p->end) { parse_error(p, "Missing JSON value"); return NULL; }
    Json *j = calloc(1, sizeof *j);
    if (!j) { parse_error(p, "Out of memory"); return NULL; }
    unsigned c = *p->p;
    if (c == '"') {
        j->kind = J_STRING; j->string = parse_string(p);
        if (!j->string) goto fail;
    } else if (c == '[' || c == '{') {
        j->kind = c == '[' ? J_ARRAY : J_OBJECT;
        unsigned closing = c == '[' ? ']' : '}';
        p->p++; space(p);
        if (p->p < p->end && *p->p == closing) { p->p++; return j; }
        while (p->p < p->end) {
            char *key = NULL;
            if (j->kind == J_OBJECT) {
                if (*p->p != '"' || !(key = parse_string(p))) goto fail;
                space(p);
                if (p->p == p->end || *p->p++ != ':') { free(key); goto fail; }
            }
            Json *child = parse_value(p, depth+1);
            if (!child) { free(key); goto fail; }
            child->key = key;
            if (!child_add(j, child)) { json_free(child); parse_error(p, "Out of memory"); goto fail; }
            space(p);
            if (p->p == p->end) goto fail;
            if (*p->p == closing) { p->p++; break; }
            if (*p->p++ != ',') goto fail;
            space(p);
            if (p->p == p->end || *p->p == closing) goto fail;
        }
        if (p->p == p->end && p->p[-1] != closing) goto fail;
        if (j->kind == J_OBJECT && j->count > 1) {
            Json **sorted = malloc(j->count * sizeof *sorted);
            if (!sorted) { parse_error(p, "Out of memory"); goto fail; }
            memcpy(sorted, j->children, j->count * sizeof *sorted);
            qsort(sorted, j->count, sizeof *sorted, key_compare);
            bool duplicate = false;
            for (size_t k = 1; k < j->count; k++) if (!strcmp(sorted[k-1]->key, sorted[k]->key)) { duplicate = true; break; }
            free(sorted);
            if (duplicate) { parse_error(p, "Duplicate JSON object key"); goto fail; }
        }
    } else if (c == '-' || (c >= '0' && c <= '9')) {
        const unsigned char *start = p->p;
        if (*p->p == '-') p->p++;
        if (p->p == p->end) goto fail;
        if (*p->p == '0') p->p++;
        else {
            if (*p->p < '1' || *p->p > '9') goto fail;
            do { p->p++; } while (p->p < p->end && *p->p >= '0' && *p->p <= '9');
        }
        if (p->p < p->end && *p->p == '.') {
            p->p++;
            if (p->p == p->end || *p->p < '0' || *p->p > '9') goto fail;
            do { p->p++; } while (p->p < p->end && *p->p >= '0' && *p->p <= '9');
        }
        if (p->p < p->end && (*p->p == 'e' || *p->p == 'E')) {
            p->p++;
            if (p->p < p->end && (*p->p == '+' || *p->p == '-')) p->p++;
            if (p->p == p->end || *p->p < '0' || *p->p > '9') goto fail;
            do { p->p++; } while (p->p < p->end && *p->p >= '0' && *p->p <= '9');
        }
        char *end;
        j->kind = J_NUMBER; j->number = strtod((const char *)start, &end);
        if ((const unsigned char *)end != p->p || !isfinite(j->number)) { parse_error(p, "JSON numbers must be finite"); goto fail; }
    } else {
        const char *word = c == 't' ? "true" : c == 'f' ? "false" : c == 'n' ? "null" : "";
        size_t n = strlen(word);
        if (!n || (size_t)(p->end-p->p) < n || memcmp(p->p, word, n)) goto fail;
        p->p += n;
        j->kind = c == 'n' ? J_NULL : J_BOOL; j->boolean = c == 't';
    }
    return j;
fail:
    parse_error(p, "Invalid JSON syntax"); json_free(j); return NULL;
}
static Json *member(Json *j, const char *key) {
    if (!j || j->kind != J_OBJECT) return NULL;
    for (size_t k = 0; k < j->count; k++) if (!strcmp(j->children[k]->key, key)) return j->children[k];
    return NULL;
}
static bool fields(Json *j, const char *const *required, size_t nr, const char *const *optional, size_t no) {
    if (!j || j->kind != J_OBJECT) return false;
    for (size_t k = 0; k < nr; k++) if (!member(j, required[k])) return false;
    for (size_t k = 0; k < j->count; k++) {
        bool found = false;
        for (size_t x = 0; x < nr; x++) if (!strcmp(required[x], j->children[k]->key)) found = true;
        for (size_t x = 0; x < no; x++) if (!strcmp(optional[x], j->children[k]->key)) found = true;
        if (!found) return false;
    }
    return true;
}
static bool number_range(Json *j, double lo, double hi) { return j && j->kind == J_NUMBER && j->number >= lo && j->number <= hi; }
static bool int_range(Json *j, double lo, double hi) { return number_range(j, lo, hi) && trunc(j->number) == j->number; }
static bool vector(Json *j, size_t n) {
    if (!j || j->kind != J_ARRAY || j->count != n) return false;
    double norm = 0;
    for (size_t k = 0; k < n; k++) {
        if (j->children[k]->kind != J_NUMBER) return false;
        norm = hypot(norm, j->children[k]->number);
    }
    return isfinite(norm);
}
static bool rotations(Json *j) {
    if (!j || j->kind != J_ARRAY || j->count > 4096) return false;
    const char *keys[] = {"i", "j", "degrees"};
    for (size_t k = 0; k < j->count; k++) {
        Json *r = j->children[k];
        if (!fields(r, keys, 3, NULL, 0) || !int_range(member(r,"i"),0,12) || !int_range(member(r,"j"),0,12) || member(r,"i")->number == member(r,"j")->number || !number_range(member(r,"degrees"),-360,360)) return false;
    }
    return true;
}
static size_t utf16_length(const char *s) {
    size_t n = 0;
    for (const unsigned char *p = (const unsigned char *)s; *p; p++) if ((*p & 0xc0) != 0x80) n += *p >= 0xf0 ? 2 : 1;
    return n;
}
static bool label(Json *j) { return j && j->kind == J_STRING && *j->string && utf16_length(j->string) <= 256; }
static bool centering_point(Json *j, size_t dimension) {
    if (!j || j->kind != J_ARRAY || j->count != dimension) return false;
    for (size_t k = 0; k < dimension; k++) if (!number_range(j->children[k],-1e100,1e100)) return false;
    return true;
}
static bool centering_series(Json *j, size_t count, size_t dimension) {
    if (!j || j->kind != J_ARRAY || j->count != count) return false;
    const char *required[] = {"name","points"};
    for (size_t k = 0; k < count; k++) {
        Json *series = j->children[k], *points = member(series,"points");
        if (!fields(series,required,2,NULL,0) || !label(member(series,"name")) || !points || points->kind != J_ARRAY || points->count < 2 || points->count > 2000) return false;
        double previous = -1;
        for (size_t n = 0; n < points->count; n++) {
            Json *point = points->children[n];
            if (!centering_point(point,dimension) || !number_range(point->children[0],0,1)) return false;
            double q = point->children[0]->number;
            if (q <= previous || (n == 0 && q != 0) || (n == points->count-1 && q != 1)) return false;
            previous = q;
        }
    }
    return true;
}
static bool centering_curves(Json *j) {
    Json *core = member(j,"core");
    if (!centering_series(member(j,"channels"),6,2) || !centering_series(member(j,"paths"),4,3) || !core || core->kind != J_ARRAY || core->count < 2 || core->count > 2000) return false;
    for (size_t k = 0; k < core->count; k++) if (!centering_point(core->children[k],2)) return false;
    return true;
}
static int string_compare(const void *a, const void *b) { return strcmp(*(const char *const *)a, *(const char *const *)b); }
static bool validate(const char *command, Json *j, char *error, size_t error_size) {
    const char *reason = "Input has missing, unsupported or incorrectly typed fields";
    if (!strcmp(command,"encode")) {
        const char *r[] = {"vector"}, *o[] = {"rotations"};
        if (!fields(j,r,1,o,1) || !vector(member(j,"vector"),13)) goto fail;
        Json *rs = member(j,"rotations"); if (rs && !rotations(rs)) goto fail;
    } else if (!strcmp(command,"recover")) {
        const char *r[] = {"schema","visible","residual","rotations"};
        if (!fields(j,r,4,NULL,0)) goto fail;
        Json *s = member(j,"schema"), *v = member(j,"visible"), *u = member(j,"residual");
        if (s->kind != J_STRING || strcmp(s->string,"origami-transform-v1") || !vector(v,3) || !vector(u,10) || !rotations(member(j,"rotations"))) goto fail;
        double norm = 0;
        for (size_t k = 0; k < v->count; k++) norm = hypot(norm,v->children[k]->number);
        for (size_t k = 0; k < u->count; k++) norm = hypot(norm,u->children[k]->number);
        if (!isfinite(norm)) goto fail;
    } else if (!strcmp(command,"simulate")) {
        const char *r[] = {"feed","initial","steps"};
        if (!fields(j,r,3,NULL,0) || !vector(member(j,"initial"),3) || !int_range(member(j,"steps"),0,1000000)) goto fail;
        Json *feed = member(j,"feed");
        if (feed->kind != J_STRING || (strcmp(feed->string,"lorenz") && strcmp(feed->string,"rossler") && strcmp(feed->string,"clifford"))) goto fail;
    } else if (!strcmp(command,"geometry")) {
        const char *r[] = {"fold"}, *o[] = {"n","a","b","gamma"};
        if (!fields(j,r,1,o,4) || !number_range(member(j,"fold"),0,90)) goto fail;
        Json *n = member(j,"n"), *a = member(j,"a"), *b = member(j,"b"), *g = member(j,"gamma");
        if ((n && !int_range(n,1,100)) || (a && (a->kind != J_NUMBER || a->number <= 0)) || (b && (b->kind != J_NUMBER || b->number <= 0)) || (g && !number_range(g,.01,89.99))) goto fail;
    } else if (!strcmp(command,"rf")) {
        const char *r[] = {"frequency"}, *o[] = {"options"};
        if (!fields(j,r,1,o,1) || !number_range(member(j,"frequency"),.01,100)) goto fail;
        Json *options = member(j,"options");
        if (options) {
            const char *names[] = {"capacitance","inductance","crease","resistance","delay"};
            const double lower[] = {.01,.01,0,0,0}, upper[] = {100,100,100,100,1000};
            if (!fields(options,NULL,0,names,5)) goto fail;
            for (size_t k = 0; k < 5; k++) { Json *v = member(options,names[k]); if (v && !number_range(v,lower[k],upper[k])) goto fail; }
        }
    } else if (!strcmp(command,"decide")) {
        if (j->kind != J_ARRAY || j->count < 1 || j->count > 10000) goto fail;
        const char *r[] = {"id","observation","actions"};
        size_t dimension = 0;
        char **ids = malloc(j->count * sizeof *ids);
        if (!ids) { reason = "Out of memory"; goto fail; }
        for (size_t k = 0; k < j->count; k++) {
            Json *row = j->children[k], *id = member(row,"id"), *ob = member(row,"observation"), *actions = member(row,"actions");
            if (!fields(row,r,3,NULL,0) || !label(id) || !ob || ob->kind != J_ARRAY || ob->count > 1024 || !vector(ob,ob->count) || !actions || actions->kind != J_ARRAY || actions->count < 1 || actions->count > 1000) { free(ids); goto fail; }
            if (k == 0) dimension = ob->count;
            if (ob->count != dimension) { free(ids); goto fail; }
            for (size_t a = 0; a < actions->count; a++) if (!label(actions->children[a])) { free(ids); goto fail; }
            ids[k] = id->string;
        }
        qsort(ids,j->count,sizeof *ids,string_compare);
        bool duplicate = false;
        for (size_t k = 1; k < j->count; k++) if (!strcmp(ids[k-1],ids[k])) { duplicate = true; break; }
        free(ids);
        if (duplicate) { reason = "Decision state IDs must be unique"; goto fail; }
    } else if (!strcmp(command,"explore")) {
        const char *required[] = {"home","operations"};
        if (!fields(j,required,2,NULL,0) || !label(member(j,"home"))) goto fail;
        Json *operations = member(j,"operations");
        if (!operations || operations->kind != J_ARRAY || operations->count > 512) goto fail;
        for (size_t k = 0; k < operations->count; k++) {
            Json *row = operations->children[k], *op = member(row,"op");
            if (!op || op->kind != J_STRING) goto fail;
            if (!strcmp(op->string,"discover")) {
                const char *names[] = {"op","id","note"};
                Json *note = member(row,"note");
                if (!fields(row,names,3,NULL,0) || !label(member(row,"id")) || !note || note->kind != J_STRING || utf16_length(note->string) > 4096) goto fail;
            } else if (!strcmp(op->string,"homeward") || !strcmp(op->string,"return_step")) {
                const char *names[] = {"op"};
                if (!fields(row,names,1,NULL,0)) goto fail;
            } else goto fail;
        }
    } else if (!strcmp(command,"centre")) {
        const char *required[] = {"schema","q","channels","paths","core"};
        if (!fields(j,required,5,NULL,0)) goto fail;
        Json *schema = member(j,"schema");
        if (schema->kind != J_STRING || strcmp(schema->string,"origami-centering-input-v1") || !number_range(member(j,"q"),0,1) || !centering_curves(j)) goto fail;
    } else if (!strcmp(command,"centre-run")) {
        const char *required[] = {"schema","runId","channels","paths","core","operations"};
        if (!fields(j,required,6,NULL,0)) goto fail;
        Json *schema = member(j,"schema"), *operations = member(j,"operations");
        if (schema->kind != J_STRING || strcmp(schema->string,"origami-centering-run-input-v1") || !label(member(j,"runId")) || !centering_curves(j) || operations->kind != J_ARRAY || operations->count > 128) goto fail;
        for (size_t k = 0; k < operations->count; k++) {
            Json *row = operations->children[k], *op = member(row,"op");
            if (!op || op->kind != J_STRING) goto fail;
            if (!strcmp(op->string,"advance")) {
                const char *names[] = {"op","q","note"}, *optional[] = {"homeward"};
                Json *note = member(row,"note"), *homeward = member(row,"homeward");
                if (!fields(row,names,3,optional,1) || !number_range(member(row,"q"),0,1) || !note || note->kind != J_STRING || utf16_length(note->string) > 4096 || (homeward && homeward->kind != J_BOOL)) goto fail;
            } else if (!strcmp(op->string,"homeward") || !strcmp(op->string,"return_step")) {
                const char *names[] = {"op"};
                if (!fields(row,names,1,NULL,0)) goto fail;
            } else goto fail;
        }
    } else { reason = "Unknown command; use --help"; goto fail; }
    return true;
fail:
    snprintf(error,error_size,"%s",reason); return false;
}

static Json *path_lookup(Json *j, const char *path) {
    if (!*path) return j;
    while (j) {
        const char *end = strchr(path,'.'); size_t n = end ? (size_t)(end-path) : strlen(path);
        if (!n || n > 256) return NULL;
        if (j->kind == J_OBJECT) {
            char key[257]; memcpy(key,path,n); key[n] = 0; j = member(j,key);
        } else if (j->kind == J_ARRAY) {
            size_t index = 0;
            for (size_t k = 0; k < n; k++) { if (path[k] < '0' || path[k] > '9' || index > INPUT_LIMIT) return NULL; index = index * 10 + (size_t)(path[k]-'0'); }
            j = index < j->count ? j->children[index] : NULL;
        } else return NULL;
        if (!end) return j;
        path = end+1;
    }
    return NULL;
}
static bool host_error(RmalStatus *status, const char *format, ...) {
    status->ok = false;
    va_list args; va_start(args,format); vsnprintf(status->message,sizeof status->message,format,args); va_end(args);
    return true;
}
static bool numeric(const RmalValue *v, double *n) {
    if (v->kind == RMAL_VALUE_INT) *n = (double)v->as.integer;
    else if (v->kind == RMAL_VALUE_FLOAT) *n = v->as.real;
    else return false;
    return isfinite(*n);
}
static bool index_arg(const RmalValue *v, size_t max, size_t *n) {
    double x;
    if (!numeric(v,&x) || x < 0 || x > (double)max || floor(x) != x) return false;
    *n = (size_t)x; return true;
}
static bool result_float(RmalValue *result, RmalStatus *status, double n) {
    if (!isfinite(n)) return host_error(status,"Nonfinite numerical result");
    result->kind = RMAL_VALUE_FLOAT; result->as.real = n; return true;
}
static char *json_quote(const char *s) {
    Text t = {0};
    if (!append(&t,"\"",1,OUTPUT_LIMIT)) goto fail;
    for (const unsigned char *p = (const unsigned char *)s; *p; p++) {
        char b[8]; size_t n;
        if (*p == '"' || *p == '\\') { b[0] = '\\'; b[1] = (char)*p; n = 2; }
        else if (*p < 32) { snprintf(b,sizeof b,"\\u%04x",*p); n = 6; }
        else { b[0] = (char)*p; n = 1; }
        if (!append(&t,b,n,OUTPUT_LIMIT)) goto fail;
    }
    if (!append(&t,"\"",1,OUTPUT_LIMIT)) goto fail;
    return t.data;
fail: free(t.data); return NULL;
}
static bool host_call(void *userdata, const char *name, const RmalValue *args, size_t argc, RmalValue *result, RmalStatus *status) {
    Host *h = userdata;
    status->ok = true; status->message[0] = 0;
    result->kind = RMAL_VALUE_NIL;
    if (!strcmp(name,"command")) {
        if (argc) return host_error(status,"command expects no arguments");
        result->kind = RMAL_VALUE_STRING; result->as.string = copy_string(h->command);
        if (!result->as.string) return host_error(status,"Out of memory");
        return true;
    }
    if (!strcmp(name,"has") || !strcmp(name,"boolean") || !strcmp(name,"number") || !strcmp(name,"integer") || !strcmp(name,"text") || !strcmp(name,"length")) {
        if (argc != 1 || args[0].kind != RMAL_VALUE_STRING) return host_error(status,"%s expects one string path",name);
        Json *j = path_lookup(h->root,args[0].as.string);
        if (!strcmp(name,"has")) { result->kind = RMAL_VALUE_BOOL; result->as.boolean = j != NULL; return true; }
        if (!j) return host_error(status,"Missing input path: %s",args[0].as.string);
        if (!strcmp(name,"boolean")) {
            if (j->kind != J_BOOL) return host_error(status,"Input path must be boolean: %s",args[0].as.string);
            result->kind = RMAL_VALUE_BOOL; result->as.boolean = j->boolean; return true;
        }
        if (!strcmp(name,"number")) {
            if (j->kind != J_NUMBER) return host_error(status,"Input path must be numeric: %s",args[0].as.string);
            return result_float(result,status,j->number);
        }
        if (!strcmp(name,"integer")) {
            if (!int_range(j,-9007199254740991.,9007199254740991.)) return host_error(status,"Input path must be a safe integer");
            result->kind = RMAL_VALUE_INT; result->as.integer = (int64_t)j->number; return true;
        }
        if (!strcmp(name,"text")) {
            if (j->kind != J_STRING) return host_error(status,"Input path must be a string");
            result->kind = RMAL_VALUE_STRING; result->as.string = copy_string(j->string);
            if (!result->as.string) return host_error(status,"Out of memory");
            return true;
        }
        if (j->kind != J_ARRAY) return host_error(status,"length requires an array path");
        result->kind = RMAL_VALUE_INT; result->as.integer = (int64_t)j->count; return true;
    }
    if (!strcmp(name,"buffer_new")) {
        size_t n;
        if (argc != 1 || !index_arg(args,CELL_LIMIT,&n)) return host_error(status,"buffer_new expects a bounded nonnegative integer");
        if (h->buffer_count == BUFFER_LIMIT || n > CELL_LIMIT-h->cells) return host_error(status,"Numeric buffer budget exceeded");
        Buffer *b = &h->buffers[h->buffer_count];
        b->data = calloc(n ? n : 1,sizeof *b->data);
        if (!b->data) return host_error(status,"Out of memory");
        b->size = n; h->cells += n;
        result->kind = RMAL_VALUE_INT; result->as.integer = (int64_t)h->buffer_count++;
        return true;
    }
    if (!strcmp(name,"buffer_get") || !strcmp(name,"buffer_set")) {
        bool write = !strcmp(name,"buffer_set"); size_t b, i;
        if (argc != (write ? 3u : 2u) || !index_arg(&args[0],BUFFER_LIMIT-1,&b) || b >= h->buffer_count || !index_arg(&args[1],CELL_LIMIT,&i) || i >= h->buffers[b].size) return host_error(status,"Invalid numeric buffer handle or index");
        if (write) {
            double value;
            if (!numeric(&args[2],&value)) return host_error(status,"Buffer values must be finite numbers");
            h->buffers[b].data[i] = value; return true;
        }
        return result_float(result,status,h->buffers[b].data[i]);
    }
    if (!strcmp(name,"emit")) {
        if (argc != 1) return host_error(status,"emit expects one value");
        char number[64]; const char *s;
        switch (args[0].kind) {
            case RMAL_VALUE_STRING: s = args[0].as.string; break;
            case RMAL_VALUE_NIL: s = "null"; break;
            case RMAL_VALUE_BOOL: s = args[0].as.boolean ? "true" : "false"; break;
            case RMAL_VALUE_INT: snprintf(number,sizeof number,"%lld",(long long)args[0].as.integer); s = number; break;
            case RMAL_VALUE_FLOAT:
                if (!isfinite(args[0].as.real)) return host_error(status,"Cannot emit a nonfinite result");
                snprintf(number,sizeof number,"%.17g",args[0].as.real == 0 ? 0.0 : args[0].as.real); s = number; break;
            default: return host_error(status,"Cannot emit this value kind");
        }
        if (!append(&h->output,s,strlen(s),OUTPUT_LIMIT)) return host_error(status,"Output budget exceeded");
        return true;
    }
    if (!strcmp(name,"quote")) {
        if (argc != 1 || args[0].kind != RMAL_VALUE_STRING) return host_error(status,"quote expects a string");
        result->kind = RMAL_VALUE_STRING; result->as.string = json_quote(args[0].as.string);
        if (!result->as.string) return host_error(status,"Out of memory or string too large");
        return true;
    }
    const char *unary[] = {"sin","cos","sqrt","abs","log10"};
    for (size_t k = 0; k < sizeof unary/sizeof *unary; k++) if (!strcmp(name,unary[k])) {
        double x;
        if (argc != 1 || !numeric(&args[0],&x)) return host_error(status,"%s expects one finite number",name);
        double value = k == 0 ? sin(x) : k == 1 ? cos(x) : k == 2 ? sqrt(x) : k == 3 ? fabs(x) : log10(x);
        return result_float(result,status,value);
    }
    const char *binary[] = {"atan2","pow","hypot2"};
    for (size_t k = 0; k < sizeof binary/sizeof *binary; k++) if (!strcmp(name,binary[k])) {
        double x,y;
        if (argc != 2 || !numeric(&args[0],&x) || !numeric(&args[1],&y)) return host_error(status,"%s expects two finite numbers",name);
        return result_float(result,status,k == 0 ? atan2(x,y) : k == 1 ? pow(x,y) : hypot(x,y));
    }
    return false;
}
static void error_output(const char *message) {
    char *quoted = json_quote(message);
    fprintf(stderr,"{\"error\":%s}\n",quoted ? quoted : "\"Out of memory\""); free(quoted);
}
static char *read_input(const char *path, char *error, size_t size) {
    FILE *file = !strcmp(path,"-") ? stdin : fopen(path,"rb");
    if (!file) { snprintf(error,size,"Cannot open input: %s",strerror(errno)); return NULL; }
#ifdef _WIN32
    if (file == stdin && _setmode(_fileno(stdin),_O_BINARY) == -1) { snprintf(error,size,"Cannot set stdin binary mode"); return NULL; }
#endif
    Text input = {0}; char chunk[8192]; size_t count;
    while ((count = fread(chunk,1,sizeof chunk,file)) > 0) if (!append(&input,chunk,count,INPUT_LIMIT)) {
        snprintf(error,size,"JSON input exceeds 1 MiB or memory budget"); free(input.data); if (file != stdin) fclose(file); return NULL;
    }
    bool failed = ferror(file) != 0;
    if (file != stdin) fclose(file);
    if (failed) { snprintf(error,size,"Cannot read JSON input"); free(input.data); return NULL; }
    if (!input.data) input.data = copy_string("");
    if (input.data && strlen(input.data) != input.size) { snprintf(error,size,"NUL byte in JSON input"); free(input.data); return NULL; }
    if (!input.data) snprintf(error,size,"Out of memory");
    return input.data;
}
int main(int argc, char **argv) {
    if (argc == 1 || (argc == 2 && !strcmp(argv[1],"--help"))) {
        puts("Origami RMAL native algorithm application\nUsage: origami-rmal <encode|recover|decide|simulate|geometry|rf|explore|centre|centre-run> [input.json|-]\nJSON results go to stdout only on success; errors go to stderr.\nRotation indices are 0..12. Input limit: 1 MiB; execution: 100,000,000 instructions."); return 0;
    }
    if (argc == 2 && !strcmp(argv[1],"--version")) { printf("origami-rmal 1.3.0-centering-run; %s\n",rmal_version()); return 0; }
    if (argc > 3) { error_output("Use --help for usage"); return 1; }
    const char *commands[] = {"encode","recover","decide","simulate","geometry","rf","explore","centre","centre-run"}; bool known = false;
    for (size_t k = 0; k < sizeof commands/sizeof *commands; k++) if (!strcmp(argv[1],commands[k])) known = true;
    if (!known) { error_output("Unknown command; use --help"); return 1; }
    char error[512] = {0};
    char *source = read_input(argc == 3 ? argv[2] : "-",error,sizeof error);
    if (!source) { error_output(error); return 1; }
    Parser parser = {(unsigned char *)source,(unsigned char *)source+strlen(source),0,{0}};
    Json *input = parse_value(&parser,0); space(&parser);
    if (!input || parser.p != parser.end) {
        error_output(parser.error[0] ? parser.error : "Trailing content after JSON input"); json_free(input); free(source); return 1;
    }
    free(source);
    if (!validate(argv[1],input,error,sizeof error)) { error_output(error); json_free(input); return 1; }
    RmalStatus status = {0};
    RmalProgram *program = rmal_parse_source(origami_program,"origami.rmal",&status);
    RmalBytecode *bytecode = NULL; RmalVm *vm = NULL; RmalValue result = {0};
    Host host = {.command=argv[1],.root=input};
    int exit_code = 1;
    if (!program) goto cleanup;
    bytecode = rmal_compile(program,&status);
    if (!bytecode) goto cleanup;
    vm = rmal_vm_create();
    if (!vm) { status.ok = false; snprintf(status.message,sizeof status.message,"Out of memory"); goto cleanup; }
    rmal_vm_set_host(vm,host_call,&host);
    rmal_vm_set_step_limit(vm,STEP_LIMIT);
    status = rmal_vm_run(vm,bytecode,false,&result);
    if (!status.ok) goto cleanup;
    if (!host.output.size) { status.ok = false; snprintf(status.message,sizeof status.message,"RMAL program produced no result"); goto cleanup; }
    /* Output is withheld until successful execution; external broken pipes remain OS IO errors. */
    if (fwrite(host.output.data,1,host.output.size,stdout) != host.output.size || fputc('\n',stdout) == EOF || fflush(stdout) == EOF) {
        status.ok = false; snprintf(status.message,sizeof status.message,"Cannot write result"); goto cleanup;
    }
    exit_code = 0;
cleanup:
    if (exit_code) error_output(status.message[0] ? status.message : "RMAL execution failed");
    rmal_value_free(&result); rmal_vm_free(vm); rmal_bytecode_free(bytecode); rmal_program_free(program);
    for (size_t k = 0; k < host.buffer_count; k++) free(host.buffers[k].data);
    free(host.output.data); json_free(input);
    return exit_code;
}
