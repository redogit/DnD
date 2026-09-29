#include "rmal/rmal.h"
#include <stdlib.h>
#include <string.h>
#ifndef RMAL_NATIVE_API_VERSION
int main(void){fputs("FAIL: explicit native callback API absent\n",stderr);return 1;}
#else
#define CHECK(x) do{if(!(x)){fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#x);return 1;}}while(0)
static RmalStatus probe(void *ctx,const RmalValue *args,size_t argc,RmalValue *result){
    RmalStatus st={.ok=false};
    if(argc!=1||args[0].kind!=RMAL_VALUE_INT){snprintf(st.message,sizeof st.message,"expected one integer");return st;}
    int *count=ctx; ++*count;
    *result=(RmalValue){.kind=RMAL_VALUE_INT,.as.integer=args[0].as.integer};st.ok=true;return st;
}
static RmalStatus run(RmalVm *vm,const char *source){
    RmalStatus st={0};RmalProgram *p=rmal_parse_source(source,"native-test",&st);
    if(!p)return st;RmalBytecode *b=rmal_compile(p,&st);
    if(!b){rmal_program_free(p);return st;}
    RmalValue result={0};st=rmal_vm_run(vm,b,true,&result);
    rmal_value_free(&result);rmal_bytecode_free(b);rmal_program_free(p);return st;
}
static RmalStatus reenter(void *ctx,const RmalValue *args,size_t n,RmalValue *result){
    (void)args;(void)n;(void)result;
    RmalStatus nested=run((RmalVm*)ctx,"assert true;");
    RmalStatus st={.ok=!nested.ok};if(!st.ok)snprintf(st.message,sizeof st.message,"reentry accepted");return st;
}
int main(void){
    RmalVm *vm=rmal_vm_create();CHECK(vm);int count=0;
    CHECK(!run(vm,"let x = AnyFunctor(7);").ok);CHECK(count==0);
    CHECK(rmal_vm_bind_native(vm,"AnyFunctor",1,probe,&count).ok);
    CHECK(!rmal_vm_bind_native(vm,"AnyFunctor",1,probe,&count).ok);
    CHECK(!rmal_vm_bind_native(vm,"bad name",1,probe,&count).ok);
    CHECK(run(vm,"let x = AnyFunctor(7); assert x == 7;").ok);CHECK(count==1);
    CHECK(!run(vm,"let x = AnyFunctor();").ok);CHECK(count==1);
    CHECK(!run(vm,"let x = AnyFunctor(\"wrong type\");").ok);CHECK(count==1);
    CHECK(!run(vm,"fn AnyFunctor(x) { return x; } let x = AnyFunctor(7);").ok);CHECK(count==1);
    CHECK(rmal_vm_bind_native(vm,"Reenter",0,reenter,vm).ok);
    CHECK(run(vm,"Reenter();").ok);
    CHECK(run(vm,"let x = AnyFunctor(9); assert x == 9;").ok);CHECK(count==2);
    rmal_vm_free(vm);puts("PASS native callback: opt-in, immutable name binding, typed errors, shadow rejection, no reentry");return 0;
}
#endif
