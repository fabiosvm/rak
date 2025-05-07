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

typedef enum
{
  RAK_CLOSURE_KIND_FUNCTION,
  RAK_CLOSURE_KIND_NATIVE_FUNCTION
} RakClosureKind;

typedef struct
{
  RakObject obj;
  RakChunk  chunk;
} RakFunction;

struct RakVM;

typedef void (*RakNativeFunction)(struct RakVM *, RakValue *, RakError *);

typedef struct
{
  RakObject      obj;
  int            arity;
  RakClosureKind kind;
  union 
  {
    RakFunction       *fn;
    RakNativeFunction  native;
  } as;
} RakClosure;

RakFunction *rak_function_new(RakError *err);
void rak_function_free(RakFunction *fn);
void rak_function_release(RakFunction *fn);
RakClosure *rak_closure_new_function(int arity, RakFunction *fn, RakError *err);
RakClosure *rak_closure_new_native_function(int arity, RakNativeFunction native, RakError *err);
void rak_closure_free(RakClosure *cl);
void rak_closure_release(RakClosure *cl);

#endif // RAK_CLOSURE_H
