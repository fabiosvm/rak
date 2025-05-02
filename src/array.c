//
// array.c
//
// Copyright 2025 Fábio de Souza Villaça Medeiros
//
// This file is part of the Rak Project.
// For detailed license information, please refer to the LICENSE file
// located in the root directory of this project.
//

#include "rak/array.h"
#include <stdio.h>

static inline void release_elements(RakArray *arr);

static inline void release_elements(RakArray *arr)
{
  int len = rak_array_len(arr);
  for (int i = 0; i < len; ++i)
  {
    RakValue val = rak_slice_get(&arr->slice, i);
    rak_value_release(val);
  }
}

void rak_array_init(RakArray *arr, RakError *err)
{
  rak_object_init(&arr->obj);
  rak_slice_init(&arr->slice, err);
}

void rak_array_init_with_capacity(RakArray *arr, int cap, RakError *err)
{
  rak_object_init(&arr->obj);
  rak_slice_init_with_capacity(&arr->slice, cap, err);
}

void rak_array_init_copy(RakArray *arr1, RakArray *arr2, RakError *err)
{
  int len = rak_array_len(arr2);
  rak_array_init_with_capacity(arr1, len, err);
  if (!rak_is_ok(err)) return;
  for (int i = 0; i < len; ++i)
  {
    RakValue val = rak_array_get(arr2, i);
    rak_slice_set(&arr1->slice, i, val);
    rak_value_retain(val);
  }
  arr1->slice.len = len;
}

void rak_array_deinit(RakArray *arr)
{
  release_elements(arr);
  rak_slice_deinit(&arr->slice);
}

RakArray *rak_array_new(RakError *err)
{
  RakArray *arr = rak_memory_alloc(sizeof(*arr), err);
  if (!rak_is_ok(err)) return NULL;
  rak_array_init(arr, err);
  if (rak_is_ok(err)) return arr;
  rak_memory_free(arr);
  return NULL;
}

RakArray *rak_array_new_with_capacity(int cap, RakError *err)
{
  RakArray *arr = rak_memory_alloc(sizeof(*arr), err);
  if (!rak_is_ok(err)) return NULL;
  rak_array_init_with_capacity(arr, cap, err);
  if (rak_is_ok(err)) return arr;
  rak_memory_free(arr);
  return NULL;
}

RakArray *rak_array_new_copy(RakArray *arr, RakError *err)
{
  RakArray *_arr = rak_memory_alloc(sizeof(*_arr), err);
  if (!rak_is_ok(err)) return NULL;
  rak_array_init_copy(_arr, arr, err);
  if (rak_is_ok(err)) return _arr;
  rak_memory_free(_arr);
  return NULL;
}

void rak_array_free(RakArray *arr)
{
  rak_array_deinit(arr);
  rak_memory_free(arr);
}

void rak_array_release(RakArray *arr)
{
  RakObject *obj = &arr->obj;
  --obj->refCount;
  if (obj->refCount) return;
  rak_array_free(arr);
}

void rak_array_ensure_capacity(RakArray *arr, int cap, RakError *err)
{
  rak_slice_ensure_capacity(&arr->slice, cap, err);
}

RakArray *rak_array_append(RakArray *arr, RakValue val, RakError *err)
{
  int len = rak_array_len(arr);
  int _len = len + 1;
  RakArray *_arr = rak_array_new_with_capacity(_len, err);
  if (!rak_is_ok(err)) return NULL;
  for (int i = 0; i < len; ++i)
  {
    RakValue _val = rak_array_get(arr, i);
    rak_slice_set(&_arr->slice, i, _val);
    rak_value_retain(_val);
  }
  rak_slice_set(&_arr->slice, len, val);
  rak_value_retain(val);
  _arr->slice.len = _len;
  return _arr;
}

RakArray *rak_array_set(RakArray *arr, int idx, RakValue val, RakError *err)
{
  int len = rak_array_len(arr);
  RakArray *_arr = rak_array_new_with_capacity(len, err);
  if (!rak_is_ok(err)) return NULL;
  for (int i = 0; i < idx; ++i)
  {
    RakValue _val = rak_array_get(arr, i);
    rak_slice_set(&_arr->slice, i, _val);
    rak_value_retain(_val);
  }
  rak_slice_set(&_arr->slice, idx, val);
  rak_value_retain(val);
  for (int i = idx + 1; i < len; ++i)
  {
    RakValue _val = rak_array_get(arr, i);
    rak_slice_set(&_arr->slice, i, _val);
    rak_value_retain(_val);
  }
  _arr->slice.len = len;
  return _arr;
}

RakArray *rak_array_remove_at(RakArray *arr, int idx, RakError *err)
{
  int len = rak_array_len(arr);
  int _len = len - 1;
  RakArray *_arr = rak_array_new_with_capacity(_len, err);
  if (!rak_is_ok(err)) return NULL;
  for (int i = 0; i < idx; ++i)
  {
    RakValue val = rak_array_get(arr, i);
    rak_slice_set(&_arr->slice, i, val);
    rak_value_retain(val);
  }
  for (int i = idx + 1; i < len; ++i)
  {
    RakValue val = rak_array_get(arr, i);
    rak_slice_set(&_arr->slice, i, val);
    rak_value_retain(val);
  }
  _arr->slice.len = _len;
  return _arr;
}

RakArray *rak_array_concat(RakArray *arr1, RakArray *arr2, RakError *err)
{
  int len1 = rak_array_len(arr1);
  int len2 = rak_array_len(arr2);
  int len = len1 + len2;
  RakArray *arr = rak_array_new_with_capacity(len, err);
  if (!rak_is_ok(err)) return NULL;
  int j = 0;
  for (int i = 0; i < len1; ++i, ++j)
  {
    RakValue val = rak_array_get(arr, i);
    rak_slice_set(&arr->slice, j, val);
    rak_value_retain(val);
  }
  for (int i = 0; i < len2; ++i, ++j)
  {
    RakValue val = rak_array_get(arr, i);
    rak_slice_set(&arr->slice, j, val);
    rak_value_retain(val);
  }
  arr->slice.len = len;
  return arr;
}

RakArray *rak_array_slice(RakArray *arr, int start, int end, RakError *err)
{
  if (start < 0 || end > rak_array_len(arr))
  {
    rak_error_set(err, "array slice out of bounds");
    return NULL;
  }
  int len = start < end ? end - start : 0;
  RakArray *_arr = rak_array_new_with_capacity(len, err);
  if (!rak_is_ok(err)) return NULL;
  for (int i = start; i < end; ++i)
  {
    RakValue val = rak_array_get(arr, i);
    rak_slice_set(&_arr->slice, i - start, val);
    rak_value_retain(val);
  }
  _arr->slice.len = len;
  return _arr;
}

void rak_array_inplace_append(RakArray *arr, RakValue val, RakError *err)
{
  rak_slice_ensure_append(&arr->slice, val, err);
  if (!rak_is_ok(err)) return;
  rak_value_retain(val);
}

void rak_array_inplace_set(RakArray *arr, int idx, RakValue val)
{
  rak_value_retain(val);
  rak_value_release(rak_array_get(arr, idx));
  rak_slice_set(&arr->slice, idx, val);
}

void rak_array_inplace_remove_at(RakArray *arr, int idx)
{
  RakValue val = rak_array_get(arr, idx);
  rak_slice_remove_at(&arr->slice, idx);
  rak_value_release(val);
}

void rak_array_inplace_concat(RakArray *arr1, RakArray *arr2, RakError *err)
{
  if (rak_array_is_empty(arr2)) return;
  int len1 = rak_array_len(arr1);
  int len2 = rak_array_len(arr2);
  int len = len1 + len2;
  rak_array_ensure_capacity(arr1, len, err);
  if (!rak_is_ok(err)) return;
  for (int i = 0; i < len2; ++i)
  {
    RakValue val = rak_array_get(arr2, i);
    rak_slice_set(&arr1->slice, len1 + i, val);
    rak_value_retain(val);
  }
  arr1->slice.len = len;
}

void rak_array_inplace_slice(RakArray *arr, int start, int end, RakError *err)
{
  // TODO: Implement this function.
  (void) arr;
  (void) start;
  (void) end;
  (void) err;
}

void rak_array_inplace_clear(RakArray *arr)
{
  release_elements(arr);
  rak_slice_clear(&arr->slice);
}

bool rak_array_equals(RakArray *arr1, RakArray *arr2)
{
  if (arr1 == arr2) return true;
  int len = rak_array_len(arr1);
  if (len != rak_array_len(arr2)) return false;
  for (int i = 0; i < len; ++i)
  {
    RakValue val1 = rak_array_get(arr1, i);
    RakValue val2 = rak_array_get(arr2, i);
    if (!rak_value_equals(val1, val2)) return false;
  }
  return true;
}

void rak_array_print(RakArray *arr)
{
  printf("[");
  int len = rak_array_len(arr);
  for (int i = 0; i < len; ++i)
  {
    if (i > 0) printf(", ");
    RakValue val = rak_array_get(arr, i);
    rak_value_print(val);
  }
  printf("]");
}
