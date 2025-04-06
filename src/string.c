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
#include <stdio.h>
#include <string.h>

void rak_string_init(RakString *str, RakError *err)
{
  rak_object_init(&str->obj);
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

void rak_string_release(RakString *str)
{
  RakObject *obj = &str->obj;
  --obj->refCount;
  if (obj->refCount) return;
  rak_string_free(str);
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

bool rak_string_equals(RakString *str1, RakString *str2)
{
  if (str1 == str2) return true;
  int len = rak_string_len(str1);
  if (len != rak_string_len(str2)) return false;
  return !memcmp(rak_string_chars(str1), rak_string_chars(str1), len);
}

int rak_string_compare(RakString *str1, RakString *str2)
{
  if (str1 == str2) return 0;
  int len1 = rak_string_len(str1);
  int len2 = rak_string_len(str2);
  int cmp = memcmp(rak_string_chars(str1), rak_string_chars(str2), len1 < len2 ? len1 : len2);
  if (cmp) return cmp;
  return len1 - len2;
}

void rak_string_print(RakString *str)
{
  printf("%.*s", rak_string_len(str), rak_string_chars(str));
}
