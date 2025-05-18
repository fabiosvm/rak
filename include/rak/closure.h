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

#include "callable.h"

typedef struct RakClosure
{
  RakObject        obj;
  RakCallableType  type;
  RakCallable     *callable;
} RakClosure;

RakClosure *rak_closure_new(RakCallableType type, RakCallable *callable, RakError *err);
void rak_closure_free(RakClosure *cl);
void rak_closure_release(RakClosure *cl);

#endif // RAK_CLOSURE_H
