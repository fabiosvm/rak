//
// function.h
//
// Copyright 2025 Fábio de Souza Villaça Medeiros
//
// This file is part of the Rak Project.
// For detailed license information, please refer to the LICENSE file
// located in the root directory of this project.
//

#ifndef RAK_FUNCTION_H
#define RAK_FUNCTION_H

#include "callable.h"
#include "chunk.h"

#define RAK_FUNCTION_MAX_NESTED_FUNCTIONS (UINT8_MAX + 1)

typedef struct RakFunction
{
  RakCallable                    callable;
  RakChunk                       chunk;
  RakSlice(struct RakFunction *) nested;
} RakFunction;

RakFunction *rak_function_new(RakString *name, int arity, RakError *err);
void rak_function_free(RakFunction *fn);
void rak_function_release(RakFunction *fn);
uint8_t rak_function_append_nested(RakFunction *fn, RakFunction *nested, RakError *err);

#endif // RAK_FUNCTION_H
