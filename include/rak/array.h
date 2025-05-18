//
// array.h
//
// Copyright 2025 Fábio de Souza Villaça Medeiros
//
// This file is part of the Rak Project.
// For detailed license information, please refer to the LICENSE file
// located in the root directory of this project.
//

#ifndef RAK_ARRAY_H
#define RAK_ARRAY_H

#include "slice.h"
#include "value.h"

#define rak_array_cap(a)      ((a)->slice.cap)
#define rak_array_len(a)      ((a)->slice.len)
#define rak_array_elements(a) ((a)->slice.data)
#define rak_array_is_empty(a) (!rak_array_len(a))
#define rak_array_get(a, i)   rak_slice_get(&(a)->slice, (i))

typedef struct
{
  RakObject          obj;
  RakSlice(RakValue) slice;
} RakArray;

void rak_array_init(RakArray *arr, RakError *err);
void rak_array_init_with_capacity(RakArray *arr, int cap, RakError *err);
void rak_array_init_from_values(RakArray *arr, int len, RakValue *values, RakError *err);
void rak_array_init_copy(RakArray *arr1, RakArray *arr2, RakError *err);
void rak_array_deinit(RakArray *arr);
RakArray *rak_array_new(RakError *err);
RakArray *rak_array_new_with_capacity(int cap, RakError *err);
RakArray *rak_array_new_from_values(int len, RakValue *values, RakError *err);
RakArray *rak_array_new_copy(RakArray *arr, RakError *err);
void rak_array_free(RakArray *arr);
void rak_array_release(RakArray *arr);
void rak_array_ensure_capacity(RakArray *arr, int cap, RakError *err);
RakArray *rak_array_append(RakArray *arr, RakValue val, RakError *err);
RakArray *rak_array_append_values(RakArray *arr, int len, RakValue *values, RakError *err);
RakArray *rak_array_set(RakArray *arr, int idx, RakValue val, RakError *err);
RakArray *rak_array_remove_at(RakArray *arr, int idx, RakError *err);
RakArray *rak_array_concat(RakArray *arr1, RakArray *arr2, RakError *err);
RakArray *rak_array_slice(RakArray *arr, int start, int end, RakError *err);
void rak_array_inplace_append(RakArray *arr, RakValue val, RakError *err);
void rak_array_inplace_append_values(RakArray *arr, int len, RakValue *values, RakError *err);
void rak_array_inplace_set(RakArray *arr, int idx, RakValue val);
void rak_array_inplace_remove_at(RakArray *arr, int idx);
void rak_array_inplace_concat(RakArray *arr1, RakArray *arr2, RakError *err);
void rak_array_inplace_slice(RakArray *arr, int start, int end, RakError *err);
void rak_array_inplace_clear(RakArray *arr);
bool rak_array_equals(RakArray *arr1, RakArray *arr2);
void rak_array_print(RakArray *arr);

#endif // RAK_ARRAY_H
