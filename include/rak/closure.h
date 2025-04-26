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

#include <stdbool.h>
#include "value.h"

typedef enum
{
  RAK_CLOSURE_KIND_NATIVE_FUNCTION
} RakClosureKind;

struct RakVM;

typedef void (*RakNativeFunction)(struct RakVM *, RakValue *, RakError *);

typedef struct
{
  RakObject      obj;
  int            arity;
  RakClosureKind kind;
  union 
  {
    RakNativeFunction native;
  } as;
} RakClosure;

RakClosure *rak_closure_new_native_function(int arity, RakNativeFunction native, RakError *err);
void rak_closure_free(RakClosure *closure);
void rak_closure_release(RakClosure *closure);

#endif // RAK_CLOSURE_H
