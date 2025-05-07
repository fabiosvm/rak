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

void rak_callable_init(RakCallable *callable, int arity)
{
  rak_object_init(&callable->obj);
  callable->arity = arity;
}

RakFunction *rak_function_new(int arity, RakError *err)
{
  RakFunction *fn = rak_memory_alloc(sizeof(*fn), err);
  if (!rak_is_ok(err)) return NULL;
  rak_callable_init(&fn->callable, arity);
  rak_chunk_init(&fn->chunk, err);
  if (rak_is_ok(err)) return fn;
  rak_memory_free(fn);
  return NULL;
}

void rak_function_free(RakFunction *fn)
{
  rak_chunk_deinit(&fn->chunk);
  rak_memory_free(fn);
}

void rak_function_release(RakFunction *fn)
{
  RakObject *obj = &fn->callable.obj;
  --obj->refCount;
  if (obj->refCount) return;
  rak_function_free(fn);
}

RakNativeFunction *rak_native_function_new(int arity, RakNativeFunctionCall call,
  RakError *err)
{
  RakNativeFunction *native = rak_memory_alloc(sizeof(*native), err);
  if (!rak_is_ok(err)) return NULL;
  rak_callable_init(&native->callable, arity);
  native->call = call;
  return native;
}

void rak_native_function_free(RakNativeFunction *native)
{
  rak_memory_free(native);
}

void rak_native_function_release(RakNativeFunction *native)
{
  RakObject *obj = &native->callable.obj;
  --obj->refCount;
  if (obj->refCount) return;
  rak_native_function_free(native);
}

RakClosure *rak_closure_new(RakCallableKind kind, RakCallable *callable, RakError *err)
{
  RakClosure *cl = rak_memory_alloc(sizeof(*cl), err);
  if (!rak_is_ok(err)) return NULL;
  rak_object_init(&cl->obj);
  cl->kind = kind;
  cl->callable = callable;
  rak_object_retain(&callable->obj);
  return cl;
}

void rak_closure_free(RakClosure *cl)
{
  if (cl->kind == RAK_CALLABLE_KIND_FUNCTION)
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
