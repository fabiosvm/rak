//
// function.c
//
// Copyright 2025 Fábio de Souza Villaça Medeiros
//
// This file is part of the Rak Project.
// For detailed license information, please refer to the LICENSE file
// located in the root directory of this project.
//

#include "rak/function.h"

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
