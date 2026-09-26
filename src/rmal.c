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
            case '!': if (lx_match(&l,'=')) {k=TK_NE;lit="!=";} else {k=TK_BANG;lit="!";} break; break;
            case '<': if (lx_match(&l,'=')) {k=TK_LE;lit="<=";} else {k=TK_LT;lit="<";} break;
            case '>': if (lx_match(&l,'=')) {k=TK_GE;lit=">=";} else {k=TK_GT;lit=">";} break;
            case '&': if (lx_match(&l,'&')) {k=TK_AND;lit="&&";} break;
            case '|': if (lx_match(&l,'|')) {k=TK_OR;lit="||";} break;
            default: break;
        }
        if (!lit) {
            char sym[2] = {c, '\0'};
            if (!lx_emit(&l, TK_SYMBOL, sym, 1, line, col)) goto fail;
            continue;
        }
        if (!lx_emit(&l, k, lit, strlen(lit), line, col)) goto fail;
    }
    if (!lx_emit(&l, TK_END, "", 0, l.line, l.column)) goto fail;
    *out = l.out;
    set_status(status, true, 0, 0, "ok");
    return true;
fail:
    tokens_free(&l.out);
    return false;
}

typedef enum { EX_LITERAL, EX_VAR, EX_UNARY, EX_BINARY, EX_CALL } ExprKind;
typedef struct Expr Expr;
struct Expr {
    ExprKind kind;
    int line, column;
    RmalValue literal;
    char *text;
    Expr **args;
    size_t argc, argcap;
};

typedef enum { ST_CONST, ST_LET, ST_STATE, ST_SET, ST_PRINT, ST_ASSERT, ST_STOP, ST_EXPR, ST_IF, ST_WHILE, ST_RETURN, ST_FUNCTION, ST_DIRECTIVE } StmtKind;
typedef struct Stmt Stmt;
struct Stmt {
    StmtKind kind;
    int line, column;
    char *name;
    char *payload;
    Expr *expr;
    char **params;
    size_t param_count, param_cap;
    Stmt **body;
    size_t body_count, body_cap;
    Stmt **else_body;
    size_t else_count, else_cap;
};

struct RmalProgram {
    char *source_name;
    char *module;
    Stmt **stmts;
    size_t count, cap;
};

static RmalValue value_nil(void){ return (RmalValue){.kind=RMAL_VALUE_NIL}; }
static RmalValue value_int(int64_t x){ return (RmalValue){.kind=RMAL_VALUE_INT,.as.integer=x}; }
static RmalValue value_bool(bool x){ return (RmalValue){.kind=RMAL_VALUE_BOOL,.as.boolean=x}; }
static RmalValue value_string(const char *x){ return (RmalValue){.kind=RMAL_VALUE_STRING,.as.string=xstrdup(x?x:"")}; }

void rmal_value_free(RmalValue *v){ if (v && v->kind==RMAL_VALUE_STRING) free(v->as.string); if(v) *v=value_nil(); }
static RmalValue value_clone(const RmalValue *v){
    if(!v) return value_nil();
    if(v->kind==RMAL_VALUE_STRING) return value_string(v->as.string);
    return *v;
}

char *rmal_value_render(const RmalValue *v){
    char buf[96];
    if(!v || v->kind==RMAL_VALUE_NIL) return xstrdup("nil");
    if(v->kind==RMAL_VALUE_BOOL) return xstrdup(v->as.boolean?"true":"false");
    if(v->kind==RMAL_VALUE_STRING) return xstrdup(v->as.string?v->as.string:"");
    snprintf(buf,sizeof buf,"%" PRId64,v->as.integer); return xstrdup(buf);
}

static bool value_truthy(const RmalValue *v){
    if(!v) return false;
    switch (v->kind) {
        case RMAL_VALUE_BOOL: return v->as.boolean;
        case RMAL_VALUE_INT: return v->as.integer != 0;
        case RMAL_VALUE_STRING: return v->as.string && v->as.string[0];
        case RMAL_VALUE_NIL: return false;
    }
    return false;
}

static bool value_equal(const RmalValue *a,const RmalValue *b){
    if(a->kind != b->kind) return false;
    switch(a->kind){
        case RMAL_VALUE_NIL: return true;
        case RMAL_VALUE_INT: return a->as.integer == b->as.integer;
        case RMAL_VALUE_BOOL: return a->as.boolean == b->as.boolean;
        case RMAL_VALUE_STRING: return strcmp(a->as.string?a->as.string:"", b->as.string?b->as.string:"")==0;
    }
    return false;
}

static Expr *expr_new(ExprKind kind,int line,int col){ Expr *e=calloc(1,sizeof *e); if(e){e->kind=kind;e->line=line;e->column=col;} return e; }
static bool expr_arg(Expr *e,Expr *a){ if(e->argc==e->argcap){size_t nc=GROW_CAPACITY(e->argcap);Expr **p=realloc(e->args,nc*sizeof *p);if(!p)return false;e->args=p;e->argcap=nc;}e->args[e->argc++]=a;return true; }
static Stmt *stmt_new(StmtKind kind,int line,int col){ Stmt *s=calloc(1,sizeof *s); if(s){s->kind=kind;s->line=line;s->column=col;}return s; }
static bool stmt_vec_push(Stmt ***data,size_t *count,size_t *cap,Stmt *s){if(*count==*cap){size_t nc=GROW_CAPACITY(*cap);Stmt **p=realloc(*data,nc*sizeof *p);if(!p)return false;*data=p;*cap=nc;}(*data)[(*count)++]=s;return true;}
static bool str_vec_push(char ***data,size_t *count,size_t *cap,const char *s){if(*count==*cap){size_t nc=GROW_CAPACITY(*cap);char **p=realloc(*data,nc*sizeof *p);if(!p)return false;*data=p;*cap=nc;}(*data)[(*count)++]=xstrdup(s);return (*data)[*count-1]!=NULL;}

static void expr_free(Expr *e){ if(!e)return; rmal_value_free(&e->literal); free(e->text); for(size_t i=0;i<e->argc;i++)expr_free(e->args[i]); free(e->args); free(e); }
static void stmt_free(Stmt *s){ if(!s)return; free(s->name);free(s->payload);expr_free(s->expr);for(size_t i=0;i<s->param_count;i++)free(s->params[i]);free(s->params);for(size_t i=0;i<s->body_count;i++)stmt_free(s->body[i]);free(s->body);for(size_t i=0;i<s->else_count;i++)stmt_free(s->else_body[i]);free(s->else_body);free(s); }
void rmal_program_free(RmalProgram *p){ if(!p)return;free(p->source_name);free(p->module);for(size_t i=0;i<p->count;i++)stmt_free(p->stmts[i]);free(p->stmts);free(p); }

typedef struct { TokenVec toks; size_t i; RmalProgram *program; RmalStatus *status; } Parser;
static Token *pk(Parser *p,size_t n){size_t k=p->i+n; if(k>=p->toks.count)k=p->toks.count-1; return &p->toks.data[k];}
static bool at_end(Parser *p){return pk(p,0)->kind==TK_END;}
static bool accept(Parser *p,TokenKind k){if(pk(p,0)->kind!=k)return false;p->i++;return true;}
static Token *expect(Parser *p,TokenKind k,const char *what){Token*t=pk(p,0);if(t->kind!=k){set_status(p->status,false,t->line,t->column,"expected %s, got '%s'",what,t->lexeme);return NULL;}p->i++;return t;}
static bool keyword(Token*t,const char*s){return t->kind==TK_IDENT&&strcmp(t->lexeme,s)==0;}
static int precedence(TokenKind k){
    switch(k){
        case TK_OR: return 1;
        case TK_AND: return 2;
        case TK_EQ: case TK_NE: return 3;
        case TK_LT: case TK_LE: case TK_GT: case TK_GE: return 4;
        case TK_PLUS: case TK_MINUS: return 5;
        case TK_STAR: case TK_SLASH: case TK_PERCENT: return 6;
        default: return -1;
    }
}
