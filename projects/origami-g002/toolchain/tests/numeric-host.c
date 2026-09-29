#include "rmal/rmal.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

static bool host(void *context, const char *name, const RmalValue *args, size_t argc,
                 RmalValue *result, RmalStatus *status) {
    int *count = context;
    if (!strcmp(name, "offset")) {
        assert(argc == 1 && args[0].kind == RMAL_VALUE_FLOAT);
        *result = (RmalValue){.kind=RMAL_VALUE_FLOAT, .as.real=args[0].as.real + *count};
        (*count)++;
        status->ok = true;
        return true;
    }
    if (!strcmp(name, "host_fail")) {
        status->ok = false;
        strcpy(status->message, "host rejected request");
        return true;
    }
    if (!strcmp(name, "bad_float")) {
        *result = (RmalValue){.kind=RMAL_VALUE_FLOAT, .as.real=INFINITY};
        status->ok = true;
        return true;
    }
    if (!strcmp(name, "owned_string")) {
        char *s=malloc(6); assert(s); memcpy(s,"hello",6);
        *result = (RmalValue){.kind=RMAL_VALUE_STRING, .as.string=s};
        status->ok = true;
        return true;
    }
    return false;
}

static RmalStatus run(RmalVm *vm, const char *source, RmalValue *result) {
    RmalStatus status={0};
    RmalProgram *program=rmal_parse_source(source,"<numeric-host-test>",&status);
    if (!program) return status;
    RmalBytecode *bytecode=rmal_compile(program,&status);
    if (bytecode) {
        status=rmal_vm_run(vm,bytecode,true,result);
        rmal_bytecode_free(bytecode);
    }
    rmal_program_free(program);
    return status;
}

int main(void) {
    RmalVm *vm=rmal_vm_create(); assert(vm);
    int count=3; rmal_vm_set_host(vm,host,&count);
    RmalValue result={0};
    RmalStatus status=run(vm,"fn bridge(x) { return offset(x); } return bridge(0.5);",&result);
    assert(status.ok && result.kind==RMAL_VALUE_FLOAT && result.as.real==3.5 && count==4);
    assert(rmal_vm_trace_count(vm)>0); rmal_value_free(&result);
    status=run(vm,"return owned_string();",&result);
    assert(status.ok && result.kind==RMAL_VALUE_STRING && !strcmp(result.as.string,"hello"));
    rmal_value_free(&result);
    status=run(vm,"return host_fail();",&result);
    assert(!status.ok && status.line==1 && strstr(status.message,"host rejected"));
    status=run(vm,"return bad_float();",&result);
    assert(!status.ok && strstr(status.message,"non-finite"));
    status=run(vm,"return absent();",&result);
    assert(!status.ok && strstr(status.message,"undefined function"));
    const char *bad[]={"return 1e;","return 1e+;","return 1.2.3;","return 1e999;","return 1.0 / 0.0;","return 1e308 * 1e308;","return 2.0 % 1.0;"};
    for(size_t i=0;i<sizeof bad/sizeof bad[0];i++){ status=run(vm,bad[i],&result); assert(!status.ok); }
    rmal_vm_set_step_limit(vm,100);
    status=run(vm,"fn spin() { while true { } } spin();",&result);
    assert(!status.ok && strstr(status.message,"instruction limit"));
    status=run(vm,"return 0.125e1;",&result);
    assert(status.ok && result.kind==RMAL_VALUE_FLOAT && result.as.real==1.25);
    rmal_value_free(&result);
    rmal_vm_set_step_limit(vm,0);
    status=run(vm,"fn recurse() { return recurse(); } recurse();",&result);
    assert(!status.ok && strstr(status.message,"call depth"));
    rmal_vm_free(vm);
    puts("numeric host, finite arithmetic, rejection, and resource bounds: PASS");
    return 0;
}
