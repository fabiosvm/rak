//
// value.c
//
// Copyright 2025 Fábio de Souza Villaça Medeiros
//
// This file is part of the Rak Project.
// For detailed license information, please refer to the LICENSE file
// located in the root directory of this project.
//

#include "rak/value.h"
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rak/array.h"
#include "rak/string.h"

RakValue rak_number_value_from_cstr(int len, const char *cstr, RakError *err)
{
  if (len < 0) len = (int) strlen(cstr);
  char *end = NULL;
  errno = 0;
  double data = strtod(cstr, &end);
  if (errno || end != &cstr[len])
  {
    rak_error_set(err, "invalid number format");
    return rak_nil_value();
  }
  return rak_number_value(data);
}

int rak_number_compare(double num1, double num2)
{
  if (fabs(num1 - num2) < RAK_NUMBER_EPSILON)
    return 0;
  return (num1 > num2) ? 1 : -1;
}

void rak_value_free(RakValue val)
{
  switch (val.type)
  {
  case RAK_TYPE_NIL:
  case RAK_TYPE_BOOL:
  case RAK_TYPE_NUMBER:
    break;
  case RAK_TYPE_STRING:
    rak_string_free(rak_as_string(val));
    break;
  case RAK_TYPE_ARRAY:
    rak_array_free(rak_as_array(val));
    break;
  }
}

void rak_value_release(RakValue val)
{
  switch (val.type)
  {
  case RAK_TYPE_NIL:
  case RAK_TYPE_BOOL:
  case RAK_TYPE_NUMBER:
    break;
  case RAK_TYPE_STRING:
    rak_string_release(rak_as_string(val));
    break;
  case RAK_TYPE_ARRAY:
    rak_array_release(rak_as_array(val));
    break;
  }
}

bool rak_value_equals(RakValue val1, RakValue val2)
{
  if (val1.type != val2.type)
    return false;
  bool res = true;
  switch (val1.type)
  {
  case RAK_TYPE_NIL:
    break;
  case RAK_TYPE_BOOL:
    res = rak_as_bool(val1) == rak_as_bool(val2);
    break;
  case RAK_TYPE_NUMBER:
    res = !rak_number_compare(rak_as_number(val1), rak_as_number(val2));
    break;
  case RAK_TYPE_STRING:
    res = rak_string_equals(rak_as_string(val1), rak_as_string(val2));
    break;
  case RAK_TYPE_ARRAY:
    res = rak_array_equals(rak_as_array(val1), rak_as_array(val2));
    break;
  }
  return res;
}

int rak_value_compare(RakValue val1, RakValue val2, RakError *err)
{
  if (val1.type != val2.type)
  {
    rak_error_set(err, "cannot compare different types");
    return 0;
  }
  int res = 0;
  switch (val1.type)
  {
  case RAK_TYPE_NIL:
    break;
  case RAK_TYPE_BOOL:
    res = rak_as_bool(val1) - rak_as_bool(val2);
    break;
  case RAK_TYPE_NUMBER:
    res = rak_number_compare(rak_as_number(val1), rak_as_number(val2));
    break;
  case RAK_TYPE_STRING:
    res = rak_string_compare(rak_as_string(val1), rak_as_string(val2));
    break;
  case RAK_TYPE_ARRAY:
    rak_error_set(err, "cannot compare arrays");
    break;
  }
  return res;
}

void rak_value_print(RakValue val)
{
  switch (val.type)
  {
  case RAK_TYPE_NIL:
    printf("nil");
    break;
  case RAK_TYPE_BOOL:
    printf("%s", rak_as_bool(val) ? "true" : "false");
    break;
  case RAK_TYPE_NUMBER:
    printf("%g", rak_as_number(val));
    break;
  case RAK_TYPE_STRING:
    rak_string_print(rak_as_string(val));
    break;
  case RAK_TYPE_ARRAY:
    rak_array_print(rak_as_array(val));
    break;
  }
}
