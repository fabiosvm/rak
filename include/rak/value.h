//
// value.h
//
// Copyright 2025 Fábio de Souza Villaça Medeiros
//
// This file is part of the Rak Project.
// For detailed license information, please refer to the LICENSE file
// located in the root directory of this project.
//

#ifndef RAK_VALUE_H
#define RAK_VALUE_H

#include <stdint.h>

typedef enum
{
  RAK_TYPE_NIL,
  RAK_TYPE_NUMBER
} RakType;

typedef struct
{
  RakType type;
  union 
  {
    double f64;
  } as;
} RakValue;

#endif // RAK_VALUE_H
