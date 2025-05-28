//
// error.h
//
// Copyright 2025 Fábio de Souza Villaça Medeiros
//
// This file is part of the Rak Project.
// For detailed license information, please refer to the LICENSE file
// located in the root directory of this project.
//

#ifndef RAK_ERROR_H
#define RAK_ERROR_H

#include <stdarg.h>
#include <stdbool.h>

#define RAK_ERROR_MAX_LEN (511)

#define rak_is_ok(e) ((e)->ok)

typedef struct
{
  bool ok;
  char cstr[RAK_ERROR_MAX_LEN + 1];
} RakError;

void rak_error_init(RakError *err);
void rak_error_set(RakError *err, const char *fmt, ...);
void rak_error_set_with_args(RakError *err, const char *fmt, va_list args);
void rak_error_print(RakError *err);

#endif // RAK_ERROR_H
