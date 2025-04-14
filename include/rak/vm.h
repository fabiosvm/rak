//
// vm.h
//
// Copyright 2025 Fábio de Souza Villaça Medeiros
//
// This file is part of the Rak Project.
// For detailed license information, please refer to the LICENSE file
// located in the root directory of this project.
//

#ifndef RAK_VM_H
#define RAK_VM_H

#include <stdio.h>
#include <math.h>
#include "array.h"
#include "chunk.h"
#include "stack.h"
#include "string.h"

#define RAK_VM_VSTK_DEFAULT_SIZE (1024)

typedef struct
{
  RakStack(RakValue) vstk;
} RakVM;

static inline void rak_vm_push(RakVM *vm, RakValue val, RakError *err);
static inline void rak_vm_push_nil(RakVM *vm, RakError *err);
static inline void rak_vm_push_bool(RakVM *vm, bool b, RakError *err);
static inline void rak_vm_push_number(RakVM *vm, double data, RakError *err);
static inline void rak_vm_push_value(RakVM *vm, RakValue val, RakError *err);
static inline void rak_vm_push_object(RakVM *vm, RakValue val, RakError *err);
static inline void rak_vm_load_const(RakVM *vm, RakChunk *chunk, uint8_t idx, RakError *err);
static inline void rak_vm_load_local(RakVM *vm, uint8_t idx, RakError *err);
static inline void rak_vm_load_element(RakVM *vm, RakError *err);
static inline void rak_vm_new_array(RakVM *vm, uint8_t len, RakError *err);
static inline void rak_vm_pop(RakVM *vm);
static inline RakValue rak_vm_get(RakVM *vm, uint8_t idx);
static inline void rak_vm_set(RakVM *vm, uint8_t idx, RakValue val);
static inline void rak_vm_set_value(RakVM *vm, uint8_t idx, RakValue val);
static inline void rak_vm_set_object(RakVM *vm, uint8_t idx, RakValue val);
static inline void rak_vm_eq(RakVM *vm);
static inline void rak_vm_gt(RakVM *vm, RakError *err);
static inline void rak_vm_lt(RakVM *vm, RakError *err);
static inline void rak_vm_add(RakVM *vm, RakError *err);
static inline void rak_vm_sub(RakVM *vm, RakError *err);
static inline void rak_vm_mul(RakVM *vm, RakError *err);
static inline void rak_vm_div(RakVM *vm, RakError *err);
static inline void rak_vm_mod(RakVM *vm, RakError *err);
static inline void rak_vm_not(RakVM *vm);
static inline void rak_vm_neg(RakVM *vm, RakError *err);
static inline void rak_vm_echo(RakVM *vm);

void rak_vm_init(RakVM *vm, int vstkSize, RakError *err);
void rak_vm_deinit(RakVM *vm);
void rak_vm_run(RakVM *vm, RakChunk *chunk, RakError *err);

static inline void rak_vm_push(RakVM *vm, RakValue val, RakError *err)
{
  if (rak_stack_is_full(&vm->vstk))
  {
    rak_error_set(err, "stack overflow");
    return;
  }
  rak_stack_push(&vm->vstk, val);
}

static inline void rak_vm_push_nil(RakVM *vm, RakError *err)
{
  rak_vm_push(vm, rak_nil_value(), err);
}

static inline void rak_vm_push_bool(RakVM *vm, bool data, RakError *err)
{
  rak_vm_push(vm, rak_bool_value(data), err);
}

static inline void rak_vm_push_number(RakVM *vm, double data, RakError *err)
{
  rak_vm_push(vm, rak_number_value(data), err);
}

static inline void rak_vm_push_value(RakVM *vm, RakValue val, RakError *err)
{
  rak_vm_push(vm, val, err);
  if (!rak_is_ok(err)) return;
  rak_value_retain(val);
}

static inline void rak_vm_push_object(RakVM *vm, RakValue val, RakError *err)
{
  rak_vm_push(vm, val, err);
  if (!rak_is_ok(err)) return;
  rak_object_retain(rak_as_object(val));
}

static inline void rak_vm_load_const(RakVM *vm, RakChunk *chunk, uint8_t idx, RakError *err)
{
  RakValue val = rak_slice_get(&chunk->consts, idx);
  rak_vm_push_value(vm, val, err);
}

static inline void rak_vm_load_local(RakVM *vm, uint8_t idx, RakError *err)
{
  RakValue val = vm->vstk.base[idx];
  rak_vm_push_value(vm, val, err);
}

static inline void rak_vm_load_element(RakVM *vm, RakError *err)
{
  RakValue val1 = rak_vm_get(vm, 1);
  RakValue val2 = rak_vm_get(vm, 0);
  if (rak_is_array(val1))
  {
    if (!rak_is_number(val2) || !rak_is_integer(val2))
    {
      rak_error_set(err, "cannot index array with non-integer value");
      return;
    }
    RakArray *arr = rak_as_array(val1);
    int64_t idx = rak_as_integer(val2);
    if (idx < 0 || idx >= rak_array_len(arr))
    {
      rak_error_set(err, "array index out of range");
      return;
    }
    RakValue res = rak_array_get(arr, (int) idx);
    rak_vm_set_value(vm, 1, res);
    rak_vm_pop(vm);
    return;
  }
  rak_error_set(err, "cannot index %s", rak_type_to_cstr(val1.type));
}

static inline void rak_vm_new_array(RakVM *vm, uint8_t len, RakError *err)
{
  int n = len - 1;
  RakValue *slots = &rak_stack_get(&vm->vstk, n);
  RakArray *arr = rak_array_new_with_capacity(len, err);
  if (!rak_is_ok(err)) return;
  for (int i = 0; i < len; ++i)
    rak_slice_set(&arr->slice, i, slots[i]);
  arr->slice.len = len;
  slots[0] = rak_array_value(arr);
  rak_object_retain(&arr->obj);
  vm->vstk.top -= n;
}

static inline void rak_vm_pop(RakVM *vm)
{
  RakValue val = rak_vm_get(vm, 0);
  rak_stack_pop(&vm->vstk);
  rak_value_release(val);
}

static inline RakValue rak_vm_get(RakVM *vm, uint8_t idx)
{
  return rak_stack_get(&vm->vstk, idx);
}

static inline void rak_vm_set(RakVM *vm, uint8_t idx, RakValue val)
{
  RakValue _val = rak_stack_get(&vm->vstk, idx);
  rak_value_release(_val);
  rak_stack_set(&vm->vstk, idx, val);
}

static inline void rak_vm_set_value(RakVM *vm, uint8_t idx, RakValue val)
{
  rak_value_retain(val);
  rak_vm_set(vm, idx, val);
}

static inline void rak_vm_set_object(RakVM *vm, uint8_t idx, RakValue val)
{
  rak_object_retain(rak_as_object(val));
  rak_vm_set(vm, idx, val);
}

static inline void rak_vm_eq(RakVM *vm)
{
  RakValue val1 = rak_vm_get(vm, 1);
  RakValue val2 = rak_vm_get(vm, 0);
  RakValue res = rak_bool_value(rak_value_equals(val1, val2));
  rak_vm_set(vm, 1, res);
  rak_vm_pop(vm);
}

static inline void rak_vm_gt(RakVM *vm, RakError *err)
{
  RakValue val1 = rak_vm_get(vm, 1);
  RakValue val2 = rak_vm_get(vm, 0);
  int cmp = rak_value_compare(val1, val2, err);
  if (!rak_is_ok(err)) return;
  RakValue res = rak_bool_value(cmp > 0);
  rak_vm_set(vm, 1, res);
  rak_vm_pop(vm);
}

static inline void rak_vm_lt(RakVM *vm, RakError *err)
{
  RakValue val1 = rak_vm_get(vm, 1);
  RakValue val2 = rak_vm_get(vm, 0);
  int cmp = rak_value_compare(val1, val2, err);
  if (!rak_is_ok(err)) return;
  RakValue res = rak_bool_value(cmp < 0);
  rak_vm_set(vm, 1, res);
  rak_vm_pop(vm);
}

static inline void rak_vm_add(RakVM *vm, RakError *err)
{
  RakValue val1 = rak_vm_get(vm, 1);
  RakValue val2 = rak_vm_get(vm, 0);
  if (rak_is_number(val1))
  {
    if (!rak_is_number(val2))
    {
      rak_error_set(err, "cannot add number and %s", rak_type_to_cstr(val2.type));
      return;
    }
    double num1 = rak_as_number(val1);
    double num2 = rak_as_number(val2);
    RakValue res = rak_number_value(num1 + num2);
    rak_vm_set(vm, 1, res);
    rak_vm_pop(vm);
    return;
  }
  if (rak_is_string(val1))
  {
    if (!rak_is_string(val2))
    {
      rak_error_set(err, "cannot add string and %s", rak_type_to_cstr(val2.type));
      return;
    }
    RakString *str1 = rak_as_string(val1);
    RakString *str2 = rak_as_string(val2);
    RakString *str3 = rak_string_new_copy(str1, err);
    if (!rak_is_ok(err)) return;
    rak_string_inplace_concat(str3, str2, err);
    if (!rak_is_ok(err))
    {
      rak_string_free(str3);
      return;
    }
    RakValue res = rak_string_value(str3);
    rak_vm_set_object(vm, 1, res);
    rak_vm_pop(vm);
    return;
  }
  if (rak_is_array(val1))
  {
    if (!rak_is_array(val2))
    {
      rak_error_set(err, "cannot add array and %s", rak_type_to_cstr(val2.type));
      return;
    }
    RakArray *arr1 = rak_as_array(val1);
    RakArray *arr2 = rak_as_array(val2);
    RakArray *arr3 = rak_array_new_copy(arr1, err);
    if (!rak_is_ok(err)) return;
    rak_array_inplace_concat(arr3, arr2, err);
    if (!rak_is_ok(err))
    {
      rak_array_free(arr3);
      return;
    }
    RakValue res = rak_array_value(arr3);
    rak_vm_set_object(vm, 1, res);
    rak_vm_pop(vm);
    return;
  }
  rak_error_set(err, "cannot add %s and %s", rak_type_to_cstr(val1.type),
    rak_type_to_cstr(val2.type));
}

static inline void rak_vm_sub(RakVM *vm, RakError *err)
{
  RakValue val1 = rak_vm_get(vm, 1);
  RakValue val2 = rak_vm_get(vm, 0);
  if (!rak_is_number(val1) || !rak_is_number(val2))
  {
    rak_error_set(err, "cannot subtract non-number values");
    return;
  }
  double num1 = rak_as_number(val1);
  double num2 = rak_as_number(val2);
  RakValue res = rak_number_value(num1 - num2);
  rak_vm_set(vm, 1, res);
  rak_vm_pop(vm);
}

static inline void rak_vm_mul(RakVM *vm, RakError *err)
{
  RakValue val1 = rak_vm_get(vm, 1);
  RakValue val2 = rak_vm_get(vm, 0);
  if (!rak_is_number(val1) || !rak_is_number(val2))
  {
    rak_error_set(err, "cannot multiply non-number values");
    return;
  }
  double num1 = rak_as_number(val1);
  double num2 = rak_as_number(val2);
  RakValue res = rak_number_value(num1 * num2);
  rak_vm_set(vm, 1, res);
  rak_vm_pop(vm);
}

static inline void rak_vm_div(RakVM *vm, RakError *err)
{
  RakValue val1 = rak_vm_get(vm, 1);
  RakValue val2 = rak_vm_get(vm, 0);
  if (!rak_is_number(val1) || !rak_is_number(val2))
  {
    rak_error_set(err, "cannot divide non-number values");
    return;
  }
  double num1 = rak_as_number(val1);
  double num2 = rak_as_number(val2);
  RakValue res = rak_number_value(num1 / num2);
  rak_vm_set(vm, 1, res);
  rak_vm_pop(vm);
}

static inline void rak_vm_mod(RakVM *vm, RakError *err)
{
  RakValue val1 = rak_vm_get(vm, 1);
  RakValue val2 = rak_vm_get(vm, 0);
  if (!rak_is_number(val1) || !rak_is_number(val2))
  {
    rak_error_set(err, "cannot calculate modulo of non-number values");
    return;
  }
  double num1 = rak_as_number(val1);
  double num2 = rak_as_number(val2);
  RakValue res = rak_number_value(fmod(num1, num2));
  rak_vm_set(vm, 1, res);
  rak_vm_pop(vm);
}

static inline void rak_vm_not(RakVM *vm)
{
  RakValue val = rak_vm_get(vm, 0);
  RakValue res = rak_is_falsy(val) ? rak_bool_value(true) : rak_bool_value(false);
  rak_vm_set(vm, 0, res);
}

static inline void rak_vm_neg(RakVM *vm, RakError *err)
{
  RakValue val = rak_vm_get(vm, 0);
  if (!rak_is_number(val))
  {
    rak_error_set(err, "cannot negate a non-number value");
    return;
  }
  double num = rak_as_number(val);
  RakValue res = rak_number_value(-num);
  rak_vm_set(vm, 0, res);
}

static inline void rak_vm_echo(RakVM *vm)
{
  RakValue val = rak_vm_get(vm, 0);
  rak_value_print(val);
  printf("\n");
  rak_vm_pop(vm);
}

#endif // RAK_VM_H
