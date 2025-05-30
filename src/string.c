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
#include "rak/error.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

static inline unsigned char hex2bin (char c);
static void handle_hex_escape(RakString *str, int len, const char * cstr, int *curr, RakError *err);
static void handle_escape_sequence(RakString *str, int len, const char * cstr, int *curr, RakError *err);
static void parse_escaped_string(RakString *str, int len, const char *cstr, RakError *err);

void rak_string_init(RakString *str, RakError *err)
{
  rak_object_init(&str->obj);
  rak_slice_init(&str->slice, err);
}

void rak_string_init_with_capacity(RakString *str, int cap, RakError *err)
{
  rak_object_init(&str->obj);
  rak_slice_init_with_capacity(&str->slice, cap, err);
}

void rak_string_init_from_cstr(RakString *str, int len, const char *cstr, RakError *err)
{
  if (len < 0) len = (int) strlen(cstr);
  rak_string_init_with_capacity(str, len, err);
  if (!rak_is_ok(err)) return;
  memcpy(rak_string_chars(str), cstr, len);
  str->slice.len = len;
}

void rak_string_init_copy(RakString *str1, RakString *str2, RakError *err)
{
  int len = rak_string_len(str2);
  rak_string_init_with_capacity(str1, len, err);
  if (!rak_is_ok(err)) return;
  memcpy(rak_string_chars(str1), rak_string_chars(str2), len);
  str1->slice.len = len;
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

RakString *rak_string_new_copy(RakString *str, RakError *err)
{
  int len = rak_string_len(str);
  RakString *_str = rak_string_new_with_capacity(len, err);
  if (!rak_is_ok(err)) return NULL;
  memcpy(rak_string_chars(_str), rak_string_chars(str), len);
  _str->slice.len = len;
  return _str;
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

RakString *rak_string_append_cstr(RakString *str, int len, const char *cstr, RakError *err)
{
  // TODO: Implement this function.
  (void) str;
  (void) len;
  (void) cstr;
  (void) err;
  return NULL;
}

RakString *rak_string_concat(RakString *str1, RakString *str2, RakError *err)
{
  // TODO: Implement this function.
  (void) str1;
  (void) str2;
  (void) err;
  return NULL;
}

RakString *rak_string_slice(RakString *str, int start, int end, RakError *err)
{
  if (start < 0 || end > rak_string_len(str))
  {
    rak_error_set(err, "string slice out of bounds");
    return NULL;
  }
  int len = start < end ? end - start : 0;
  RakString *_str = rak_string_new_with_capacity(len, err);
  if (!rak_is_ok(err)) return NULL;
  memcpy(rak_string_chars(_str), rak_string_chars(str) + start, len);
  _str->slice.len = len;
  return _str;
}

void rak_string_inplace_append_cstr(RakString *str, int len, const char *cstr, RakError *err)
{
  if (len < 0) len = (int) strlen(cstr);
  rak_string_ensure_capacity(str, rak_string_len(str) + len, err);
  if (!rak_is_ok(err)) return;
  memcpy(&str->slice.data[rak_string_len(str)], cstr, len);
  str->slice.len += len;
}

void rak_string_inplace_concat(RakString *str1, RakString *str2, RakError *err)
{
  if (rak_string_is_empty(str2)) return;
  int len1 = rak_string_len(str1);
  int len2 = rak_string_len(str2);
  int len = len1 + len2;
  rak_string_ensure_capacity(str1, len, err);
  if (!rak_is_ok(err)) return;
  memcpy(&str1->slice.data[len1], rak_string_chars(str2), len2);
  str1->slice.len = len;
}

void rak_string_inplace_slice(RakString *str, int start, int end, RakError *err)
{
  // TODO: Implement this function.
  (void) str;
  (void) start;
  (void) end;
  (void) err;
}

void rak_string_inplace_clear(RakString *str)
{
  rak_slice_clear(&str->slice);
}

bool rak_string_equals(RakString *str1, RakString *str2)
{
  if (str1 == str2) return true;
  int len = rak_string_len(str1);
  if (len != rak_string_len(str2)) return false;
  return !memcmp(rak_string_chars(str1), rak_string_chars(str2), len);
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
  fwrite(rak_string_chars(str), 1, rak_string_len(str), stdout);
}

void rak_string_init_from_cstr_with_escapes(RakString *str, int len, const char *cstr, RakError *err)
{
  if (len < 0) len = (int) strlen(cstr);
  rak_string_init_with_capacity(str, len, err);
  if (!rak_is_ok(err)) return;
  parse_escaped_string(str, len, cstr, err);
  if (rak_is_ok(err)) return;
  rak_string_deinit(str);
}

RakString *rak_string_new_from_cstr_with_escapes(int len, const char *cstr, RakError *err)
{
  RakString *str = rak_memory_alloc(sizeof(*str), err);
  if (!rak_is_ok(err)) return NULL;
  rak_string_init_from_cstr_with_escapes(str, len, cstr, err);
  if (rak_is_ok(err)) return str;
  rak_memory_free(str);
  return NULL;
}

static inline unsigned char hex2bin (char c)
{
  if (c >= '0' && c <= '9')
    return c - '0' ;
  if (c >= 'A' && c <= 'F')
    return c - 'A' + 10 ;
  if (c >= 'a' && c <= 'f')
    return c - 'a' + 10 ;
  return 0;
}

static void handle_hex_escape(RakString *str, int len, const char *cstr, int *curr, RakError *err)
{
  if ((*curr) + 1 >= len || !isxdigit(cstr[*curr]) || !isxdigit(cstr[(*curr) + 1]))
  {
    rak_error_set(err,"expecting two hexadecimal numbers for escape '\\x'");
    return;
  }  
  unsigned char code = hex2bin(cstr[*curr]) * 16 + hex2bin(cstr[(*curr) + 1]);
  if (code > 0x7f)
  {
    rak_error_set(err, "escape '\\x' is limited to ascii chars (below 0x7f)");
    return;
  }
  rak_string_inplace_append_cstr(str, 1, (char *) &code, err);
  *curr += 1;
  return;
}

static void handle_escape_sequence(RakString *str, int len, const char *cstr, int *curr, RakError *err)
{
  switch (cstr[*curr]) {
  case 'n':
    rak_string_inplace_append_cstr(str, 1, "\n", err);
    break;
  case 'r':
    rak_string_inplace_append_cstr(str, 1, "\r", err);
    break;
  case 't':
    rak_string_inplace_append_cstr(str, 1, "\t", err);
    break;
  case '"':
    rak_string_inplace_append_cstr(str, 1, "\"", err);
    break;
  case '\\':
    rak_string_inplace_append_cstr(str, 1, "\\", err);
    break;
  case '0':
    rak_string_inplace_append_cstr(str, 1, "\0", err);
    break;
  case 'x':
    ++(*curr);
    handle_hex_escape(str, len, cstr, curr, err);
    return;
  default:
    rak_error_set(
      err,
      "unknown escape sequence '\\%c'",
      isprint(cstr[*curr]) ? cstr[*curr] : '?'
    );
    return;
  }
  return;
}

static void parse_escaped_string(RakString *str, int len, const char *cstr, RakError *err)
{
  for (int curr = 0; curr < len; curr++)
  {
    if (cstr[curr] == '\\')
    {
      curr++;
      handle_escape_sequence(str, len, cstr, &curr, err);
      if (!rak_is_ok(err)) return;
      continue;
    }
    rak_string_inplace_append_cstr(str, 1, &cstr[curr], err);
    if (!rak_is_ok(err)) return;
  }
}
