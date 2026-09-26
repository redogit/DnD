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

static bool lex_all(const char *src, TokenVec *out, RmalStatus *status) {
    Lexer l = {.src = src ? src : "", .line = 1, .column = 1, .status = status};
    while (lx_peek(&l, 0)) {
        int line = l.line, col = l.column;
        const char *start = l.src + l.i;
        char c = lx_advance(&l);
        if (isspace((unsigned char)c)) continue;
        if (c == '#') { while (lx_peek(&l, 0) && lx_peek(&l, 0) != '\n') lx_advance(&l); continue; }
        if (c == '/' && lx_peek(&l, 0) == '/') { while (lx_peek(&l, 0) && lx_peek(&l, 0) != '\n') lx_advance(&l); continue; }
        if (isalpha((unsigned char)c) || c == '_') {
            while (isalnum((unsigned char)lx_peek(&l, 0)) || lx_peek(&l, 0) == '_' || lx_peek(&l, 0) == '.') lx_advance(&l);
            if (!lx_emit(&l, TK_IDENT, start, (size_t)(l.src + l.i - start), line, col)) goto fail;
            continue;
        }
        if (isdigit((unsigned char)c)) {
            while (isdigit((unsigned char)lx_peek(&l, 0))) lx_advance(&l);
            if (!lx_emit(&l, TK_NUMBER, start, (size_t)(l.src + l.i - start), line, col)) goto fail;
            continue;
        }
        if (c == '"') {
            size_t begin = l.i;
            bool escaped = false;
            while (lx_peek(&l, 0)) {
                char q = lx_advance(&l);
                if (!escaped && q == '"') break;
                if (!escaped && q == '\\') escaped = true; else escaped = false;
            }
            if (l.src[l.i - 1] != '"') {
                set_status(status, false, line, col, "unterminated string"); goto fail;
            }
            const char *raw = l.src + begin;
            size_t raw_n = l.i - begin - 1;
            char *decoded = malloc(raw_n + 1);
            if (!decoded) { set_status(status, false, line, col, "out of memory"); goto fail; }
            size_t w = 0;
            for (size_t r = 0; r < raw_n; ++r) {
                if (raw[r] == '\\' && r + 1 < raw_n) {
                    char e = raw[++r];
                    decoded[w++] = e == 'n' ? '\n' : e == 't' ? '\t' : e == 'r' ? '\r' : e;
                } else decoded[w++] = raw[r];
            }
            decoded[w] = '\0';
            if (!token_push(&l.out, (Token){TK_STRING, decoded, line, col})) { free(decoded); set_status(status,false,line,col,"out of memory"); goto fail; }
            continue;
        }
        TokenKind k = TK_END;
        const char *lit = NULL;
        switch (c) {
            case '(': k=TK_LPAREN; lit="("; break; case ')': k=TK_RPAREN; lit=")"; break;
            case '{': k=TK_LBRACE; lit="{"; break; case '}': k=TK_RBRACE; lit="}"; break;
            case ',': k=TK_COMMA; lit=","; break; case ';': k=TK_SEMI; lit=";"; break;
            case '+': k=TK_PLUS; lit="+"; break; case '-': k=TK_MINUS; lit="-"; break;
            case '*': k=TK_STAR; lit="*"; break; case '/': k=TK_SLASH; lit="/"; break;
            case '%': k=TK_PERCENT; lit="%"; break;
            case '=': if (lx_match(&l,'=')) {k=TK_EQ;lit="==";} else {k=TK_ASSIGN;lit="=";} break;
            case '!': if (lx_match(&l,'=')) {k=TK_NE;lit="!=";} else {k=TK_BANG;lit="!";} break;