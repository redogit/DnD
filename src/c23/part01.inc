#include "rmal/rmal.h"

#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#define RMAL_VERSION "3.1.0-c23"
#define GROW_CAPACITY(c) ((c) < 8 ? 8 : (c) * 2)

static char *xstrdup(const char *s) {
    if (!s) return NULL;
    size_t n = strlen(s) + 1;
    char *p = malloc(n);
    if (p) memcpy(p, s, n);
    return p;
}

static char *xstrndup(const char *s, size_t n) {
    char *p = malloc(n + 1);
    if (!p) return NULL;
    memcpy(p, s, n);
    p[n] = '\0';
    return p;
}

static void set_status(RmalStatus *st, bool ok, int line, int column, const char *fmt, ...) {
    if (!st) return;
    st->ok = ok;
    st->line = line;
    st->column = column;
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(st->message, sizeof st->message, fmt, ap);
    va_end(ap);
}

const char *rmal_version(void) { return RMAL_VERSION; }
const char *rmal_language_name(void) { return "RMAL — Ryan McMillan April Language"; }
const char *rmal_compiler_name(void) { return "RMALC — Ryan McMillan April Language Compiler"; }

typedef enum {
    TK_END, TK_IDENT, TK_NUMBER, TK_STRING,
    TK_LPAREN, TK_RPAREN, TK_LBRACE, TK_RBRACE, TK_COMMA, TK_SEMI,
    TK_ASSIGN, TK_PLUS, TK_MINUS, TK_STAR, TK_SLASH, TK_PERCENT, TK_BANG,
    TK_EQ, TK_NE, TK_LT, TK_LE, TK_GT, TK_GE, TK_AND, TK_OR, TK_SYMBOL
} TokenKind;

typedef struct {
    TokenKind kind;
    char *lexeme;
    int line;
    int column;
} Token;

typedef struct {
    Token *data;
    size_t count;
    size_t cap;
} TokenVec;

static bool token_push(TokenVec *v, Token t) {
    if (v->count == v->cap) {
        size_t nc = GROW_CAPACITY(v->cap);
        Token *p = realloc(v->data, nc * sizeof *p);
        if (!p) return false;
        v->data = p;
        v->cap = nc;
    }
    v->data[v->count++] = t;
    return true;
}

static void tokens_free(TokenVec *v) {
    for (size_t i = 0; i < v->count; ++i) free(v->data[i].lexeme);
    free(v->data);
    *v = (TokenVec){0};
}

typedef struct {
    const char *src;
    size_t i;
    int line;
    int column;
    TokenVec out;
    RmalStatus *status;
} Lexer;

static char lx_peek(Lexer *l, size_t n) {
    char c = l->src[l->i + n];
    return c;
}

static char lx_advance(Lexer *l) {
    char c = l->src[l->i];
    if (!c) return 0;
    l->i++;
    if (c == '\n') { l->line++; l->column = 1; }
    else l->column++;
    return c;
}

static bool lx_match(Lexer *l, char want) {
    if (lx_peek(l, 0) != want) return false;
    lx_advance(l);
    return true;
}

static bool lx_emit(Lexer *l, TokenKind k, const char *s, size_t n, int line, int col) {
    char *copy = xstrndup(s, n);
    if (!copy || !token_push(&l->out, (Token){k, copy, line, col})) {
        free(copy);
        set_status(l->status, false, line, col, "out of memory while lexing");
        return false;
    }
    return true;
}
