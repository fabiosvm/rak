//
// builtin.h
//
// Copyright 2025 Fábio de Souza Villaça Medeiros
//
// This file is part of the Rak Project.
// For detailed license information, please refer to the LICENSE file
// located in the root directory of this project.
//

#ifndef RAK_BUILTIN_H
#define RAK_BUILTIN_H

#include "array.h"

RakArray *rak_builtin_globals(RakError *err);
int rak_builtin_resolve_global(int len, char *chars);

#endif // RAK_BUILTIN_H
