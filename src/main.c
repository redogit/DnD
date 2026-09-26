#include "rmal/rmal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *read_file(const char *path) {
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    if (fseek(f, 0, SEEK_END) != 0) { fclose(f); return NULL; }
    long n = ftell(f);
    if (n < 0) { fclose(f); return NULL; }
    rewind(f);
    char *buf = malloc((size_t)n + 1);
    if (!buf) { fclose(f); return NULL; }
    size_t got = fread(buf, 1, (size_t)n, f);
    fclose(f);
    if (got != (size_t)n) { free(buf); return NULL; }
    buf[got] = '\0';
    return buf;
}

static int fail_status(const RmalStatus *st) {
    if (st && st->line > 0)
        fprintf(stderr, "RMALC ERROR %d:%d: %s\n", st->line, st->column, st->message);
    else
        fprintf(stderr, "RMALC ERROR: %s\n", st ? st->message : "unknown error");
    return 1;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "rmalc <version|language-spec|selfcheck|check|compile|manifest|run|trace|audit> [file]\n");
        return 2;
    }
    const char *cmd = argv[1];
    if (!strcmp(cmd, "version")) {
        printf("RMAL %s\nRMALC %s\nimplementation ISO C23\n", rmal_version(), rmal_version());
        return 0;
    }
    if (!strcmp(cmd, "language-spec")) {
        bool json = argc >= 4 && !strcmp(argv[2], "--format") && !strcmp(argv[3], "json");
        char *s = rmal_language_spec(json);
        fputs(s, stdout);
        free(s);
        return 0;
    }
    if (!strcmp(cmd, "selfcheck")) {
        char *report = NULL;
        bool ok = rmal_selfcheck(&report);
        if (report) { fputs(report, stdout); free(report); }
        return ok ? 0 : 1;
    }
    if (argc < 3) {
        fprintf(stderr, "source file required\n");
        return 2;
    }
    const char *path = argv[2];
    char *source = read_file(path);
    if (!source) {
        fprintf(stderr, "cannot read %s\n", path);
        return 1;
    }
    RmalStatus st = {0};
    RmalProgram *p = rmal_parse_source(source, path, &st);
    free(source);
    if (!p) return fail_status(&st);
    RmalBytecode *b = rmal_compile(p, &st);
    if (!b) { rmal_program_free(p); return fail_status(&st); }

    int rc = 0;
    if (!strcmp(cmd, "check")) {
        puts("CHECK PASS");
    } else if (!strcmp(cmd, "compile")) {
        char *s = rmal_disassemble(b); fputs(s, stdout); free(s);
    } else if (!strcmp(cmd, "manifest")) {
        bool json = argc >= 4 && !strcmp(argv[3], "--json");
        char *s = rmal_manifest(b, json); fputs(s, stdout); free(s);
    } else if (!strcmp(cmd, "audit")) {
        char *s = rmal_audit(p, b); fputs(s, stdout); free(s);
    } else if (!strcmp(cmd, "run") || !strcmp(cmd, "trace")) {
        RmalVm *vm = rmal_vm_create();
        RmalValue result = {0};
        st = rmal_vm_run(vm, b, !strcmp(cmd, "trace"), &result);
        if (!st.ok) rc = fail_status(&st);
        if (!strcmp(cmd, "trace")) {
            for (size_t i = 0; i < rmal_vm_trace_count(vm); ++i) {
                const RmalTraceEvent *e = rmal_vm_trace_at(vm, i);
                printf("TRACE pc=%zu op=%s @%d:%d %s\n", e->pc, e->op, e->line, e->column, e->detail ? e->detail : "");
            }
        }
        rmal_value_free(&result);
        rmal_vm_free(vm);
    } else {
        fprintf(stderr, "unknown command: %s\n", cmd);
        rc = 2;
    }

    rmal_bytecode_free(b);
    rmal_program_free(p);
    return rc;
}
