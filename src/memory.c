//
// memory.c
//
// Copyright 2025 Fábio de Souza Villaça Medeiros
//
// This file is part of the Rak Project.
// For detailed license information, please refer to the LICENSE file
// located in the root directory of this project.
//

#include "rak/memory.h"
#include <stdlib.h>

void *rak_memory_alloc(size_t size, RakError *err)
{
  void *ptr = malloc(size);
  if (!ptr) rak_error_set(err, "out of memory");
  return ptr;
}

void *rak_memory_realloc(void *ptr, size_t size, RakError *err)
{
  void *_ptr = realloc(ptr, size);
  if (!_ptr) rak_error_set(err, "out of memory");
  return _ptr;
}

void rak_memory_free(void *ptr)
{
  free(ptr);
}
