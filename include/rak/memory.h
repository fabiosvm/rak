//
// memory.h
//
// Copyright 2025 Fábio de Souza Villaça Medeiros
//
// This file is part of the Rak Project.
// For detailed license information, please refer to the LICENSE file
// located in the root directory of this project.
//

#ifndef RAK_MEMORY_H
#define RAK_MEMORY_H

#include <stddef.h>
#include "error.h"

void *rak_memory_alloc(size_t size, RakError *err);
void *rak_memory_realloc(void *ptr, size_t size, RakError *err);
void rak_memory_free(void *ptr);

#endif // RAK_MEMORY_H
