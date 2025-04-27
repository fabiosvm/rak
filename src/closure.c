//
// closure.c
//
// Copyright 2025 Fábio de Souza Villaça Medeiros
//
// This file is part of the Rak Project.
// For detailed license information, please refer to the LICENSE file
// located in the root directory of this project.
//

#include "rak/closure.h"
#include "rak/memory.h"

RakClosure *rak_closure_new_native_function(int arity, RakNativeFunction native, RakError *err)
{
  RakClosure *closure = rak_memory_alloc(sizeof(*closure), err);
  if (!rak_is_ok(err)) return NULL;
  rak_object_init(&closure->obj);
  closure->arity = arity;
  closure->kind = RAK_CLOSURE_KIND_NATIVE_FUNCTION;
  closure->as.native = native;
  return closure;
}

void rak_closure_free(RakClosure *closure)
{
  rak_memory_free(closure);
}

void rak_closure_release(RakClosure *closure)
{
  RakObject *obj = &closure->obj;
  --obj->refCount;
  if (obj->refCount) return;
  rak_closure_free(closure);
}
