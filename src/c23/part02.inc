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
