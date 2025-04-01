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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

RakValue rak_number_value_from_cstr(int len, const char *cstr, RakError *err)
{
  if (len < 0) len = (int) strlen(cstr);
  char *end = NULL;
  errno = 0;
  double num = strtod(cstr, &end);
  if (errno || end != &cstr[len])
  {
    rak_error_set(err, "invalid number format");
    return rak_nil_value();
  }
  return rak_number_value(num);
}

void rak_value_print(RakValue val)
{
  switch (val.type)
  {
  case RAK_TYPE_NIL:
    printf("nil");
    break;
  case RAK_TYPE_NUMBER:
    printf("%g", rak_as_number(val));
    break;
  }
}
