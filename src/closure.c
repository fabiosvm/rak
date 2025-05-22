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
#include "rak/function.h"
#include "rak/native.h"

RakClosure *rak_closure_new(RakCallableType type, RakCallable *callable, RakError *err)
{
  RakClosure *cl = rak_memory_alloc(sizeof(*cl), err);
  if (!rak_is_ok(err)) return NULL;
  rak_object_init(&cl->obj);
  cl->type = type;
  cl->callable = callable;
  rak_object_retain(&callable->obj);
  return cl;
}

void rak_closure_free(RakClosure *cl)
{
  if (cl->type == RAK_CALLABLE_TYPE_FUNCTION)
  {
    RakFunction *fn = (RakFunction *) cl->callable;
    rak_function_release(fn);
    rak_memory_free(cl);
    return;
  }
  RakNativeFunction *native = (RakNativeFunction *) cl->callable;
  rak_native_function_release(native);
  rak_memory_free(cl);
}

void rak_closure_release(RakClosure *cl)
{
  RakObject *obj = &cl->obj;
  --obj->refCount;
  if (obj->refCount) return;
  rak_closure_free(cl);
}
