//
// native.c
//
// Copyright 2025 Fábio de Souza Villaça Medeiros
//
// This file is part of the Rak Project.
// For detailed license information, please refer to the LICENSE file
// located in the root directory of this project.
//

#include "rak/native.h"

RakNativeFunction *rak_native_function_new(RakString *name, int arity,
  RakNativeFunctionCall call, RakError *err)
{
  RakNativeFunction *native = rak_memory_alloc(sizeof(*native), err);
  if (!rak_is_ok(err)) return NULL;
  rak_callable_init(&native->callable, name, arity);
  native->call = call;
  return native;
}

void rak_native_function_free(RakNativeFunction *native)
{
  rak_callable_deinit(&native->callable);
  rak_memory_free(native);
}

void rak_native_function_release(RakNativeFunction *native)
{
  RakObject *obj = &native->callable.obj;
  --obj->refCount;
  if (obj->refCount) return;
  rak_native_function_free(native);
}
