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
#include "fiber.h"
#include "function.h"
#include "range.h"
#include "record.h"

static inline void rak_vm_push_nil(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static inline void rak_vm_push_false(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static inline void rak_vm_push_true(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static inline void rak_vm_push_int(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static inline void rak_vm_load_const(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static inline void rak_vm_load_global(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static inline void rak_vm_load_local(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static inline void rak_vm_store_local(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static inline void rak_vm_fetch_local(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static inline void rak_vm_ref_local(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static inline void rak_vm_load_local_ref(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static inline void rak_vm_store_local_ref(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static inline void rak_vm_new_array(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static inline void rak_vm_new_range(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static inline void rak_vm_new_record(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static inline void rak_vm_new_closure(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static inline void rak_vm_move(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static inline void rak_vm_pop(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static inline void rak_vm_get_element(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static inline void rak_vm_set_element(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static inline void rak_vm_load_element(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static inline void rak_vm_fetch_element(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static inline void rak_vm_update_element(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static inline void rak_vm_get_field(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static inline void rak_vm_put_field(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static inline void rak_vm_load_field(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static inline void rak_vm_fetch_field(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static inline void rak_vm_update_field(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static inline void rak_vm_unpack_elements(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static inline void rak_vm_unpack_fields(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static inline void rak_vm_jump(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static inline void rak_vm_jump_if_false(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static inline void rak_vm_jump_if_true(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static inline void rak_vm_eq(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static inline void rak_vm_gt(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static inline void rak_vm_ge(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static inline void rak_vm_lt(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static inline void rak_vm_le(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static inline void rak_vm_add(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static inline void rak_vm_add2(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static inline void rak_vm_add3(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static inline void rak_vm_sub(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static inline void rak_vm_sub2(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static inline void rak_vm_sub3(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static inline void rak_vm_mul(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static inline void rak_vm_mul2(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static inline void rak_vm_mul3(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static inline void rak_vm_div(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static inline void rak_vm_div2(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static inline void rak_vm_div3(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static inline void rak_vm_mod(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static inline void rak_vm_mod2(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static inline void rak_vm_mod3(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static inline void rak_vm_not(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static inline void rak_vm_neg(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static inline void rak_vm_call(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static inline void rak_vm_tail_call(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static inline void rak_vm_yield(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static inline void rak_vm_return(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static inline void rak_vm_return_nil(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);

void rak_vm_dispatch(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);

static inline void rak_vm_push_nil(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  (void) cl;
  (void) ip;
  (void) slots;
  rak_fiber_push_nil(fiber, err);
}

static inline void rak_vm_push_false(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  (void) cl;
  (void) ip;
  (void) slots;
  rak_fiber_push_bool(fiber, false, err);
}

static inline void rak_vm_push_true(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  (void) cl;
  (void) ip;
  (void) slots;
  rak_fiber_push_bool(fiber, true, err);
}

static inline void rak_vm_push_int(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  (void) cl;
  (void) slots;
  uint16_t data = rak_instr_ab(*ip);
  rak_fiber_push_number(fiber, data, err);
}

static inline void rak_vm_load_const(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  (void) slots;
  uint8_t idx = rak_instr_a(*ip);
  RakChunk *chunk = &((RakFunction *) cl->callable)->chunk;
  RakValue val = rak_slice_get(&chunk->consts, idx);
  rak_fiber_push_value(fiber, val, err);
}

static inline void rak_vm_load_global(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  (void) cl;
  (void) slots;
  uint8_t idx = rak_instr_a(*ip);
  RakValue val = rak_array_get(fiber->globals, idx);
  rak_fiber_push_value(fiber, val, err);
}

static inline void rak_vm_load_local(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  (void) cl;
  uint8_t idx = rak_instr_a(*ip);
  RakValue val = slots[idx];
  rak_fiber_push_value(fiber, val, err);
}

static inline void rak_vm_store_local(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  (void) cl;
  (void) err;
  uint8_t idx = rak_instr_a(*ip);
  RakValue val = rak_fiber_get(fiber, 0);
  rak_value_release(slots[idx]);
  slots[idx] = val;
  rak_stack_pop(&fiber->vstk);
}

static inline void rak_vm_fetch_local(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  (void) cl;
  uint8_t idx = rak_instr_a(*ip);
  RakValue val = slots[idx];
  if (rak_is_object(val) && rak_as_object(val)->refCount > 1)
    val.flags |= RAK_FLAG_SHARED;
  rak_fiber_push_value(fiber, val, err);
}

static inline void rak_vm_ref_local(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  (void) cl;
  uint8_t idx = rak_instr_a(*ip);
  RakValue *slot = &slots[idx];
  RakValue val = *slot;
  if (rak_is_object(val) && rak_as_object(val)->refCount > 1)
    slot->flags |= RAK_FLAG_SHARED;
  rak_fiber_push_value(fiber, rak_ref_value(slot), err);
}

static inline void rak_vm_load_local_ref(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  (void) cl;
  uint8_t idx = rak_instr_a(*ip);
  RakValue *slot = rak_as_ref(slots[idx]);
  rak_fiber_push_value(fiber, *slot, err);
}

static inline void rak_vm_store_local_ref(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  (void) cl;
  (void) err;
  uint8_t idx = rak_instr_a(*ip);
  RakValue *slot = rak_as_ref(slots[idx]);
  RakValue val = rak_fiber_get(fiber, 0);
  rak_value_release(*slot);
  *slot = val;
  rak_stack_pop(&fiber->vstk);
}

static inline void rak_vm_new_array(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  (void) cl;
  (void) slots;
  uint8_t len = rak_instr_a(*ip);
  RakArray *arr = rak_array_new_with_capacity(len, err);
  if (!len)
  {
    RakValue res = rak_array_value(arr);
    rak_fiber_push_object(fiber, res, err);
    return;
  }
  int n = len - 1;
  RakValue *_slots = &rak_stack_get(&fiber->vstk, n);
  if (!rak_is_ok(err)) return;
  for (int i = 0; i < len; ++i)
    rak_slice_set(&arr->slice, i, _slots[i]);
  arr->slice.len = len;
  _slots[0] = rak_array_value(arr);
  rak_object_retain(&arr->obj);
  fiber->vstk.top -= n;
}

static inline void rak_vm_new_range(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  (void) cl;
  (void) ip;
  (void) slots;
  RakValue val1 = rak_fiber_get(fiber, 1);
  RakValue val2 = rak_fiber_get(fiber, 0);
  if (!rak_is_number(val1) || !rak_is_number(val2)
   || !rak_is_integer(val1) || !rak_is_integer(val2))
  {
    rak_fiber_set_error(fiber, ip, err, "range must be of type integer numbers");
    return;
  }
  double start = rak_as_number(val1);
  double end = rak_as_number(val2);
  RakRange *range = rak_range_new(start, end, err);
  if (!rak_is_ok(err)) return;
  RakValue res = rak_range_value(range);
  rak_stack_set(&fiber->vstk, 1, res);
  rak_object_retain(&range->obj);
  rak_fiber_pop(fiber);
}

static inline void rak_vm_new_record(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  (void) cl;
  (void) slots;
  uint8_t len = rak_instr_a(*ip);
  RakRecord *rec = rak_record_new_with_capacity(len, err);
  if (!len)
  {
    RakValue res = rak_record_value(rec);
    rak_fiber_push_object(fiber, res, err);
    return;
  }
  int n = (len << 1) - 1;
  RakValue *_slots = &rak_stack_get(&fiber->vstk, n);
  if (!rak_is_ok(err)) return;
  for (int i = 0; i < len; ++i)
  {
    int j = i << 1;
    RakString *name = rak_as_string(_slots[j]);
    RakValue val = _slots[j + 1];
    int idx = rak_record_index_of(rec, name);
    if (idx >= 0)
    {
      RakRecordField *field = &rak_record_get(rec, idx);
      rak_value_retain(val);
      rak_value_release(field->val);
      field->val = val;
      continue;
    }
    RakRecordField field = {
      .name = name,
      .val = val,
    };
    rak_slice_ensure_append(&rec->slice, field, err);
    if (rak_is_ok(err)) continue;
    rak_record_free(rec);
    return;
  }
  _slots[0] = rak_record_value(rec);
  rak_object_retain(&rec->obj);
  fiber->vstk.top -= n;
}

static inline void rak_vm_new_closure(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  (void) slots;
  uint8_t idx = rak_instr_a(*ip);
  RakFunction *fn = (RakFunction *) cl->callable;
  RakFunction *nested = rak_slice_get(&fn->nested, idx);
  RakClosure *_cl = rak_closure_new(RAK_CALLABLE_TYPE_FUNCTION, &nested->callable, err);
  if (!rak_is_ok(err)) return;
  RakValue val = rak_closure_value(_cl);
  rak_fiber_push_object(fiber, val, err);
  if (rak_is_ok(err)) return;
  rak_closure_free(_cl);
}

static inline void rak_vm_move(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  (void) fiber;
  (void) cl;
  (void) err;
  uint32_t instr = *ip;
  uint8_t dst = rak_instr_a(instr);
  uint8_t src = rak_instr_b(instr);
  RakValue val = slots[src];
  rak_value_release(slots[dst]);
  slots[dst] = val;
  rak_value_retain(val);
}

static inline void rak_vm_pop(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  (void) cl;
  (void) ip;
  (void) slots;
  (void) err;
  rak_fiber_pop(fiber);
}

static inline void rak_vm_get_element(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  (void) cl;
  (void) ip;
  (void) slots;
  RakValue val1 = rak_fiber_get(fiber, 1);
  RakValue val2 = rak_fiber_get(fiber, 0);
  if (rak_is_string(val1))
  {
    RakString *str = rak_as_string(val1);
    if (!rak_is_range(val2))
    {
      rak_fiber_set_error(fiber, ip, err, "cannot index string with value of type %s",
        rak_type_to_cstr(val2.type));
      return;
    }
    RakRange *range = rak_as_range(val2);
    int start = (int) range->start;
    int end = (int) range->end;
    RakString *_str = rak_string_slice(str, start, end, err);
    if (!rak_is_ok(err)) return;
    RakValue res = rak_string_value(_str);
    rak_fiber_set_object(fiber, 1, res);
    rak_fiber_pop(fiber);
    return;
  }
  if (rak_is_array(val1))
  {
    RakArray *arr = rak_as_array(val1);
    if (rak_is_number(val2))
    {
      if (!rak_is_integer(val2))
      {
        rak_fiber_set_error(fiber, ip, err, "cannot index array with non-integer number");
        return;
      }
      int64_t idx = rak_as_integer(val2);
      if (idx < 0 || idx >= rak_array_len(arr))
      {
        rak_fiber_set_error(fiber, ip, err, "index out of bounds");
        return;
      }
      RakValue res = rak_array_get(arr, (int) idx);
      rak_fiber_set_value(fiber, 1, res);
      rak_fiber_pop(fiber);
      return;
    }
    if (!rak_is_range(val2))
    {
      rak_fiber_set_error(fiber, ip, err, "cannot index array with value of type %s",
        rak_type_to_cstr(val2.type));
      return;
    }
    RakRange *range = rak_as_range(val2);
    int start = (int) range->start;
    int end = (int) range->end;
    RakArray *_arr = rak_array_slice(arr, start, end, err);
    if (!rak_is_ok(err)) return;
    RakValue res = rak_array_value(_arr);
    rak_fiber_set_object(fiber, 1, res);
    rak_fiber_pop(fiber);
    return;
  }
  if (rak_is_range(val1))
  {
    RakRange *range = rak_as_range(val1);
    if (!rak_is_number(val2) || !rak_is_integer(val2))
    {
      rak_fiber_set_error(fiber, ip, err, "cannot index range with non-integer number");
      return;
    }
    int64_t idx = rak_as_integer(val2);
    if (idx < 0 || idx >= rak_range_len(range))
    {
      rak_fiber_set_error(fiber, ip, err, "index out of bounds");
      return;
    }
    RakValue res = rak_number_value(rak_range_get(range, (int) idx));
    rak_fiber_set_value(fiber, 1, res);
    rak_fiber_pop(fiber);
    return;
  }
  if (!rak_is_record(val1))
  {
    rak_fiber_set_error(fiber, ip, err, "cannot index value of type %s",
      rak_type_to_cstr(val1.type));
    return;
  }
  RakRecord *rec = rak_as_record(val1);
  if (!rak_is_string(val2))
  {
    rak_fiber_set_error(fiber, ip, err, "cannot index record with value of type %s",
      rak_type_to_cstr(val2.type));
    return;
  }
  RakString *name = rak_as_string(val2);
  int idx = rak_record_index_of(rec, name);
  if (idx == -1)
  {
    rak_fiber_set_error(fiber, ip, err, "record has no field named '%.*s'",
      rak_string_len(name), rak_string_chars(name));
    return;
  }
  RakValue res = rak_record_get(rec, idx).val;
  rak_fiber_set_value(fiber, 1, res);
  rak_fiber_pop(fiber);
}

static inline void rak_vm_set_element(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  (void) cl;
  (void) ip;
  (void) slots;
  RakValue val1 = rak_fiber_get(fiber, 2);
  RakValue val2 = rak_fiber_get(fiber, 1);
  RakValue val3 = rak_fiber_get(fiber, 0);
  if (rak_is_array(val1))
  {
    RakArray *arr = rak_as_array(val1);
    if (!rak_is_number(val2) || !rak_is_integer(val2))
    {
      rak_fiber_set_error(fiber, ip, err, "cannot index array with non-integer number");
      return;
    }
    int64_t idx = rak_as_integer(val2);
    if (idx < 0 || idx >= rak_array_len(arr))
    {
      rak_fiber_set_error(fiber, ip, err, "index out of bounds");
      return;
    }
    if (rak_is_shared(val1))
    {
      RakArray *_arr = rak_array_set(arr, (int) idx, val3, err);
      if (!rak_is_ok(err)) return;
      RakValue res = rak_array_value(_arr);
      rak_fiber_set_object(fiber, 2, res);
      rak_value_release(val3);
      fiber->vstk.top -= 2;
      return;
    }
    rak_array_inplace_set(arr, (int) idx, val3);
    rak_value_release(val3);
    fiber->vstk.top -= 2;
    return;
  }
  if (!rak_is_record(val1))
  {
    rak_fiber_set_error(fiber, ip, err, "cannot index value of type %s",
      rak_type_to_cstr(val1.type));
    return;
  }
  RakRecord *rec = rak_as_record(val1);
  if (!rak_is_string(val2))
  {
    rak_fiber_set_error(fiber, ip, err, "cannot index record with value of type %s",
      rak_type_to_cstr(val2.type));
    return;
  }
  RakString *name = rak_as_string(val2);
  if (rak_is_shared(val1))
  {
    RakRecord *_rec = rak_record_put(rec, name, val3, err);
    if (!rak_is_ok(err)) return;
    RakValue res = rak_record_value(_rec);
    rak_fiber_set_object(fiber, 2, res);
    rak_value_release(val3);
    fiber->vstk.top -= 2;
    return;
  }
  rak_record_inplace_put(rec, name, val3, err);
  if (!rak_is_ok(err)) return;
  rak_value_release(val3);
  fiber->vstk.top -= 2;
}

static inline void rak_vm_load_element(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  (void) cl;
  (void) ip;
  (void) slots;
  RakValue val1 = rak_fiber_get(fiber, 1);
  RakValue val2 = rak_fiber_get(fiber, 0);
  if (rak_is_array(val1))
  {
    RakArray *arr = rak_as_array(val1);
    if (!rak_is_number(val2) || !rak_is_integer(val2))
    {
      rak_fiber_set_error(fiber, ip, err, "cannot index array with non-integer number");
      return;
    }
    int64_t idx = rak_as_integer(val2);
    if (idx < 0 || idx >= rak_array_len(arr))
    {
      rak_fiber_set_error(fiber, ip, err, "index out of bounds");
      return;
    }
    RakValue res = rak_array_get(arr, (int) idx);
    rak_fiber_push_value(fiber, res, err);
    return;
  }
  if (!rak_is_record(val1))
  {
    rak_fiber_set_error(fiber, ip, err, "cannot index value of type %s",
      rak_type_to_cstr(val1.type));
    return;
  }
  RakRecord *rec = rak_as_record(val1);
  if (!rak_is_string(val2))
  {
    rak_fiber_set_error(fiber, ip, err, "cannot index record with value of type %s",
      rak_type_to_cstr(val2.type));
    return;
  }
  RakString *name = rak_as_string(val2);
  int idx = rak_record_index_of(rec, name);
  if (idx == -1)
  {
    rak_fiber_set_error(fiber, ip, err, "record has no field named '%.*s'",
      rak_string_len(name), rak_string_chars(name));
    return;
  }
  rak_stack_set(&fiber->vstk, 0, rak_number_value(idx));
  rak_string_release(name);
  RakValue res = rak_record_get(rec, idx).val;
  rak_fiber_push_value(fiber, res, err);
}

static inline void rak_vm_fetch_element(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  (void) cl;
  (void) ip;
  (void) slots;
  RakValue val1 = rak_fiber_get(fiber, 1);
  RakValue val2 = rak_fiber_get(fiber, 0);
  if (rak_is_array(val1))
  {
    RakArray *arr = rak_as_array(val1);
    if (!rak_is_number(val2) || !rak_is_integer(val2))
    {
      rak_fiber_set_error(fiber, ip, err, "cannot index array with non-integer number");
      return;
    }
    int64_t idx = rak_as_integer(val2);
    if (idx < 0 || idx >= rak_array_len(arr))
    {
      rak_fiber_set_error(fiber, ip, err, "index out of bounds");
      return;
    }
    RakValue res = rak_array_get(arr, (int) idx);
    if (rak_is_shared(val1) || (rak_is_object(res) && rak_as_object(res)->refCount > 1))
      res.flags |= RAK_FLAG_SHARED;
    rak_fiber_push_value(fiber, res, err);
    return;
  }
  if (!rak_is_record(val1))
  {
    rak_fiber_set_error(fiber, ip, err, "cannot index value of type %s",
      rak_type_to_cstr(val1.type));
    return;
  }
  RakRecord *rec = rak_as_record(val1);
  if (!rak_is_string(val2))
  {
    rak_fiber_set_error(fiber, ip, err, "cannot index record with value of type %s",
      rak_type_to_cstr(val2.type));
    return;
  }
  RakString *name = rak_as_string(val2);
  int idx = rak_record_index_of(rec, name);
  if (idx == -1)
  {
    rak_fiber_set_error(fiber, ip, err, "record has no field named '%.*s'",
      rak_string_len(name), rak_string_chars(name));
    return;
  }
  rak_stack_set(&fiber->vstk, 0, rak_number_value(idx));
  rak_string_release(name);
  RakValue res = rak_record_get(rec, idx).val;
  if (rak_is_shared(val1) || (rak_is_object(res) && rak_as_object(res)->refCount > 1))
    res.flags |= RAK_FLAG_SHARED;
  rak_fiber_push_value(fiber, res, err);
}

static inline void rak_vm_update_element(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  (void) cl;
  (void) ip;
  (void) slots;
  RakValue val1 = rak_fiber_get(fiber, 2);
  RakValue val2 = rak_fiber_get(fiber, 1);
  RakValue val3 = rak_fiber_get(fiber, 0);
  int idx = (int) rak_as_number(val2);
  if (rak_is_array(val1))
  {
    RakArray *arr = rak_as_array(val1);
    if (rak_is_shared(val1))
    {
      RakArray *_arr = rak_array_set(arr, idx, val3, err);
      if (!rak_is_ok(err)) return;
      RakValue res = rak_array_value(_arr);
      rak_fiber_set_object(fiber, 2, res);
      rak_value_release(val3);
      fiber->vstk.top -= 2;
      return;
    }
    rak_array_inplace_set(arr, idx, val3);
    rak_value_release(val3);
    fiber->vstk.top -= 2;
    return;
  }
  RakRecord *rec = rak_as_record(val1);
  if (rak_is_shared(val1))
  {
    RakRecord *_rec = rak_record_set(rec, idx, val3, err);
    if (!rak_is_ok(err)) return;
    RakValue res = rak_record_value(_rec);
    rak_fiber_set_object(fiber, 2, res);
    rak_value_release(val3);
    fiber->vstk.top -= 2;
    return;
  }
  rak_record_inplace_set(rec, idx, val3);
  rak_value_release(val3);
  fiber->vstk.top -= 2;
}

static inline void rak_vm_get_field(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  (void) slots;
  uint8_t idx = rak_instr_a(*ip);
  RakValue val = rak_fiber_get(fiber, 0);
  if (!rak_is_record(val))
  {
    rak_fiber_set_error(fiber, ip, err, "cannot index value of type %s",
      rak_type_to_cstr(val.type));
    return;
  }
  RakRecord *rec = rak_as_record(val);
  RakChunk *chunk = &((RakFunction *) cl->callable)->chunk;
  RakString *name = rak_as_string(rak_slice_get(&chunk->consts, idx));
  int _idx = rak_record_index_of(rec, name);
  if (_idx == -1)
  {
    rak_fiber_set_error(fiber, ip, err, "record has no field named '%.*s'",
      rak_string_len(name), rak_string_chars(name));
    return;
  }
  RakValue res = rak_record_get(rec, _idx).val;
  rak_fiber_set_value(fiber, 0, res);
}

static inline void rak_vm_put_field(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  (void) slots;
  uint8_t idx = rak_instr_a(*ip);
  RakValue val1 = rak_fiber_get(fiber, 1);
  RakValue val2 = rak_fiber_get(fiber, 0);
  if (!rak_is_record(val1))
  {
    rak_fiber_set_error(fiber, ip, err, "cannot index value of type %s",
      rak_type_to_cstr(val1.type));
    return;
  }
  RakRecord *rec = rak_as_record(val1);
  RakChunk *chunk = &((RakFunction *) cl->callable)->chunk;
  RakString *name = rak_as_string(rak_slice_get(&chunk->consts, idx));
  if (rak_is_shared(val1))
  {
    RakRecord *_rec = rak_record_put(rec, name, val2, err);
    if (!rak_is_ok(err)) return;
    RakValue res = rak_record_value(_rec);
    rak_fiber_set_object(fiber, 1, res);
    rak_value_release(val2);
    rak_stack_pop(&fiber->vstk);
    return;
  }
  rak_record_inplace_put(rec, name, val2, err);
  if (!rak_is_ok(err)) return;
  rak_value_release(val2);
  rak_stack_pop(&fiber->vstk);
}

static inline void rak_vm_load_field(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  (void) slots;
  uint8_t idx = rak_instr_a(*ip);
  RakValue val = rak_fiber_get(fiber, 0);
  if (!rak_is_record(val))
  {
    rak_fiber_set_error(fiber, ip, err, "cannot index value of type %s",
      rak_type_to_cstr(val.type));
    return;
  }
  RakRecord *rec = rak_as_record(val);
  RakChunk *chunk = &((RakFunction *) cl->callable)->chunk;
  RakString *name = rak_as_string(rak_slice_get(&chunk->consts, idx));
  int _idx = rak_record_index_of(rec, name);
  if (_idx == -1)
  {
    rak_fiber_set_error(fiber, ip, err, "record has no field named '%.*s'",
      rak_string_len(name), rak_string_chars(name));
    return;
  }
  rak_fiber_push(fiber, rak_number_value(_idx), err);
  if (!rak_is_ok(err)) return;
  RakValue res = rak_record_get(rec, _idx).val;
  rak_fiber_push_value(fiber, res, err);
}

static inline void rak_vm_fetch_field(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  (void) slots;
  uint8_t idx = rak_instr_a(*ip);
  RakValue val = rak_fiber_get(fiber, 0);
  if (!rak_is_record(val))
  {
    rak_fiber_set_error(fiber, ip, err, "cannot index value of type %s",
      rak_type_to_cstr(val.type));
    return;
  }
  RakRecord *rec = rak_as_record(val);
  RakChunk *chunk = &((RakFunction *) cl->callable)->chunk;
  RakString *name = rak_as_string(rak_slice_get(&chunk->consts, idx));
  int _idx = rak_record_index_of(rec, name);
  if (_idx == -1)
  {
    rak_fiber_set_error(fiber, ip, err, "record has no field named '%.*s'",
      rak_string_len(name), rak_string_chars(name));
    return;
  }
  rak_fiber_push(fiber, rak_number_value(_idx), err);
  if (!rak_is_ok(err)) return;
  RakValue res = rak_record_get(rec, _idx).val;
  if (rak_is_shared(val) || (rak_is_object(res) && rak_as_object(res)->refCount > 1))
    res.flags |= RAK_FLAG_SHARED;
  rak_fiber_push_value(fiber, res, err);
}

static inline void rak_vm_update_field(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  (void) cl;
  (void) ip;
  (void) slots;
  RakValue val1 = rak_fiber_get(fiber, 2);
  RakValue val2 = rak_fiber_get(fiber, 1);
  RakValue val3 = rak_fiber_get(fiber, 0);
  RakRecord *rec = rak_as_record(val1);
  int idx = (int) rak_as_number(val2);
  if (rak_is_shared(val1))
  {
    RakRecord *_rec = rak_record_set(rec, idx, val3, err);
    if (!rak_is_ok(err)) return;
    RakValue res = rak_record_value(_rec);
    rak_fiber_set_object(fiber, 2, res);
    rak_value_release(val3);
    fiber->vstk.top -= 2;
    return;
  }
  rak_record_inplace_set(rec, idx, val3);
  rak_value_release(val3);
  fiber->vstk.top -= 2;
}

static inline void rak_vm_unpack_elements(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  (void) cl;
  (void) slots;
  uint8_t n = rak_instr_a(*ip);
  RakValue *_slots = &rak_stack_get(&fiber->vstk, n);
  RakValue val = _slots[0];
  if (!rak_is_array(val))
  {
    rak_fiber_set_error(fiber, ip, err, "cannot unpack elements from value of type %s",
      rak_type_to_cstr(val.type));
    return;
  }
  RakArray *arr = rak_as_array(val);
  int len = rak_array_len(arr);
  for (int i = 0; i < n && i < len; ++i)
  {
    int idx = (int) rak_as_integer(_slots[i + 1]);
    RakValue _val = rak_array_get(arr, idx);
    _slots[i] = _val;
    rak_value_retain(_val);
  }
  for (int i = len; i < n; ++i)
    _slots[i] = rak_nil_value();
  rak_stack_pop(&fiber->vstk);
  rak_array_release(arr);
}

static inline void rak_vm_unpack_fields(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  (void) cl;
  (void) slots;
  uint8_t n = rak_instr_a(*ip);
  RakValue *_slots = &rak_stack_get(&fiber->vstk, n);
  RakValue val = _slots[0];
  if (!rak_is_record(val))
  {
    rak_fiber_set_error(fiber, ip, err, "cannot unpack fields from value of type %s",
      rak_type_to_cstr(val.type));
    return;
  }
  RakRecord *rec = rak_as_record(val);
  RakString *name = rak_as_string(_slots[1]);
  int idx = rak_record_index_of(rec, name);
  if (idx == -1)
  {
    rak_fiber_set_error(fiber, ip, err, "record has no field named '%.*s'",
      rak_string_len(name), rak_string_chars(name));
    return;
  }
  RakValue _val = rak_record_get(rec, idx).val;
  _slots[0] = _val;
  rak_value_retain(_val);
  for (int i = 1; i < n; ++i)
  {
    name = rak_as_string(_slots[i + 1]);
    idx = rak_record_index_of(rec, name);
    if (idx == -1)
    {
      rak_fiber_set_error(fiber, ip, err, "record has no field named '%.*s'",
        rak_string_len(name), rak_string_chars(name));
      rak_record_release(rec);
      return;
    }
    _val = rak_record_get(rec, idx).val;
    RakString *str = rak_as_string(_slots[i]);
    _slots[i] = _val;
    rak_value_retain(_val);
    rak_string_release(str);
  }
  RakString *str = rak_as_string(_slots[n]);
  rak_stack_pop(&fiber->vstk);
  rak_string_release(str);
  rak_record_release(rec);
}

static inline void rak_vm_jump(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  (void) slots;
  (void) err;
  uint16_t off = rak_instr_ab(*ip);
  RakChunk *chunk = &((RakFunction *) cl->callable)->chunk;
  RakCallFrame *frame = &rak_stack_get(&fiber->cstk, 0);
  frame->state = &rak_slice_get(&chunk->instrs, off);
}

static inline void rak_vm_jump_if_false(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  (void) slots;
  (void) err;
  uint16_t off = rak_instr_ab(*ip);
  RakValue val = rak_fiber_get(fiber, 0);
  RakChunk *chunk = &((RakFunction *) cl->callable)->chunk;
  RakCallFrame *frame = &rak_stack_get(&fiber->cstk, 0);
  frame->state = rak_is_falsy(val) ? &rak_slice_get(&chunk->instrs, off) : ip + 1;
}

static inline void rak_vm_jump_if_true(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  (void) slots;
  (void) err;
  uint16_t off = rak_instr_ab(*ip);
  RakValue val = rak_fiber_get(fiber, 0);
  RakChunk *chunk = &((RakFunction *) cl->callable)->chunk;
  RakCallFrame *frame = &rak_stack_get(&fiber->cstk, 0);
  frame->state = rak_is_falsy(val) ? ip + 1 : &rak_slice_get(&chunk->instrs, off);
}

static inline void rak_vm_eq(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  (void) cl;
  (void) ip;
  (void) slots;
  (void) err;
  RakValue val1 = rak_fiber_get(fiber, 1);
  RakValue val2 = rak_fiber_get(fiber, 0);
  RakValue res = rak_bool_value(rak_value_equals(val1, val2));
  rak_fiber_set(fiber, 1, res);
  rak_fiber_pop(fiber);
}

static inline void rak_vm_gt(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  (void) cl;
  (void) ip;
  (void) slots;
  RakValue val1 = rak_fiber_get(fiber, 1);
  RakValue val2 = rak_fiber_get(fiber, 0);
  int cmp = rak_value_compare(val1, val2, err);
  if (!rak_is_ok(err)) return;
  RakValue res = rak_bool_value(cmp > 0);
  rak_fiber_set(fiber, 1, res);
  rak_fiber_pop(fiber);
}

static inline void rak_vm_ge(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  (void) cl;
  (void) ip;
  (void) slots;
  RakValue val1 = rak_fiber_get(fiber, 1);
  RakValue val2 = rak_fiber_get(fiber, 0);
  int cmp = rak_value_compare(val1, val2, err);
  if (!rak_is_ok(err)) return;
  RakValue res = rak_bool_value(cmp >= 0);
  rak_fiber_set(fiber, 1, res);
  rak_fiber_pop(fiber);
}

static inline void rak_vm_lt(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  (void) cl;
  (void) ip;
  (void) slots;
  RakValue val1 = rak_fiber_get(fiber, 1);
  RakValue val2 = rak_fiber_get(fiber, 0);
  int cmp = rak_value_compare(val1, val2, err);
  if (!rak_is_ok(err)) return;
  RakValue res = rak_bool_value(cmp < 0);
  rak_fiber_set(fiber, 1, res);
  rak_fiber_pop(fiber);
}

static inline void rak_vm_le(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  (void) cl;
  (void) ip;
  (void) slots;
  RakValue val1 = rak_fiber_get(fiber, 1);
  RakValue val2 = rak_fiber_get(fiber, 0);
  int cmp = rak_value_compare(val1, val2, err);
  if (!rak_is_ok(err)) return;
  RakValue res = rak_bool_value(cmp <= 0);
  rak_fiber_set(fiber, 1, res);
  rak_fiber_pop(fiber);
}

static inline void rak_vm_add(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  (void) cl;
  (void) ip;
  (void) slots;
  RakValue val1 = rak_fiber_get(fiber, 1);
  RakValue val2 = rak_fiber_get(fiber, 0);
  if (rak_is_number(val1))
  {
    if (!rak_is_number(val2))
    {
      rak_fiber_set_error(fiber, ip, err, "cannot add number and %s",
        rak_type_to_cstr(val2.type));
      return;
    }
    double num1 = rak_as_number(val1);
    double num2 = rak_as_number(val2);
    RakValue res = rak_number_value(num1 + num2);
    rak_fiber_set(fiber, 1, res);
    rak_fiber_pop(fiber);
    return;
  }
  if (rak_is_string(val1))
  {
    if (!rak_is_string(val2))
    {
      rak_fiber_set_error(fiber, ip, err, "cannot add string and %s",
        rak_type_to_cstr(val2.type));
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
    rak_fiber_set_object(fiber, 1, res);
    rak_fiber_pop(fiber);
    return;
  }
  if (rak_is_array(val1))
  {
    if (!rak_is_array(val2))
    {
      rak_fiber_set_error(fiber, ip, err, "cannot add array and %s",
        rak_type_to_cstr(val2.type));
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
    rak_fiber_set_object(fiber, 1, res);
    rak_fiber_pop(fiber);
    return;
  }
  rak_fiber_set_error(fiber, ip, err, "cannot add %s and %s",
    rak_type_to_cstr(val1.type), rak_type_to_cstr(val2.type));
}

static inline void rak_vm_add2(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  (void) cl;
  uint8_t lhs = rak_instr_a(*ip);
  uint8_t rhs = rak_instr_b(*ip);
  RakValue val1 = slots[lhs];
  RakValue val2 = slots[rhs];
  if (rak_is_number(val1))
  {
    if (!rak_is_number(val2))
    {
      rak_fiber_set_error(fiber, ip, err, "cannot add number and %s",
        rak_type_to_cstr(val2.type));
      return;
    }
    double num1 = rak_as_number(val1);
    double num2 = rak_as_number(val2);
    RakValue res = rak_number_value(num1 + num2);
    rak_fiber_push(fiber, res, err);
    return;
  }
  if (rak_is_string(val1))
  {
    if (!rak_is_string(val2))
    {
      rak_fiber_set_error(fiber, ip, err, "cannot add string and %s",
        rak_type_to_cstr(val2.type));
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
    rak_fiber_push_object(fiber, res, err);
    return;
  }
  if (rak_is_array(val1))
  {
    if (!rak_is_array(val2))
    {
      rak_fiber_set_error(fiber, ip, err, "cannot add array and %s",
        rak_type_to_cstr(val2.type));
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
    rak_fiber_push_object(fiber, res, err);
    return;
  }
  rak_fiber_set_error(fiber, ip, err, "cannot add %s and %s",
    rak_type_to_cstr(val1.type), rak_type_to_cstr(val2.type));
}

static inline void rak_vm_add3(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  (void) cl;
  uint8_t dst = rak_instr_a(*ip);
  uint8_t lhs = rak_instr_b(*ip);
  uint8_t rhs = rak_instr_c(*ip);
  RakValue val1 = slots[lhs];
  RakValue val2 = slots[rhs];
  if (rak_is_number(val1))
  {
    if (!rak_is_number(val2))
    {
      rak_fiber_set_error(fiber, ip, err, "cannot add number and %s",
        rak_type_to_cstr(val2.type));
      return;
    }
    double num1 = rak_as_number(val1);
    double num2 = rak_as_number(val2);
    RakValue res = rak_number_value(num1 + num2);
    rak_value_release(slots[dst]);
    slots[dst] = res;
    return;
  }
  if (rak_is_string(val1))
  {
    if (!rak_is_string(val2))
    {
      rak_fiber_set_error(fiber, ip, err, "cannot add string and %s",
        rak_type_to_cstr(val2.type));
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
    rak_value_release(slots[dst]);
    slots[dst] = res;
    rak_value_retain(res);
    return;
  }
  if (rak_is_array(val1))
  {
    if (!rak_is_array(val2))
    {
      rak_fiber_set_error(fiber, ip, err, "cannot add array and %s",
        rak_type_to_cstr(val2.type));
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
    rak_value_release(slots[dst]);
    slots[dst] = res;
    rak_value_retain(res);
    return;
  }
  rak_fiber_set_error(fiber, ip, err, "cannot add %s and %s",
    rak_type_to_cstr(val1.type), rak_type_to_cstr(val2.type));
}

static inline void rak_vm_sub(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  (void) cl;
  (void) ip;
  (void) slots;
  RakValue val1 = rak_fiber_get(fiber, 1);
  RakValue val2 = rak_fiber_get(fiber, 0);
  if (!rak_is_number(val1) || !rak_is_number(val2))
  {
    rak_fiber_set_error(fiber, ip, err, "cannot subtract non-number values");
    return;
  }
  double num1 = rak_as_number(val1);
  double num2 = rak_as_number(val2);
  RakValue res = rak_number_value(num1 - num2);
  rak_fiber_set(fiber, 1, res);
  rak_fiber_pop(fiber);
}

static inline void rak_vm_sub2(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  (void) cl;
  uint8_t lhs = rak_instr_a(*ip);
  uint8_t rhs = rak_instr_b(*ip);
  RakValue val1 = slots[lhs];
  RakValue val2 = slots[rhs];
  if (!rak_is_number(val1) || !rak_is_number(val2))
  {
    rak_fiber_set_error(fiber, ip, err, "cannot subtract non-number values");
    return;
  }
  double num1 = rak_as_number(val1);
  double num2 = rak_as_number(val2);
  RakValue res = rak_number_value(num1 - num2);
  rak_fiber_push(fiber, res, err);
}

static inline void rak_vm_sub3(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  (void) cl;
  uint8_t dst = rak_instr_a(*ip);
  uint8_t lhs = rak_instr_b(*ip);
  uint8_t rhs = rak_instr_c(*ip);
  RakValue val1 = slots[lhs];
  RakValue val2 = slots[rhs];
  if (!rak_is_number(val1) || !rak_is_number(val2))
  {
    rak_fiber_set_error(fiber, ip, err, "cannot subtract non-number values");
    return;
  }
  double num1 = rak_as_number(val1);
  double num2 = rak_as_number(val2);
  RakValue res = rak_number_value(num1 - num2);
  rak_value_release(slots[dst]);
  slots[dst] = res;
}

static inline void rak_vm_mul(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  (void) cl;
  (void) ip;
  (void) slots;
  RakValue val1 = rak_fiber_get(fiber, 1);
  RakValue val2 = rak_fiber_get(fiber, 0);
  if (!rak_is_number(val1) || !rak_is_number(val2))
  {
    rak_fiber_set_error(fiber, ip, err, "cannot multiply non-number values");
    return;
  }
  double num1 = rak_as_number(val1);
  double num2 = rak_as_number(val2);
  RakValue res = rak_number_value(num1 * num2);
  rak_fiber_set(fiber, 1, res);
  rak_fiber_pop(fiber);
}

static inline void rak_vm_mul2(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  (void) cl;
  uint8_t lhs = rak_instr_a(*ip);
  uint8_t rhs = rak_instr_b(*ip);
  RakValue val1 = slots[lhs];
  RakValue val2 = slots[rhs];
  if (!rak_is_number(val1) || !rak_is_number(val2))
  {
    rak_fiber_set_error(fiber, ip, err, "cannot multiply non-number values");
    return;
  }
  double num1 = rak_as_number(val1);
  double num2 = rak_as_number(val2);
  RakValue res = rak_number_value(num1 * num2);
  rak_fiber_push(fiber, res, err);
}

static inline void rak_vm_mul3(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  (void) cl;
  uint8_t dst = rak_instr_a(*ip);
  uint8_t lhs = rak_instr_b(*ip);
  uint8_t rhs = rak_instr_c(*ip);
  RakValue val1 = slots[lhs];
  RakValue val2 = slots[rhs];
  if (!rak_is_number(val1) || !rak_is_number(val2))
  {
    rak_fiber_set_error(fiber, ip, err, "cannot multiply non-number values");
    return;
  }
  double num1 = rak_as_number(val1);
  double num2 = rak_as_number(val2);
  RakValue res = rak_number_value(num1 * num2);
  rak_value_release(slots[dst]);
  slots[dst] = res;
}

static inline void rak_vm_div(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  (void) cl;
  (void) ip;
  (void) slots;
  RakValue val1 = rak_fiber_get(fiber, 1);
  RakValue val2 = rak_fiber_get(fiber, 0);
  if (!rak_is_number(val1) || !rak_is_number(val2))
  {
    rak_fiber_set_error(fiber, ip, err, "cannot divide non-number values");
    return;
  }
  double num1 = rak_as_number(val1);
  double num2 = rak_as_number(val2);
  RakValue res = rak_number_value(num1 / num2);
  rak_fiber_set(fiber, 1, res);
  rak_fiber_pop(fiber);
}

static inline void rak_vm_div2(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  (void) cl;
  uint8_t lhs = rak_instr_a(*ip);
  uint8_t rhs = rak_instr_b(*ip);
  RakValue val1 = slots[lhs];
  RakValue val2 = slots[rhs];
  if (!rak_is_number(val1) || !rak_is_number(val2))
  {
    rak_fiber_set_error(fiber, ip, err, "cannot divide non-number values");
    return;
  }
  double num1 = rak_as_number(val1);
  double num2 = rak_as_number(val2);
  RakValue res = rak_number_value(num1 / num2);
  rak_fiber_push(fiber, res, err);
}

static inline void rak_vm_div3(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  (void) cl;
  uint8_t dst = rak_instr_a(*ip);
  uint8_t lhs = rak_instr_b(*ip);
  uint8_t rhs = rak_instr_c(*ip);
  RakValue val1 = slots[lhs];
  RakValue val2 = slots[rhs];
  if (!rak_is_number(val1) || !rak_is_number(val2))
  {
    rak_fiber_set_error(fiber, ip, err, "cannot divide non-number values");
    return;
  }
  double num1 = rak_as_number(val1);
  double num2 = rak_as_number(val2);
  RakValue res = rak_number_value(num1 / num2);
  rak_value_release(slots[dst]);
  slots[dst] = res;
}

static inline void rak_vm_mod(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  (void) cl;
  (void) ip;
  (void) slots;
  RakValue val1 = rak_fiber_get(fiber, 1);
  RakValue val2 = rak_fiber_get(fiber, 0);
  if (!rak_is_number(val1) || !rak_is_number(val2))
  {
    rak_fiber_set_error(fiber, ip, err, "cannot calculate modulo of non-number values");
    return;
  }
  double num1 = rak_as_number(val1);
  double num2 = rak_as_number(val2);
  RakValue res = rak_number_value(fmod(num1, num2));
  rak_fiber_set(fiber, 1, res);
  rak_fiber_pop(fiber);
}

static inline void rak_vm_mod2(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  (void) cl;
  uint8_t lhs = rak_instr_a(*ip);
  uint8_t rhs = rak_instr_b(*ip);
  RakValue val1 = slots[lhs];
  RakValue val2 = slots[rhs];
  if (!rak_is_number(val1) || !rak_is_number(val2))
  {
    rak_fiber_set_error(fiber, ip, err, "cannot calculate modulo of non-number values");
    return;
  }
  double num1 = rak_as_number(val1);
  double num2 = rak_as_number(val2);
  RakValue res = rak_number_value(fmod(num1, num2));
  rak_fiber_push(fiber, res, err);
}

static inline void rak_vm_mod3(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  (void) cl;
  uint8_t dst = rak_instr_a(*ip);
  uint8_t lhs = rak_instr_b(*ip);
  uint8_t rhs = rak_instr_c(*ip);
  RakValue val1 = slots[lhs];
  RakValue val2 = slots[rhs];
  if (!rak_is_number(val1) || !rak_is_number(val2))
  {
    rak_fiber_set_error(fiber, ip, err, "cannot calculate modulo of non-number values");
    return;
  }
  double num1 = rak_as_number(val1);
  double num2 = rak_as_number(val2);
  RakValue res = rak_number_value(fmod(num1, num2));
  rak_value_release(slots[dst]);
  slots[dst] = res;
}

static inline void rak_vm_not(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  (void) cl;
  (void) ip;
  (void) slots;
  (void) err;
  RakValue val = rak_fiber_get(fiber, 0);
  RakValue res = rak_is_falsy(val) ? rak_bool_value(true) : rak_bool_value(false);
  rak_fiber_set(fiber, 0, res);
}

static inline void rak_vm_neg(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  (void) cl;
  (void) ip;
  (void) slots;
  RakValue val = rak_fiber_get(fiber, 0);
  if (!rak_is_number(val))
  {
    rak_fiber_set_error(fiber, ip, err, "cannot negate a non-number value");
    return;
  }
  double num = rak_as_number(val);
  RakValue res = rak_number_value(-num);
  rak_fiber_set(fiber, 0, res);
}

static inline void rak_vm_call(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  (void) cl;
  (void) slots;
  uint8_t nargs = rak_instr_a(*ip);
  RakValue *_slots = &rak_stack_get(&fiber->vstk, nargs);
  RakValue val = _slots[0];
  if (!rak_is_closure(val))
  {
    rak_fiber_set_error(fiber, ip, err, "cannot call non-closure value");
    return;
  }
  if (rak_stack_is_full(&fiber->cstk))
  {
    rak_fiber_set_error(fiber, ip, err, "too many nested calls");
    return;
  }
  RakClosure *_cl = rak_as_closure(val);
  int arity = _cl->callable->arity;
  while (nargs > arity)
  {
    rak_fiber_pop(fiber);
    --nargs;
  }
  while (nargs < arity)
  {
    rak_fiber_push_nil(fiber, err);
    if (!rak_is_ok(err)) return;
    ++nargs;
  }
  int n = _cl->callable->inouts.len;
  for (int i = 0; i < n; ++i)
  {
    int idx = rak_slice_get(&_cl->callable->inouts, i);
    RakValue _val = _slots[idx];
    if (rak_is_ref(_val)) continue;
    rak_fiber_set_error(fiber, ip, err, "argument #%d must be a reference, got %s",
      idx, rak_type_to_cstr(_val.type));
    return;
  }
  RakCallFrame *frame = &rak_stack_get(&fiber->cstk, 0);
  frame->state = ip + 1;
  RakCallFrame _frame = {
    .cl = _cl,
    .slots = _slots
  };
  if (_cl->type == RAK_CALLABLE_TYPE_FUNCTION)
  {
    RakFunction *fn = (RakFunction *) _cl->callable;
    _frame.state = fn->chunk.instrs.data;
    rak_stack_push(&fiber->cstk, _frame);
    return;
  }
  _frame.state = (void *) 0;
  rak_stack_push(&fiber->cstk, _frame);
}

static inline void rak_vm_tail_call(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  uint8_t nargs = rak_instr_a(*ip);
  RakValue *_slots = &rak_stack_get(&fiber->vstk, nargs);
  RakValue val = _slots[0];
  if (!rak_is_closure(val))
  {
    rak_fiber_set_error(fiber, ip, err, "cannot call non-closure value");
    return;
  }
  RakClosure *_cl = rak_as_closure(val);
  int arity = _cl->callable->arity;
  while (nargs > arity)
  {
    rak_fiber_pop(fiber);
    --nargs;
  }
  while (nargs < arity)
  {
    rak_fiber_push_nil(fiber, err);
    if (!rak_is_ok(err)) return;
    ++nargs;
  }
  int n = _cl->callable->inouts.len;
  for (int i = 0; i < n; ++i)
  {
    int idx = rak_slice_get(&_cl->callable->inouts, i);
    RakValue _val = _slots[idx];
    if (rak_is_ref(_val)) continue;
    rak_fiber_set_error(fiber, ip, err, "argument #%d must be a reference, got %s",
      idx, rak_type_to_cstr(_val.type));
    return;
  }
  rak_closure_release(cl);
  slots[0] = _slots[0];
  for (int i = 1; i <= arity; ++i)
  {
    RakValue _val = _slots[i];
    rak_value_release(slots[i]);
    slots[i] = _val;
  }
  fiber->vstk.top = &slots[arity];
  RakCallFrame *frame = &rak_stack_get(&fiber->cstk, 0);
  frame->cl = _cl;
  if (_cl->type == RAK_CALLABLE_TYPE_FUNCTION)
  {
    RakFunction *fn = (RakFunction *) _cl->callable;
    frame->state = fn->chunk.instrs.data;
    return;
  }
  frame->state = (void *) 0;
}

static inline void rak_vm_yield(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  (void) cl;
  (void) slots;
  (void) err;
  rak_fiber_yield(fiber, ip);
}

static inline void rak_vm_return(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  (void) ip;
  (void) err;
  rak_fiber_return(fiber, cl, slots);
}

static inline void rak_vm_return_nil(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  rak_fiber_push_nil(fiber, err);
  if (!rak_is_ok(err)) return;
  rak_vm_return(fiber, cl, ip, slots, err);
}

#endif // RAK_VM_H
