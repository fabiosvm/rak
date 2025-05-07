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

RakFunction *rak_function_new(RakError *err)
{
  RakFunction *fn = rak_memory_alloc(sizeof(*fn), err);
  if (!rak_is_ok(err)) return NULL;
  rak_object_init(&fn->obj);
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
  RakObject *obj = &fn->obj;
  --obj->refCount;
  if (obj->refCount) return;
  rak_function_free(fn);
}

RakClosure *rak_closure_new_function(int arity, RakFunction *fn, RakError *err)
{
  RakClosure *cl = rak_memory_alloc(sizeof(*cl), err);
  if (!rak_is_ok(err)) return NULL;
  rak_object_init(&cl->obj);
  cl->arity = arity;
  cl->kind = RAK_CLOSURE_KIND_FUNCTION;
  cl->as.fn = fn;
  rak_object_retain(&fn->obj);
  return cl;
}

RakClosure *rak_closure_new_native_function(int arity, RakNativeFunction native, RakError *err)
{
  RakClosure *cl = rak_memory_alloc(sizeof(*cl), err);
  if (!rak_is_ok(err)) return NULL;
  rak_object_init(&cl->obj);
  cl->arity = arity;
  cl->kind = RAK_CLOSURE_KIND_NATIVE_FUNCTION;
  cl->as.native = native;
  return cl;
}

void rak_closure_free(RakClosure *cl)
{
  RakClosureKind kind = cl->kind;
  if (kind == RAK_CLOSURE_KIND_FUNCTION)
  {
    rak_function_free(cl->as.fn);
    return;
  }
  rak_memory_free(cl);
}

void rak_closure_release(RakClosure *cl)
{
  RakObject *obj = &cl->obj;
  --obj->refCount;
  if (obj->refCount) return;
  rak_closure_free(cl);
}
