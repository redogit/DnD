RmalProgram *rmal_parse_source(const char *source,const char *source_name,RmalStatus *status){
    TokenVec toks={0};if(!lex_all(source,&toks,status))return NULL;
    RmalProgram *prog=calloc(1,sizeof *prog);if(!prog){tokens_free(&toks);set_status(status,false,0,0,"out of memory");return NULL;}prog->source_name=xstrdup(source_name?source_name:"<memory>");prog->module=xstrdup("main");
    Parser p={.toks=toks,.program=prog,.status=status};while(!at_end(&p)){Stmt*s=parse_stmt(&p);if(!s||!stmt_vec_push(&prog->stmts,&prog->count,&prog->cap,s)){stmt_free(s);rmal_program_free(prog);tokens_free(&toks);return NULL;}}
    tokens_free(&toks);set_status(status,true,0,0,"ok");return prog;
}

typedef enum { OP_PUSH, OP_LOAD, OP_DEFINE, OP_STORE, OP_ADD, OP_SUB, OP_MUL, OP_DIV, OP_MOD, OP_NEG, OP_NOT, OP_EQ, OP_NE, OP_LT, OP_LE, OP_GT, OP_GE, OP_AND, OP_OR, OP_PRINT, OP_ASSERT, OP_POP, OP_JUMP, OP_JFALSE, OP_CALL, OP_RET, OP_HALT, OP_DIRECTIVE } Op;
typedef enum { BIND_CONST=1, BIND_LET=2, BIND_STATE=3 } BindKind;
typedef struct {Op op;int64_t a,b;int line,column;} Ins;
typedef struct {Ins*data;size_t count,cap;} InsVec;
typedef struct {char*name;char**params;size_t param_count;InsVec code;} Function;
typedef struct {char*name;char*payload;int line,column;} Directive;
struct RmalBytecode {char*source_name;char*module;RmalValue*consts;size_t const_count,const_cap;char**names;size_t name_count,name_cap;Function*funcs;size_t func_count,func_cap;Directive*directives;size_t dir_count,dir_cap;InsVec main_code;};

static bool ins_push(InsVec*v,Ins x){if(v->count==v->cap){size_t nc=GROW_CAPACITY(v->cap);Ins*p=realloc(v->data,nc*sizeof*p);if(!p)return false;v->data=p;v->cap=nc;}v->data[v->count++]=x;return true;}
static size_t bc_const(RmalBytecode*b,RmalValue v){if(b->const_count==b->const_cap){size_t nc=GROW_CAPACITY(b->const_cap);RmalValue*p=realloc(b->consts,nc*sizeof*p);if(!p)return SIZE_MAX;b->consts=p;b->const_cap=nc;}b->consts[b->const_count]=value_clone(&v);return b->const_count++;}
static size_t bc_name(RmalBytecode*b,const char*n){for(size_t i=0;i<b->name_count;i++)if(strcmp(b->names[i],n)==0)return i;if(b->name_count==b->name_cap){size_t nc=GROW_CAPACITY(b->name_cap);char**p=realloc(b->names,nc*sizeof*p);if(!p)return SIZE_MAX;b->names=p;b->name_cap=nc;}b->names[b->name_count]=xstrdup(n);return b->name_count++;}
static bool bc_dir(RmalBytecode*b,const Stmt*s,size_t*idx){if(b->dir_count==b->dir_cap){size_t nc=GROW_CAPACITY(b->dir_cap);Directive*p=realloc(b->directives,nc*sizeof*p);if(!p)return false;b->directives=p;b->dir_cap=nc;}size_t k=b->dir_count++;b->directives[k]=(Directive){xstrdup(s->name),xstrdup(s->payload?s->payload:""),s->line,s->column};*idx=k;return true;}
static bool bc_func_push(RmalBytecode*b,Function f){if(b->func_count==b->func_cap){size_t nc=GROW_CAPACITY(b->func_cap);Function*p=realloc(b->funcs,nc*sizeof*p);if(!p)return false;b->funcs=p;b->func_cap=nc;}b->funcs[b->func_count++]=f;return true;}

static bool emit_expr(RmalBytecode*,const Expr*,InsVec*,RmalStatus*);
static bool emit_stmt(RmalBytecode*b,const Stmt*s,InsVec*out,RmalStatus*st){
    #define EMIT(OP,A,B) do{if(!ins_push(out,(Ins){OP,(int64_t)(A),(int64_t)(B),s->line,s->column})){set_status(st,false,s->line,s->column,"out of memory emitting bytecode");return false;}}while(0)
    if(s->kind==ST_CONST||s->kind==ST_LET||s->kind==ST_STATE){if(!emit_expr(b,s->expr,out,st))return false;size_t n=bc_name(b,s->name);if(n==SIZE_MAX)return false;int kind=s->kind==ST_CONST?BIND_CONST:s->kind==ST_STATE?BIND_STATE:BIND_LET;EMIT(OP_DEFINE,n,kind);return true;}
    if(s->kind==ST_SET){if(!emit_expr(b,s->expr,out,st))return false;size_t n=bc_name(b,s->name);if(n==SIZE_MAX)return false;EMIT(OP_STORE,n,0);return true;}
    if(s->kind==ST_PRINT){if(!emit_expr(b,s->expr,out,st))return false;EMIT(OP_PRINT,0,0);return true;}
    if(s->kind==ST_ASSERT){if(!emit_expr(b,s->expr,out,st))return false;EMIT(OP_ASSERT,0,0);return true;}
    if(s->kind==ST_STOP){EMIT(OP_HALT,0,0);return true;}
    if(s->kind==ST_EXPR){if(!emit_expr(b,s->expr,out,st))return false;EMIT(OP_POP,0,0);return true;}
    if(s->kind==ST_RETURN){if(!emit_expr(b,s->expr,out,st))return false;EMIT(OP_RET,0,0);return true;}
    if(s->kind==ST_DIRECTIVE){size_t d;if(!bc_dir(b,s,&d))return false;EMIT(OP_DIRECTIVE,d,0);return true;}
    if(s->kind==ST_IF){if(!emit_expr(b,s->expr,out,st))return false;size_t jf=out->count;EMIT(OP_JFALSE,0,0);for(size_t i=0;i<s->body_count;i++)if(!emit_stmt(b,s->body[i],out,st))return false;size_t j=out->count;EMIT(OP_JUMP,0,0);out->data[jf].a=(int64_t)out->count;for(size_t i=0;i<s->else_count;i++)if(!emit_stmt(b,s->else_body[i],out,st))return false;out->data[j].a=(int64_t)out->count;return true;}
    if(s->kind==ST_WHILE){size_t start=out->count;if(!emit_expr(b,s->expr,out,st))return false;size_t jf=out->count;EMIT(OP_JFALSE,0,0);for(size_t i=0;i<s->body_count;i++)if(!emit_stmt(b,s->body[i],out,st))return false;EMIT(OP_JUMP,start,0);out->data[jf].a=(int64_t)out->count;return true;}
    return true;
    #undef EMIT
}

static bool emit_expr(RmalBytecode*b,const Expr*e,InsVec*out,RmalStatus*st){
    #define E(OP,A,B) do{if(!ins_push(out,(Ins){OP,(int64_t)(A),(int64_t)(B),e->line,e->column})){set_status(st,false,e->line,e->column,"out of memory");return false;}}while(0)
    if(e->kind==EX_LITERAL){size_t c=bc_const(b,e->literal);if(c==SIZE_MAX)return false;E(OP_PUSH,c,0);return true;}
    if(e->kind==EX_VAR){size_t n=bc_name(b,e->text);if(n==SIZE_MAX)return false;E(OP_LOAD,n,0);return true;}
    if(e->kind==EX_UNARY){if(!emit_expr(b,e->args[0],out,st))return false;E(strcmp(e->text,"-")==0?OP_NEG:OP_NOT,0,0);return true;}
    if(e->kind==EX_CALL){for(size_t i=0;i<e->argc;i++)if(!emit_expr(b,e->args[i],out,st))return false;size_t n=bc_name(b,e->text);if(n==SIZE_MAX)return false;E(OP_CALL,n,e->argc);return true;}
    if(!emit_expr(b,e->args[0],out,st)||!emit_expr(b,e->args[1],out,st))return false;
    Op op=OP_ADD;const char*x=e->text;
    if(!strcmp(x,"+"))op=OP_ADD;else if(!strcmp(x,"-"))op=OP_SUB;else if(!strcmp(x,"*"))op=OP_MUL;else if(!strcmp(x,"/"))op=OP_DIV;else if(!strcmp(x,"%"))op=OP_MOD;else if(!strcmp(x,"=="))op=OP_EQ;else if(!strcmp(x,"!="))op=OP_NE;else if(!strcmp(x,"<"))op=OP_LT;else if(!strcmp(x,"<="))op=OP_LE;else if(!strcmp(x,">"))op=OP_GT;else if(!strcmp(x,">="))op=OP_GE;else if(!strcmp(x,"&&"))op=OP_AND;else if(!strcmp(x,"||"))op=OP_OR;else{set_status(st,false,e->line,e->column,"unknown operator %s",x);return false;}E(op,0,0);return true;
    #undef E
}

RmalBytecode *rmal_compile(const RmalProgram*p,RmalStatus*st){
    RmalBytecode*b=calloc(1,sizeof*b);if(!b){set_status(st,false,0,0,"out of memory");return NULL;}b->source_name=xstrdup(p->source_name);b->module=xstrdup(p->module);
    for(size_t i=0;i<p->count;i++)if(p->stmts[i]->kind==ST_FUNCTION){const Stmt*s=p->stmts[i];Function f={0};f.name=xstrdup(s->name);f.param_count=s->param_count;f.params=calloc(f.param_count,sizeof* f.params);if(f.param_count&&!f.params)goto oom;for(size_t k=0;k<f.param_count;k++)f.params[k]=xstrdup(s->params[k]);for(size_t k=0;k<s->body_count;k++)if(!emit_stmt(b,s->body[k],&f.code,st))goto fail;size_t nil=bc_const(b,value_nil());if(nil==SIZE_MAX)goto oom;if(!ins_push(&f.code,(Ins){OP_PUSH,(int64_t)nil,0,s->line,s->column})||!ins_push(&f.code,(Ins){OP_RET,0,0,s->line,s->column}))goto oom;if(!bc_func_push(b,f))goto oom;}
    for(size_t i=0;i<p->count;i++)if(p->stmts[i]->kind!=ST_FUNCTION)if(!emit_stmt(b,p->stmts[i],&b->main_code,st))goto fail;if(!ins_push(&b->main_code,(Ins){OP_HALT,0,0,0,0}))goto oom;set_status(st,true,0,0,"ok");return b;
oom:set_status(st,false,0,0,"out of memory compiling");fail:rmal_bytecode_free(b);return NULL;
}

void rmal_bytecode_free(RmalBytecode*b){if(!b)return;free(b->source_name);free(b->module);for(size_t i=0;i<b->const_count;i++)rmal_value_free(&b->consts[i]);free(b->consts);for(size_t i=0;i<b->name_count;i++)free(b->names[i]);free(b->names);for(size_t i=0;i<b->func_count;i++){free(b->funcs[i].name);for(size_t j=0;j<b->funcs[i].param_count;j++)free(b->funcs[i].params[j]);free(b->funcs[i].params);free(b->funcs[i].code.data);}free(b->funcs);for(size_t i=0;i<b->dir_count;i++){free(b->directives[i].name);free(b->directives[i].payload);}free(b->directives);free(b->main_code.data);free(b);}

typedef struct {char*name;RmalValue value;BindKind kind;} Binding;
typedef struct {Binding*data;size_t count,cap;} Env;
static void env_free(Env*e){for(size_t i=0;i<e->count;i++){free(e->data[i].name);rmal_value_free(&e->data[i].value);}free(e->data);*e=(Env){0};}
static Binding*env_find(Env*e,const char*n){for(size_t i=e->count;i-->0;)if(strcmp(e->data[i].name,n)==0)return &e->data[i];return NULL;}
static bool env_define(Env*e,const char*n,RmalValue v,BindKind kind){if(env_find(e,n))return false;if(e->count==e->cap){size_t nc=GROW_CAPACITY(e->cap);Binding*p=realloc(e->data,nc*sizeof*p);if(!p)return false;e->data=p;e->cap=nc;}e->data[e->count++]=(Binding){xstrdup(n),value_clone(&v),kind};return true;}

typedef struct {RmalValue*data;size_t count,cap;} Stack;
static bool stack_push(Stack*s,RmalValue v){if(s->count==s->cap){size_t nc=GROW_CAPACITY(s->cap);RmalValue*p=realloc(s->data,nc*sizeof*p);if(!p)return false;s->data=p;s->cap=nc;}s->data[s->count++]=value_clone(&v);return true;}
static bool stack_pop(Stack*s,RmalValue*out){if(!s->count)return false;*out=s->data[--s->count];return true;}
static void stack_free(Stack*s){for(size_t i=0;i<s->count;i++)rmal_value_free(&s->data[i]);free(s->data);*s=(Stack){0};}

struct RmalVm {Env globals;RmalTraceEvent*trace;size_t trace_count,trace_cap;
    RmalHostFunction host;void *host_data;size_t step_limit,steps,call_depth;};
RmalVm*rmal_vm_create(void){RmalVm*vm=calloc(1,sizeof(RmalVm));if(vm)vm->step_limit=1000000;return vm;}
void rmal_vm_set_host(RmalVm*vm,RmalHostFunction callback,void*user_data){if(vm){vm->host=callback;vm->host_data=user_data;}}
void rmal_vm_set_step_limit(RmalVm*vm,size_t limit){if(vm)vm->step_limit=limit?limit:1000000;}
void rmal_vm_free(RmalVm*v){if(!v)return;env_free(&v->globals);for(size_t i=0;i<v->trace_count;i++)free(v->trace[i].detail);free(v->trace);free(v);}
size_t rmal_vm_trace_count(const RmalVm*v){return v?v->trace_count:0;}
const RmalTraceEvent*rmal_vm_trace_at(const RmalVm*v,size_t i){return v&&i<v->trace_count?&v->trace[i]:NULL;}
static const char*op_name(Op op){static const char*n[]={"PUSH","LOAD","DEFINE","STORE","ADD","SUB","MUL","DIV","MOD","NEG","NOT","EQ","NE","LT","LE","GT","GE","AND","OR","PRINT","ASSERT","POP","JUMP","JFALSE","CALL","RET","HALT","DIRECTIVE"};return n[(int)op];}
static void trace_add(RmalVm*v,size_t pc,Ins in,const char*detail){if(v->trace_count==v->trace_cap){size_t nc=GROW_CAPACITY(v->trace_cap);RmalTraceEvent*p=realloc(v->trace,nc*sizeof*p);if(!p)return;v->trace=p;v->trace_cap=nc;}v->trace[v->trace_count++]=(RmalTraceEvent){pc,in.line,in.column,op_name(in.op),xstrdup(detail?detail:"")};}
static Function*find_func(const RmalBytecode*b,const char*n){for(size_t i=0;i<b->func_count;i++)if(strcmp(b->funcs[i].name,n)==0)return &b->funcs[i];return NULL;}
static bool require_int(const RmalValue*v,int64_t*out,RmalStatus*st,int line,int col){if(v->kind!=RMAL_VALUE_INT){set_status(st,false,line,col,"expected integer");return false;}*out=v->as.integer;return true;}
static bool require_number(const RmalValue*v,double*out,RmalStatus*st,int line,int col){
    if(v->kind==RMAL_VALUE_INT){*out=(double)v->as.integer;return true;}
    if(v->kind==RMAL_VALUE_FLOAT&&isfinite(v->as.real)){*out=v->as.real;return true;}
    set_status(st,false,line,col,"expected finite number");return false;
}
static RmalStatus exec_code(RmalVm*,const RmalBytecode*,const InsVec*,Env*,const Function*,RmalValue*,bool);

static RmalStatus exec_code(RmalVm*vm,const RmalBytecode*b,const InsVec*code,Env*locals,const Function*fn,RmalValue*result,bool tracing){
    RmalStatus st={.ok=true};Stack stack={0};
    if(vm->call_depth>=256){set_status(&st,false,0,0,"call depth limit exceeded");return st;}
    vm->call_depth++;
    for(size_t pc=0;pc<code->count;pc++){
        Ins in=code->data[pc];
        if(vm->steps>=vm->step_limit){set_status(&st,false,in.line,in.column,"instruction limit exceeded");goto done;}
        vm->steps++;
        if(tracing)trace_add(vm,pc,in,"");RmalValue a=value_nil(),bb=value_nil(),r=value_nil();
        if(in.op==OP_PUSH){if(!stack_push(&stack,b->consts[in.a]))goto oom;continue;}
        if(in.op==OP_LOAD){const char*n=b->names[in.a];Binding*x=locals?env_find(locals,n):NULL;if(!x)x=env_find(&vm->globals,n);if(!x){set_status(&st,false,in.line,in.column,"undefined name: %s",n);goto done;}if(!stack_push(&stack,x->value))goto oom;continue;}
        if(in.op==OP_DEFINE){if(!stack_pop(&stack,&a)){set_status(&st,false,in.line,in.column,"stack underflow");goto done;}Env*e=fn?locals:&vm->globals;if(!env_define(e,b->names[in.a],a,(BindKind)in.b)){set_status(&st,false,in.line,in.column,"binding already defined: %s",b->names[in.a]);rmal_value_free(&a);goto done;}rmal_value_free(&a);continue;}
        if(in.op==OP_STORE){if(!stack_pop(&stack,&a)){set_status(&st,false,in.line,in.column,"stack underflow");goto done;}const char*n=b->names[in.a];Binding*x=locals?env_find(locals,n):NULL;if(!x)x=env_find(&vm->globals,n);if(!x){set_status(&st,false,in.line,in.column,"SET of undefined binding: %s",n);rmal_value_free(&a);goto done;}if(x->kind==BIND_CONST){set_status(&st,false,in.line,in.column,"cannot SET CONST binding: %s",n);rmal_value_free(&a);goto done;}rmal_value_free(&x->value);x->value=value_clone(&a);rmal_value_free(&a);continue;}
        if(in.op==OP_PRINT){if(!stack_pop(&stack,&a)){set_status(&st,false,in.line,in.column,"stack underflow");goto done;}char*s=rmal_value_render(&a);puts(s?s:"");free(s);rmal_value_free(&a);continue;}
        if(in.op==OP_ASSERT){if(!stack_pop(&stack,&a)){set_status(&st,false,in.line,in.column,"stack underflow");goto done;}bool ok=value_truthy(&a);rmal_value_free(&a);if(!ok){set_status(&st,false,in.line,in.column,"require/assert failed");goto done;}continue;}
        if(in.op==OP_POP){if(!stack_pop(&stack,&a)){set_status(&st,false,in.line,in.column,"stack underflow");goto done;}rmal_value_free(&a);continue;}
        if(in.op==OP_JUMP){pc=(size_t)in.a-1;continue;}
        if(in.op==OP_JFALSE){if(!stack_pop(&stack,&a)){set_status(&st,false,in.line,in.column,"stack underflow");goto done;}bool f=!value_truthy(&a);rmal_value_free(&a);if(f)pc=(size_t)in.a-1;continue;}
        if(in.op==OP_DIRECTIVE)continue;
        if(in.op==OP_HALT){*result=stack.count?stack.data[stack.count-1]:value_nil();if(stack.count)stack.count--;goto done;}
        if(in.op==OP_RET){*result=stack.count?stack.data[stack.count-1]:value_nil();if(stack.count)stack.count--;goto done;}
        if(in.op==OP_CALL){
            size_t argc=(size_t)in.b;RmalValue*args=calloc(argc,sizeof*args);if(argc&&!args)goto oom;
            bool popped=true;
            for(size_t i=argc;i-->0;)if(!stack_pop(&stack,&args[i])){popped=false;break;}
            if(!popped){for(size_t i=0;i<argc;i++)rmal_value_free(&args[i]);free(args);set_status(&st,false,in.line,in.column,"stack underflow in call");goto done;}
            const char*name=b->names[in.a];Function*f=find_func(b,name);RmalValue rv=value_nil();
            if(!f){
                RmalStatus hs={.ok=true};bool handled=vm->host&&vm->host(vm->host_data,name,args,argc,&rv,&hs);
                for(size_t i=0;i<argc;i++)rmal_value_free(&args[i]);free(args);
                if(!handled){rmal_value_free(&rv);set_status(&st,false,in.line,in.column,"undefined function: %s",name);goto done;}
                if(!hs.ok){st=hs;if(st.line<=0){st.line=in.line;st.column=in.column;}rmal_value_free(&rv);goto done;}
                if(rv.kind<RMAL_VALUE_NIL||rv.kind>RMAL_VALUE_FLOAT||(rv.kind==RMAL_VALUE_FLOAT&&!isfinite(rv.as.real))){
                    rmal_value_free(&rv);set_status(&st,false,in.line,in.column,"invalid or non-finite host result");goto done;
                }
            }else{
                if(argc!=f->param_count){set_status(&st,false,in.line,in.column,"arity mismatch calling %s",f->name);for(size_t i=0;i<argc;i++)rmal_value_free(&args[i]);free(args);goto done;}
                Env call_env={0};bool bound=true;
                for(size_t i=0;i<argc;i++){if(!env_define(&call_env,f->params[i],args[i],BIND_LET))bound=false;rmal_value_free(&args[i]);}free(args);
                if(!bound){env_free(&call_env);set_status(&st,false,in.line,in.column,"cannot bind function arguments");goto done;}
                RmalStatus cs=exec_code(vm,b,&f->code,&call_env,f,&rv,tracing);env_free(&call_env);
                if(!cs.ok){st=cs;rmal_value_free(&rv);goto done;}
            }
            if(!stack_push(&stack,rv)){rmal_value_free(&rv);goto oom;}rmal_value_free(&rv);continue;
        }
        if(in.op==OP_NEG||in.op==OP_NOT){if(!stack_pop(&stack,&a)){set_status(&st,false,in.line,in.column,"stack underflow");goto done;}if(in.op==OP_NEG){if(a.kind==RMAL_VALUE_FLOAT){r=value_float(-a.as.real);}else{int64_t x;if(!require_int(&a,&x,&st,in.line,in.column)){rmal_value_free(&a);goto done;}r=value_int(-x);}}else r=value_bool(!value_truthy(&a));rmal_value_free(&a);if(!stack_push(&stack,r)){rmal_value_free(&r);goto oom;}rmal_value_free(&r);continue;}
        if(!stack_pop(&stack,&bb)||!stack_pop(&stack,&a)){set_status(&st,false,in.line,in.column,"stack underflow");rmal_value_free(&a);rmal_value_free(&bb);goto done;}
        if(in.op==OP_EQ||in.op==OP_NE){bool q=value_equal(&a,&bb);r=value_bool(in.op==OP_EQ?q:!q);}
        else if(in.op==OP_AND||in.op==OP_OR){bool x=value_truthy(&a),y=value_truthy(&bb);r=value_bool(in.op==OP_AND?x&&y:x||y);}
        else if(in.op==OP_ADD&&(a.kind==RMAL_VALUE_STRING||bb.kind==RMAL_VALUE_STRING)){char*sa=rmal_value_render(&a),*sb=rmal_value_render(&bb);size_t n=strlen(sa)+strlen(sb)+1;char*z=malloc(n);if(!z){free(sa);free(sb);goto oom;}snprintf(z,n,"%s%s",sa,sb);free(sa);free(sb);r.kind=RMAL_VALUE_STRING;r.as.string=z;}
        else if(a.kind==RMAL_VALUE_FLOAT||bb.kind==RMAL_VALUE_FLOAT){
            double x,y;if(!require_number(&a,&x,&st,in.line,in.column)||!require_number(&bb,&y,&st,in.line,in.column))goto numeric_fail;
            switch(in.op){
                case OP_ADD:r=value_float(x+y);break;
                case OP_SUB:r=value_float(x-y);break;
                case OP_MUL:r=value_float(x*y);break;
                case OP_DIV:if(y==0.0){set_status(&st,false,in.line,in.column,"division by zero");goto numeric_fail;}r=value_float(x/y);break;
                case OP_LT:r=value_bool(x<y);break;case OP_LE:r=value_bool(x<=y);break;
                case OP_GT:r=value_bool(x>y);break;case OP_GE:r=value_bool(x>=y);break;
                default:set_status(&st,false,in.line,in.column,"operator requires integer operands");goto numeric_fail;
            }
            if(r.kind==RMAL_VALUE_FLOAT&&!isfinite(r.as.real)){set_status(&st,false,in.line,in.column,"non-finite arithmetic result");goto numeric_fail;}
        }
        else {int64_t x,y;if(!require_int(&a,&x,&st,in.line,in.column)||!require_int(&bb,&y,&st,in.line,in.column)){rmal_value_free(&a);rmal_value_free(&bb);goto done;}switch(in.op){case OP_ADD:r=value_int(x+y);break;case OP_SUB:r=value_int(x-y);break;case OP_MUL:r=value_int(x*y);break;case OP_DIV:if(y==0){set_status(&st,false,in.line,in.column,"division by zero");goto numeric_fail;}r=value_int(x/y);break;case OP_MOD:if(y==0){set_status(&st,false,in.line,in.column,"modulo by zero");goto numeric_fail;}r=value_int(x%y);break;case OP_LT:r=value_bool(x<y);break;case OP_LE:r=value_bool(x<=y);break;case OP_GT:r=value_bool(x>y);break;case OP_GE:r=value_bool(x>=y);break;default:set_status(&st,false,in.line,in.column,"invalid binary opcode");goto numeric_fail;} }
        rmal_value_free(&a);rmal_value_free(&bb);if(!stack_push(&stack,r)){rmal_value_free(&r);goto oom;}rmal_value_free(&r);continue;
numeric_fail:rmal_value_free(&a);rmal_value_free(&bb);rmal_value_free(&r);goto done;
    }
    *result=value_nil();goto done;
oom:set_status(&st,false,0,0,"out of memory during execution");
done:stack_free(&stack);vm->call_depth--;return st;
}

RmalStatus rmal_vm_run(RmalVm*vm,const RmalBytecode*b,bool trace,RmalValue*result){vm->steps=0;vm->call_depth=0;for(size_t i=0;i<vm->trace_count;i++)free(vm->trace[i].detail);vm->trace_count=0;env_free(&vm->globals);RmalValue r=value_nil();RmalStatus st=exec_code(vm,b,&b->main_code,NULL,NULL,&r,trace);if(result)*result=r;else rmal_value_free(&r);return st;}

typedef struct {char*data;size_t len,cap;} Str;
static bool str_appendf(Str*s,const char*fmt,...){va_list ap;va_start(ap,fmt);va_list aq;va_copy(aq,ap);int n=vsnprintf(NULL,0,fmt,aq);va_end(aq);if(n<0){va_end(ap);return false;}size_t need=s->len+(size_t)n+1;if(need>s->cap){size_t nc=s->cap? s->cap:256;while(nc<need)nc*=2;char*p=realloc(s->data,nc);if(!p){va_end(ap);return false;}s->data=p;s->cap=nc;}vsnprintf(s->data+s->len,s->cap-s->len,fmt,ap);s->len+=(size_t)n;va_end(ap);return true;}

char*rmal_disassemble(const RmalBytecode*b){Str s={0};str_appendf(&s,"RMALBC1 module=%s source=%s\n",b->module,b->source_name);for(size_t i=0;i<b->main_code.count;i++){Ins x=b->main_code.data[i];str_appendf(&s,"%zu\t%s\t%" PRId64 "\t%" PRId64 "\t@%d:%d\n",i,op_name(x.op),x.a,x.b,x.line,x.column);}for(size_t f=0;f<b->func_count;f++){str_appendf(&s,"fn %s\n",b->funcs[f].name);for(size_t i=0;i<b->funcs[f].code.count;i++){Ins x=b->funcs[f].code.data[i];str_appendf(&s,"  %zu\t%s\t%" PRId64 "\t%" PRId64 "\t@%d:%d\n",i,op_name(x.op),x.a,x.b,x.line,x.column);}}return s.data? s.data:xstrdup("");}

static void json_escape(Str*s,const char*x){str_appendf(s,"\"");for(;x&&*x;x++){if(*x=='\"'||*x=='\\')str_appendf(s,"\\%c",*x);else if(*x=='\n')str_appendf(s,"\\n");else str_appendf(s,"%c",*x);}str_appendf(s,"\"");}
