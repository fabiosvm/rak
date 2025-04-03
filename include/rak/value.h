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
#include "error.h"

#define RAK_FALG_FALSY (1 << 0)

#define RAK_NUMBER_EPSILON (1e-9)
#define RAK_INTEGER_MIN    (-9007199254740992LL)
#define RAK_INTEGER_MAX    (9007199254740992LL)

#define rak_nil_value()     ((RakValue) { .type = RAK_TYPE_NIL, .flags = RAK_FALG_FALSY })
#define rak_bool_value(d)   ((RakValue) { .type = RAK_TYPE_BOOL, .flags = (d) ? 0 : RAK_FALG_FALSY, .opaque.b = (d) })
#define rak_number_value(d) ((RakValue) { .type = RAK_TYPE_NUMBER, .flags = 0, .opaque.f64 = (d) })

#define rak_as_bool(v)    ((v).opaque.b)
#define rak_as_number(v)  ((v).opaque.f64)
#define rak_as_integer(v) ((int64_t) rak_as_number(v))

#define rak_is_nil(v)     ((v).type == RAK_TYPE_NIL)
#define rak_is_bool(v)    ((v).type == RAK_TYPE_BOOL)
#define rak_is_number(v)  ((v).type == RAK_TYPE_NUMBER)
#define rak_is_integer(v) (rak_as_number(v) == rak_as_integer(v))
#define rak_is_falsy(v)   ((v).flags & RAK_FALG_FALSY)

typedef enum
{
  RAK_TYPE_NIL,
  RAK_TYPE_BOOL,
  RAK_TYPE_NUMBER
} RakType;

typedef union
{
  bool   b;
  double f64;
} RakOpaque;

typedef struct
{
  RakType   type;
  int       flags;
  RakOpaque opaque;
} RakValue;

RakValue rak_number_value_from_cstr(int len, const char *cstr, RakError *err);
int rak_number_compare(double num1, double num2);
bool rak_value_equals(RakValue val1, RakValue val2);
int rak_value_compare(RakValue val1, RakValue val2, RakError *err);
void rak_value_print(RakValue val);

#endif // RAK_VALUE_H
