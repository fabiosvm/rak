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

#include "closure.h"

#define RAK_COMPILER_MAX_SYMBOLS (UINT8_MAX + 1)

RakClosure *rak_compile(RakString *file, RakString *source, RakError *err);

#endif // RAK_COMPILER_H
