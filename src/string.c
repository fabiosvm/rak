//
// string.c
//
// Copyright 2025 Fábio de Souza Villaça Medeiros
//
// This file is part of the Rak Project.
// For detailed license information, please refer to the LICENSE file
// located in the root directory of this project.
//

#include "rak/string.h"
#include <string.h>

void rak_string_init(RakString *str, RakError *err)
{
  rak_slice_init(&str->slice, err);
}

void rak_string_init_with_capacity(RakString *str, int cap, RakError *err)
{
  rak_slice_init_with_capacity(&str->slice, cap, err);
}

void rak_string_init_from_cstr(RakString *str, int len, const char *cstr, RakError *err)
{
  if (len < 0) len = (int) strlen(cstr);
  rak_string_init_with_capacity(str, len, err);
  if (!rak_is_ok(err)) return;
  memcpy(str->slice.data, cstr, len);
  str->slice.len = len;
}

void rak_string_deinit(RakString *str)
{
  rak_slice_deinit(&str->slice);
}

RakString *rak_string_new(RakError *err)
{
  RakString *str = rak_memory_alloc(sizeof(*str), err);
  if (!rak_is_ok(err)) return NULL;
  rak_string_init(str, err);
  if (rak_is_ok(err)) return str;
  rak_memory_free(str);
  return NULL;
}

RakString *rak_string_new_with_capacity(int cap, RakError *err)
{
  RakString *str = rak_memory_alloc(sizeof(*str), err);
  if (!rak_is_ok(err)) return NULL;
  rak_string_init_with_capacity(str, cap, err);
  if (rak_is_ok(err)) return str;
  rak_memory_free(str);
  return NULL;
}

RakString *rak_string_new_from_cstr(int len, const char *cstr, RakError *err)
{
  RakString *str = rak_memory_alloc(sizeof(*str), err);
  if (!rak_is_ok(err)) return NULL;
  rak_string_init_from_cstr(str, len, cstr, err);
  if (rak_is_ok(err)) return str;
  rak_memory_free(str);
  return NULL;
}

void rak_string_free(RakString *str)
{
  rak_string_deinit(str);
  rak_memory_free(str);
}

void rak_string_ensure_capacity(RakString *str, int cap, RakError *err)
{
  rak_slice_ensure_capacity(&str->slice, cap, err);
}

void rak_string_append_cstr(RakString *str, int len, const char *cstr, RakError *err)
{
  if (len < 0) len = (int) strlen(cstr);
  rak_string_ensure_capacity(str, str->slice.len + len, err);
  if (!rak_is_ok(err)) return;
  void *dest = &str->slice.data[str->slice.len];
  memcpy(dest, cstr, len);
  str->slice.len += len;
}

void rak_string_clear(RakString *str)
{
  rak_slice_clear(&str->slice);
}
