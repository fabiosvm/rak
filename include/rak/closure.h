//
// closure.h
//
// Copyright 2025 Fábio de Souza Villaça Medeiros
//
// This file is part of the Rak Project.
// For detailed license information, please refer to the LICENSE file
// located in the root directory of this project.
//

#ifndef RAK_CLOSURE_H
#define RAK_CLOSURE_H

#include "chunk.h"
#include "string.h"

typedef struct
{
  RakObject  obj;
  RakString *name;
  int        arity;
} RakCallable;

typedef struct RakFunction
{
  RakCallable                    callable;
  RakChunk                       chunk;
  RakSlice(struct RakFunction *) nested;
} RakFunction;

struct RakVM;

typedef void (*RakNativeFunctionCall)(struct RakVM *, RakValue *, RakError *);

typedef struct
{
  RakCallable           callable;
  RakNativeFunctionCall call;
} RakNativeFunction;

typedef enum
{
  RAK_CALLABLE_TYPE_FUNCTION,
  RAK_CALLABLE_TYPE_NATIVE_FUNCTION
} RakCallableType;

typedef struct
{
  RakObject        obj;
  RakCallableType  type;
  RakCallable     *callable;
} RakClosure;

void rak_callable_init(RakCallable *callable, RakString *name, int arity);
void rak_callable_deinit(RakCallable *callable);
RakFunction *rak_function_new(RakString *name, int arity, RakError *err);
void rak_function_free(RakFunction *fn);
void rak_function_release(RakFunction *fn);
uint8_t rak_function_append_nested(RakFunction *fn, RakFunction *nested, RakError *err);
RakNativeFunction *rak_native_function_new(RakString *name, int arity,
  RakNativeFunctionCall call, RakError *err);
void rak_native_function_free(RakNativeFunction *native);
void rak_native_function_release(RakNativeFunction *native);
RakClosure *rak_closure_new(RakCallableType type, RakCallable *callable, RakError *err);
void rak_closure_free(RakClosure *cl);
void rak_closure_release(RakClosure *cl);

#endif // RAK_CLOSURE_H
