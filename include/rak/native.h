//
// native.h
//
// Copyright 2025 Fábio de Souza Villaça Medeiros
//
// This file is part of the Rak Project.
// For detailed license information, please refer to the LICENSE file
// located in the root directory of this project.
//

#ifndef RAK_NATIVE_H
#define RAK_NATIVE_H

#include "callable.h"

struct RakFiber;

struct RakClosure;

typedef void (*RakNativeFunctionCall)(struct RakFiber *, struct RakClosure *, int, RakValue *, RakError *);

typedef struct
{
  RakCallable           callable;
  RakNativeFunctionCall call;
} RakNativeFunction;

RakNativeFunction *rak_native_function_new(RakString *name, int arity,
  RakNativeFunctionCall call, RakError *err);
void rak_native_function_free(RakNativeFunction *native);
void rak_native_function_release(RakNativeFunction *native);

#endif // RAK_NATIVE_H
