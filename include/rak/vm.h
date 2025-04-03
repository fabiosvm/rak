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

#include <math.h>
#include "chunk.h"
#include "stack.h"

#define RAK_VM_VSTK_DEFAULT_SIZE (1024)

typedef struct
{
  RakStack(RakValue) vstk;
} RakVM;

static inline void rak_vm_push(RakVM *vm, RakValue val, RakError *err);
static inline void rak_vm_push_nil(RakVM *vm, RakError *err);
static inline void rak_vm_push_bool(RakVM *vm, bool b, RakError *err);
static inline void rak_vm_push_number(RakVM *vm, double num, RakError *err);
static inline void rak_vm_load_const(RakVM *vm, RakChunk *chunk, uint8_t idx, RakError *err);
static inline void rak_vm_pop(RakVM *vm);
static inline RakValue rak_vm_get(RakVM *vm, int idx);
static inline void rak_vm_set(RakVM *vm, int idx, RakValue val);
static inline void rak_vm_eq(RakVM *vm, RakError *err);
static inline void rak_vm_gt(RakVM *vm, RakError *err);
static inline void rak_vm_lt(RakVM *vm, RakError *err);
static inline void rak_vm_add(RakVM *vm, RakError *err);
static inline void rak_vm_sub(RakVM *vm, RakError *err);
static inline void rak_vm_mul(RakVM *vm, RakError *err);
static inline void rak_vm_div(RakVM *vm, RakError *err);
static inline void rak_vm_mod(RakVM *vm, RakError *err);
static inline void rak_vm_not(RakVM *vm, RakError *err);
static inline void rak_vm_neg(RakVM *vm, RakError *err);

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

static inline void rak_vm_load_const(RakVM *vm, RakChunk *chunk, uint8_t idx, RakError *err)
{
  RakValue val = rak_slice_get(&chunk->consts, idx);
  rak_vm_push(vm, val, err);
}

static inline void rak_vm_pop(RakVM *vm)
{
  rak_stack_pop(&vm->vstk);
}

static inline RakValue rak_vm_get(RakVM *vm, int idx)
{
  return rak_stack_get(&vm->vstk, idx);
}

static inline void rak_vm_set(RakVM *vm, int idx, RakValue val)
{
  rak_stack_set(&vm->vstk, idx, val);
}

static inline void rak_vm_eq(RakVM *vm, RakError *err)
{
  (void) err;
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
  if (!rak_is_number(val1) || !rak_is_number(val2))
  {
    rak_error_set(err, "cannot add non-number values");
    return;
  }
  double num1 = rak_as_number(val1);
  double num2 = rak_as_number(val2);
  RakValue res = rak_number_value(num1 + num2);
  rak_vm_set(vm, 1, res);
  rak_vm_pop(vm);
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

static inline void rak_vm_not(RakVM *vm, RakError *err)
{
  (void) err;
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

#endif // RAK_VM_H
