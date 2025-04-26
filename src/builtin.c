//
// builtin.c
//
// Copyright 2025 Fábio de Souza Villaça Medeiros
//
// This file is part of the Rak Project.
// For detailed license information, please refer to the LICENSE file
// located in the root directory of this project.
//

#include "rak/builtin.h"
#include <string.h>

static const char *globals[] = {
  "println"
};

static void println_native(RakVM *vm, RakValue *slots, RakError *err);

static void println_native(RakVM *vm, RakValue *slots, RakError *err)
{
  RakValue val = slots[1];
  rak_value_print(val);
  printf("\n");
  rak_vm_push_nil(vm, err);
}

void rak_builtin_load_globals(RakVM *vm, RakError *err)
{
  RakClosure *closure = rak_closure_new_native_function(1, println_native, err);
  if (!rak_is_ok(err)) return;
  RakValue val = rak_closure_value(closure);
  rak_vm_push_object(vm, val, err);
}

int rak_builtin_resolve_global(int len, char *chars)
{
  int n = (int) (sizeof(globals) / sizeof(*globals));
  for (int i = 0; i < n; ++i)
  {
    const char *cstr = globals[i];
    if (!memcmp(cstr, chars, len) && !cstr[len])
      return i;
  }
  return -1;
}
