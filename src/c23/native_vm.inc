/* Host-only capability bindings. Callbacks must not free/reenter the owning VM.
 * Arguments are borrowed for the call; result ownership transfers to the VM.
 * No dynamic loading, ambient capabilities, or script-side binding API exists. */
typedef struct {
    char *name;
    size_t arity;
    RmalNativeFunction callback;
    void *context;
} NativeBinding;
struct RmalVm {
    Env globals;
    RmalTraceEvent *trace;
    size_t trace_count, trace_cap;
    NativeBinding *native;
    size_t native_count;
    bool running;
};
RmalVm *rmal_vm_create(void){return calloc(1,sizeof(RmalVm));}
void rmal_vm_free(RmalVm *v){
    if(!v)return;
    env_free(&v->globals);
    for(size_t i=0;i<v->trace_count;i++)free(v->trace[i].detail);
    for(size_t i=0;i<v->native_count;i++)free(v->native[i].name);
    free(v->native);free(v->trace);free(v);
}
static NativeBinding *find_native(RmalVm *v,const char *name){
    for(size_t i=0;i<v->native_count;i++)if(strcmp(v->native[i].name,name)==0)return &v->native[i];
    return NULL;
}
RmalStatus rmal_vm_bind_native(RmalVm *v,const char *name,size_t arity,
                               RmalNativeFunction callback,void *context){
    RmalStatus st={.ok=false};
    if(!v||!name||!*name||!callback||arity>64){set_status(&st,false,0,0,"invalid native binding");return st;}
    if(v->running){set_status(&st,false,0,0,"cannot bind native function during execution");return st;}
    for(size_t i=0;name[i];i++){
        const unsigned char ch=(unsigned char)name[i];
        if(i>=128||!((ch>='a'&&ch<='z')||(ch>='A'&&ch<='Z')||ch=='_'||(i>0&&ch>='0'&&ch<='9'))){
            set_status(&st,false,0,0,"invalid native function name");return st;
        }
    }
    if(find_native(v,name)||v->native_count>=64){set_status(&st,false,0,0,"duplicate or excess native binding");return st;}
    char *copy=xstrdup(name);if(!copy){set_status(&st,false,0,0,"native binding allocation failed");return st;}
    NativeBinding *next=realloc(v->native,(v->native_count+1)*sizeof *next);
    if(!next){free(copy);set_status(&st,false,0,0,"native binding allocation failed");return st;}
    v->native=next;v->native[v->native_count++]=(NativeBinding){copy,arity,callback,context};st.ok=true;return st;
}
