    if(v->kind==RMAL_VALUE_STRING) return value_string(v->as.string);
    return *v;
}

char *rmal_value_render(const RmalValue *v){
    char buf[96];
    if(!v || v->kind==RMAL_VALUE_NIL) return xstrdup("nil");
    if(v->kind==RMAL_VALUE_BOOL) return xstrdup(v->as.boolean?"true":"false");
    if(v->kind==RMAL_VALUE_STRING) return xstrdup(v->as.string?v->as.string:"");
    if(v->kind==RMAL_VALUE_FLOAT) snprintf(buf,sizeof buf,"%.17g",v->as.real);
    else snprintf(buf,sizeof buf,"%" PRId64,v->as.integer);
    return xstrdup(buf);
}

static bool value_truthy(const RmalValue *v){
    if(!v) return false;
    switch (v->kind) {
        case RMAL_VALUE_BOOL: return v->as.boolean;
        case RMAL_VALUE_INT: return v->as.integer != 0;
        case RMAL_VALUE_FLOAT: return v->as.real != 0.0;
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
        case RMAL_VALUE_FLOAT: return a->as.real == b->as.real;
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
static Expr *parse_expr(Parser*,int);

static Expr *parse_prefix(Parser *p){
    Token *t=pk(p,0);
    if(accept(p,TK_MINUS)||accept(p,TK_BANG)){
        Expr *e=expr_new(EX_UNARY,t->line,t->column); if(!e)return NULL; e->text=xstrdup(t->lexeme); Expr*a=parse_prefix(p); if(!a||!expr_arg(e,a)){expr_free(e);return NULL;} return e;
    }
    if(accept(p,TK_LPAREN)){Expr*e=parse_expr(p,0);if(!expect(p,TK_RPAREN,")")){expr_free(e);return NULL;}return e;}
    if(t->kind==TK_NUMBER){
        p->i++; errno=0; char *end=NULL; RmalValue value=value_nil();
        if(strpbrk(t->lexeme,".eE")) {
            double x=strtod(t->lexeme,&end);
            /* strtod may set ERANGE for a finite subnormal or rounded zero. */
            if(!end || end==t->lexeme || *end || !isfinite(x)){set_status(p->status,false,t->line,t->column,"invalid finite floating literal");return NULL;}
            value=value_float(x);
        } else {
            long long x=strtoll(t->lexeme,&end,10);
            if(errno || !end || *end){set_status(p->status,false,t->line,t->column,"invalid integer");return NULL;}
            value=value_int((int64_t)x);
        }
        Expr*e=expr_new(EX_LITERAL,t->line,t->column);if(e)e->literal=value;return e;
    }
    if(t->kind==TK_STRING){p->i++;Expr*e=expr_new(EX_LITERAL,t->line,t->column);if(e)e->literal=value_string(t->lexeme);return e;}
    if(keyword(t,"true")||keyword(t,"false")||keyword(t,"nil")){p->i++;Expr*e=expr_new(EX_LITERAL,t->line,t->column);if(e)e->literal=keyword(t,"nil")?value_nil():value_bool(keyword(t,"true"));return e;}
    Token *id=expect(p,TK_IDENT,"expression"); if(!id)return NULL;
    if(accept(p,TK_LPAREN)){
        Expr*e=expr_new(EX_CALL,id->line,id->column);if(!e)return NULL;e->text=xstrdup(id->lexeme);
        if(!accept(p,TK_RPAREN)){do{Expr*a=parse_expr(p,0);if(!a||!expr_arg(e,a)){expr_free(e);return NULL;}}while(accept(p,TK_COMMA));if(!expect(p,TK_RPAREN,")")){expr_free(e);return NULL;}}
        return e;
    }
    Expr*e=expr_new(EX_VAR,id->line,id->column);if(e)e->text=xstrdup(id->lexeme);return e;
}

static Expr *parse_expr(Parser *p,int minp){
    int base_line=pk(p,0)->line; Expr *lhs=parse_prefix(p); if(!lhs)return NULL;
    for(;;){if(pk(p,0)->line>base_line)break;int prec=precedence(pk(p,0)->kind);if(prec<minp)break;Token*op=pk(p,0);p->i++;Expr*rhs=parse_expr(p,prec+1);if(!rhs){expr_free(lhs);return NULL;}Expr*e=expr_new(EX_BINARY,op->line,op->column);if(!e||!expr_arg(e,lhs)||!expr_arg(e,rhs)){expr_free(e);return NULL;}e->text=xstrdup(op->lexeme);lhs=e;}return lhs;
}

static bool is_directive_name(const char *s){
    static const char *names[]={"EXPORT","IMPORT","EXTERN","SURFACE","TARGET","TYPE","TRAIT","CLASS","RELATION","RELATE","OPERATOR","CONFIG","PROFILE","CONTEXT","CLAIM","EVIDENCE","RULE","LANGUAGE","ENTITY","ATTRIBUTE","BOUNDARY","PRESERVE","ALLOW","DENY","FORBID","OBLIGATION","INVARIANT","REMAINDER","COUNTERPROBE","VERIFY","CONTINUE","TRACE","FIND","PATH","LENS","SURVIVE","TAKE","GOAL","SCOPE","BOUND","GENERATE","FIT","CONSTRAIN","MUTATE","ROTATE","COMPOSE","CLASSIFY","BUILD","ARCHIVE","REPEAT"};
    for(size_t i=0;i<sizeof names/sizeof names[0];i++)if(strcmp(s,names[i])==0)return true;return false;
}

static char *directive_payload(Parser *p,int line){
    size_t cap=128,len=0;char *buf=calloc(cap,1);if(!buf)return NULL;
    while(!at_end(p)&&pk(p,0)->kind!=TK_SEMI&&pk(p,0)->line==line){
        const char *s=pk(p,0)->lexeme;size_t n=strlen(s);size_t need=len+n+2;if(need>cap){while(cap<need)cap*=2;char*q=realloc(buf,cap);if(!q){free(buf);return NULL;}buf=q;}
        if(len)buf[len++]=' ';memcpy(buf+len,s,n);len+=n;buf[len]='\0';p->i++;
    }
    accept(p,TK_SEMI);return buf;
}

static bool parse_block(Parser *p,Stmt ***data,size_t *count,size_t *cap);
static Stmt *parse_stmt(Parser *p){
    Token *t=pk(p,0);
    if(keyword(t,"MODULE")){p->i++;Token*id=expect(p,TK_IDENT,"module name");if(!id)return NULL;free(p->program->module);p->program->module=xstrdup(id->lexeme);accept(p,TK_SEMI);Stmt*s=stmt_new(ST_DIRECTIVE,t->line,t->column);if(s){s->name=xstrdup("MODULE");s->payload=xstrdup(id->lexeme);}return s;}
    if(keyword(t,"CONST")||keyword(t,"const")||keyword(t,"LET")||keyword(t,"let")||keyword(t,"STATE")||keyword(t,"SET")){
        StmtKind kind=keyword(t,"CONST")||keyword(t,"const")?ST_CONST:keyword(t,"STATE")?ST_STATE:keyword(t,"SET")?ST_SET:ST_LET;p->i++;Token*id=expect(p,TK_IDENT,"binding name");if(!id||!expect(p,TK_ASSIGN,"="))return NULL;Expr*e=parse_expr(p,0);if(!e)return NULL;accept(p,TK_SEMI);Stmt*s=stmt_new(kind,t->line,t->column);if(s){s->name=xstrdup(id->lexeme);s->expr=e;}return s;
    }
    if(keyword(t,"print")){p->i++;Expr*e=parse_expr(p,0);if(!e)return NULL;accept(p,TK_SEMI);Stmt*s=stmt_new(ST_PRINT,t->line,t->column);if(s)s->expr=e;return s;}
    if(keyword(t,"assert")||keyword(t,"ASSERT")||keyword(t,"REQUIRE")){p->i++;Expr*e=parse_expr(p,0);if(!e)return NULL;accept(p,TK_SEMI);Stmt*s=stmt_new(ST_ASSERT,t->line,t->column);if(s)s->expr=e;return s;}
    if(keyword(t,"STOP")){p->i++;accept(p,TK_SEMI);return stmt_new(ST_STOP,t->line,t->column);}
    if(keyword(t,"return")){p->i++;Expr*e=parse_expr(p,0);if(!e)return NULL;accept(p,TK_SEMI);Stmt*s=stmt_new(ST_RETURN,t->line,t->column);if(s)s->expr=e;return s;}
    if(keyword(t,"if")){p->i++;Expr*e=parse_expr(p,0);if(!e)return NULL;Stmt*s=stmt_new(ST_IF,t->line,t->column);if(!s){expr_free(e);return NULL;}s->expr=e;if(!parse_block(p,&s->body,&s->body_count,&s->body_cap)){stmt_free(s);return NULL;}if(keyword(pk(p,0),"else")){p->i++;if(!parse_block(p,&s->else_body,&s->else_count,&s->else_cap)){stmt_free(s);return NULL;}}return s;}
    if(keyword(t,"while")){p->i++;Expr*e=parse_expr(p,0);if(!e)return NULL;Stmt*s=stmt_new(ST_WHILE,t->line,t->column);if(!s){expr_free(e);return NULL;}s->expr=e;if(!parse_block(p,&s->body,&s->body_count,&s->body_cap)){stmt_free(s);return NULL;}return s;}
    if(keyword(t,"fn")){p->i++;Token*id=expect(p,TK_IDENT,"function name");if(!id||!expect(p,TK_LPAREN,"("))return NULL;Stmt*s=stmt_new(ST_FUNCTION,t->line,t->column);if(!s)return NULL;s->name=xstrdup(id->lexeme);if(!accept(p,TK_RPAREN)){do{Token*pa=expect(p,TK_IDENT,"parameter");if(!pa||!str_vec_push(&s->params,&s->param_count,&s->param_cap,pa->lexeme)){stmt_free(s);return NULL;}}while(accept(p,TK_COMMA));if(!expect(p,TK_RPAREN,")")){stmt_free(s);return NULL;}}if(!parse_block(p,&s->body,&s->body_count,&s->body_cap)){stmt_free(s);return NULL;}return s;}
    if(t->kind==TK_IDENT&&is_directive_name(t->lexeme)){p->i++;Stmt*s=stmt_new(ST_DIRECTIVE,t->line,t->column);if(!s)return NULL;s->name=xstrdup(t->lexeme);s->payload=directive_payload(p,t->line);if(!s->payload){stmt_free(s);return NULL;}return s;}
    Expr*e=parse_expr(p,0);if(!e)return NULL;accept(p,TK_SEMI);Stmt*s=stmt_new(ST_EXPR,t->line,t->column);if(s)s->expr=e;return s;
}

static bool parse_block(Parser *p,Stmt ***data,size_t *count,size_t *cap){if(!expect(p,TK_LBRACE,"{"))return false;while(!accept(p,TK_RBRACE)){if(at_end(p)){Token*t=pk(p,0);set_status(p->status,false,t->line,t->column,"unterminated block");return false;}Stmt*s=parse_stmt(p);if(!s||!stmt_vec_push(data,count,cap,s)){stmt_free(s);return false;}}return true;}
