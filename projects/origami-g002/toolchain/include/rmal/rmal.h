#ifndef RMAL_RMAL_H
#define RMAL_RMAL_H

#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 202311L
#error RMALC 3.1 requires ISO C23 or newer
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

typedef enum {
    RMAL_VALUE_NIL = 0,
    RMAL_VALUE_INT,
    RMAL_VALUE_BOOL,
    RMAL_VALUE_STRING,
    RMAL_VALUE_FLOAT
} RmalValueKind;

typedef struct {
    RmalValueKind kind;
    union {
        int64_t integer;
        double real;
        bool boolean;
        char *string;
    } as;
} RmalValue;

typedef struct {
    size_t pc;
    int line;
    int column;
    const char *op;
    char *detail;
} RmalTraceEvent;

typedef struct RmalProgram RmalProgram;
typedef struct RmalBytecode RmalBytecode;
typedef struct RmalVm RmalVm;

typedef struct {
    bool ok;
    int line;
    int column;
    char message[512];
} RmalStatus;

const char *rmal_version(void);
const char *rmal_language_name(void);
const char *rmal_compiler_name(void);

RmalProgram *rmal_parse_source(const char *source, const char *source_name, RmalStatus *status);
void rmal_program_free(RmalProgram *program);

RmalBytecode *rmal_compile(const RmalProgram *program, RmalStatus *status);
void rmal_bytecode_free(RmalBytecode *bytecode);

char *rmal_disassemble(const RmalBytecode *bytecode);
char *rmal_manifest(const RmalBytecode *bytecode, bool json);
char *rmal_audit(const RmalProgram *program, const RmalBytecode *bytecode);
char *rmal_language_spec(bool json);

/* Host arguments are borrowed for this call. A handled callback returns true,
 * sets status.ok, and transfers any malloc-owned result string to the VM.
 * Returning false means unknown function. RMAL functions take precedence. */
typedef bool (*RmalHostFunction)(void *user_data, const char *name,
    const RmalValue *args, size_t argc, RmalValue *result, RmalStatus *status);
RmalVm *rmal_vm_create(void);
void rmal_vm_set_host(RmalVm *vm, RmalHostFunction callback, void *user_data);
/* Shared across nested calls; reset on each run. Zero restores 1,000,000. */
void rmal_vm_set_step_limit(RmalVm *vm, size_t limit);
void rmal_vm_free(RmalVm *vm);
RmalStatus rmal_vm_run(RmalVm *vm, const RmalBytecode *bytecode, bool trace, RmalValue *result);
size_t rmal_vm_trace_count(const RmalVm *vm);
const RmalTraceEvent *rmal_vm_trace_at(const RmalVm *vm, size_t index);

void rmal_value_free(RmalValue *value);
char *rmal_value_render(const RmalValue *value);

bool rmal_selfcheck(char **report);

#endif
