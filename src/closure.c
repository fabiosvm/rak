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

void rak_callable_init(RakCallable *callable, RakString *name, int arity)
{
  rak_object_init(&callable->obj);
  callable->name = name;
  if (name) rak_object_retain(&name->obj);
  callable->arity = arity;
}

void rak_callable_deinit(RakCallable *callable)
{
  RakString *name = callable->name;
  if (!name) return;
  rak_string_release(name);
}

RakFunction *rak_function_new(RakString *name, int arity, RakError *err)
{
  RakFunction *fn = rak_memory_alloc(sizeof(*fn), err);
  if (!rak_is_ok(err)) return NULL;
  rak_callable_init(&fn->callable, name, arity);
  rak_chunk_init(&fn->chunk, err);
  if (!rak_is_ok(err))
  {
    rak_callable_deinit(&fn->callable);
    rak_memory_free(fn);
    return NULL;
  }
  rak_slice_init(&fn->nested, err);
  if (rak_is_ok(err)) return fn;
  rak_callable_deinit(&fn->callable);
  rak_chunk_deinit(&fn->chunk);
  rak_memory_free(fn);
  return NULL;
}

void rak_function_free(RakFunction *fn)
{
  rak_callable_deinit(&fn->callable);
  rak_chunk_deinit(&fn->chunk);
  for (int i = 0; i < fn->nested.len; ++i)
  {
    RakFunction *nested = rak_slice_get(&fn->nested, i);
    rak_function_release(nested);
  }
  rak_slice_deinit(&fn->nested);
  rak_memory_free(fn);
}

void rak_function_release(RakFunction *fn)
{
  RakObject *obj = &fn->callable.obj;
  --obj->refCount;
  if (obj->refCount) return;
  rak_function_free(fn);
}

uint8_t rak_function_append_nested(RakFunction *fn, RakFunction *nested, RakError *err)
{
  int idx = fn->nested.len;
  if (idx > UINT8_MAX)
  {
    rak_error_set(err, "too many nested functions");
    return 0;
  }
  rak_slice_ensure_append(&fn->nested, nested, err);
  if (!rak_is_ok(err)) return 0;
  rak_object_retain(&nested->callable.obj);
  return (uint8_t) idx;
}

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
