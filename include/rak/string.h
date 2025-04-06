//
// string.h
//
// Copyright 2025 Fábio de Souza Villaça Medeiros
//
// This file is part of the Rak Project.
// For detailed license information, please refer to the LICENSE file
// located in the root directory of this project.
//

#ifndef RAK_STRING_H
#define RAK_STRING_H

#include "slice.h"
#include "value.h"

#define rak_string_cap(s)      ((s)->slice.cap)
#define rak_string_len(s)      ((s)->slice.len)
#define rak_string_chars(s)    ((s)->slice.data)
#define rak_string_is_empty(s) (!rak_string_len(s))

typedef struct
{
  RakObject      obj;
  RakSlice(char) slice;
} RakString;

void rak_string_init(RakString *str, RakError *err);
void rak_string_init_with_capacity(RakString *str, int cap, RakError *err);
void rak_string_init_from_cstr(RakString *str, int len, const char *cstr, RakError *err);
void rak_string_deinit(RakString *str);
RakString *rak_string_new(RakError *err);
RakString *rak_string_new_with_capacity(int cap, RakError *err);
RakString *rak_string_new_from_cstr(int len, const char *cstr, RakError *err);
void rak_string_free(RakString *str);
void rak_string_release(RakString *str);
void rak_string_ensure_capacity(RakString *str, int cap, RakError *err);
void rak_string_append_cstr(RakString *str, int len, const char *cstr, RakError *err);
void rak_string_clear(RakString *str);
bool rak_string_equals(RakString *str1, RakString *str2);
int rak_string_compare(RakString *str1, RakString *str2);
void rak_string_print(RakString *str);

#endif // RAK_STRING_H
