//
// callable.c
//
// Copyright 2025 Fábio de Souza Villaça Medeiros
//
// This file is part of the Rak Project.
// For detailed license information, please refer to the LICENSE file
// located in the root directory of this project.
//

#include "rak/callable.h"

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
