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

void rak_array_init(RakArray *arr, RakError *err)
{
  rak_object_init(&arr->obj);
  rak_slice_init(&arr->slice, err);
}

void rak_array_init_with_capacity(RakArray *arr, int cap, RakError *err)
{
  rak_slice_init_with_capacity(&arr->slice, cap, err);
}

void rak_array_deinit(RakArray *arr)
{
  int len = arr->slice.len;
  for (int i = 0; i < len; ++i)
  {
    RakValue val = rak_slice_get(&arr->slice, i);
    rak_value_release(val);
  }
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

void rak_array_clear(RakArray *arr)
{
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
    RakValue val = rak_array_get(arr, i);
    rak_value_print(val);
    if (i < len - 1) printf(", ");
  }
  printf("]");
}
