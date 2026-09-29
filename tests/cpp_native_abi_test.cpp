#include "rmal/rmal.h"
int main() { auto* vm = rmal_vm_create(); if (!vm) return 1; rmal_vm_free(vm); }
