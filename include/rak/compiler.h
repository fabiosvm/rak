//
// compiler.h
//
// Copyright 2025 Fábio de Souza Villaça Medeiros
//
// This file is part of the Rak Project.
// For detailed license information, please refer to the LICENSE file
// located in the root directory of this project.
//

#ifndef RAK_COMPILER_H
#define RAK_COMPILER_H

#include "chunk.h"

void rak_compile(char *source, RakChunk *chunk, RakError *err);

#endif // RAK_COMPILER_H
