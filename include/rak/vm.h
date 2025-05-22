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

#include "fiber.h"
#include "function.h"
#include <math.h>
#include "range.h"
#include "record.h"
#include "value.h"

static inline void rak_vm_push(RakFiber *fiber, RakValue val, RakError *err);
static inline void rak_vm_push_nil(RakFiber *fiber, RakError *err);
static inline void rak_vm_push_bool(RakFiber *fiber, bool data, RakError *err);
static inline void rak_vm_push_number(RakFiber *fiber, double data, RakError *err);
static inline void rak_vm_push_value(RakFiber *fiber, RakValue val, RakError *err);
static inline void rak_vm_push_object(RakFiber *fiber, RakValue val, RakError *err);
static inline void rak_vm_load_const(RakFiber *fiber, RakChunk *chunk, uint8_t idx, RakError *err);
static inline void rak_vm_load_global(RakFiber *fiber, uint8_t idx, RakError *err);
static inline void rak_vm_load_local(RakFiber *fiber, RakValue *slots, uint8_t idx, RakError *err);
static inline void rak_vm_store_local(RakFiber *fiber, RakValue *slots, uint8_t idx);
static inline void rak_vm_fetch_local(RakFiber *fiber, RakValue *slots, uint8_t idx, RakError *err);
static inline void rak_vm_ref_local(RakFiber *fiber, RakValue *slots, uint8_t idx, RakError *err);
static inline void rak_vm_load_local_ref(RakFiber *fiber, RakValue *slots, uint8_t idx, RakError *err);
static inline void rak_vm_store_local_ref(RakFiber *fiber, RakValue *slots, uint8_t idx);
static inline void rak_vm_check_ref(RakValue *slots, uint8_t idx, RakError *err);
static inline void rak_vm_new_array(RakFiber *fiber, uint8_t len, RakError *err);
static inline void rak_vm_new_range(RakFiber *fiber, RakError *err);
static inline void rak_vm_new_record(RakFiber *fiber, uint8_t len, RakError *err);
static inline void rak_vm_new_closure(RakFiber *fiber, RakFunction *fn, uint8_t idx, RakError *err);
static inline void rak_vm_dup(RakFiber *fiber, RakError *err);
static inline void rak_vm_pop(RakFiber *fiber);
static inline void rak_vm_get_element(RakFiber *fiber, RakError *err);
static inline void rak_vm_set_element(RakFiber *fiber, RakError *err);
static inline void rak_vm_load_element(RakFiber *fiber, RakError *err);
static inline void rak_vm_fetch_element(RakFiber *fiber, RakError *err);
static inline void rak_vm_update_element(RakFiber *fiber, RakError *err);
static inline void rak_vm_get_field(RakFiber *fiber, RakChunk *chunk, uint8_t idx, RakError *err);
static inline void rak_vm_put_field(RakFiber *fiber, RakChunk *chunk, uint8_t idx, RakError *err);
static inline void rak_vm_load_field(RakFiber *fiber, RakChunk *chunk, uint8_t idx, RakError *err);
static inline void rak_vm_fetch_field(RakFiber *fiber, RakChunk *chunk, uint8_t idx, RakError *err);
static inline void rak_vm_update_field(RakFiber *fiber, RakError *err);
static inline void rak_vm_unpack_elements(RakFiber *fiber, uint8_t n, RakError *err);
static inline void rak_vm_unpack_fields(RakFiber *fiber, uint8_t n, RakError *err);
static inline RakValue rak_vm_get(RakFiber *fiber, uint8_t idx);
static inline void rak_vm_set(RakFiber *fiber, uint8_t idx, RakValue val);
static inline void rak_vm_set_value(RakFiber *fiber, uint8_t idx, RakValue val);
static inline void rak_vm_set_object(RakFiber *fiber, uint8_t idx, RakValue val);
static inline void rak_vm_eq(RakFiber *fiber);
static inline void rak_vm_gt(RakFiber *fiber, RakError *err);
static inline void rak_vm_lt(RakFiber *fiber, RakError *err);
static inline void rak_vm_add(RakFiber *fiber, RakError *err);
static inline void rak_vm_sub(RakFiber *fiber, RakError *err);
static inline void rak_vm_mul(RakFiber *fiber, RakError *err);
static inline void rak_vm_div(RakFiber *fiber, RakError *err);
static inline void rak_vm_mod(RakFiber *fiber, RakError *err);
static inline void rak_vm_not(RakFiber *fiber);
static inline void rak_vm_neg(RakFiber *fiber, RakError *err);
static inline void rak_vm_call(RakFiber *fiber, uint8_t nargs, RakError *err);
static inline void rak_vm_tail_call(RakFiber *fiber, RakValue *slots, uint8_t nargs, RakError *err);
static inline void rak_vm_yield(RakFiber *fiber);
static inline void rak_vm_return(RakFiber *fiber, RakClosure *cl, RakValue *slots);

void rak_vm_dispatch(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);

static inline void rak_vm_push(RakFiber *fiber, RakValue val, RakError *err)
{
  if (rak_stack_is_full(&fiber->vstk))
  {
    rak_error_set(err, "stack overflow");
    return;
  }
  rak_stack_push(&fiber->vstk, val);
}

static inline void rak_vm_push_nil(RakFiber *fiber, RakError *err)
{
  rak_vm_push(fiber, rak_nil_value(), err);
}

static inline void rak_vm_push_bool(RakFiber *fiber, bool data, RakError *err)
{
  rak_vm_push(fiber, rak_bool_value(data), err);
}

static inline void rak_vm_push_number(RakFiber *fiber, double data, RakError *err)
{
  rak_vm_push(fiber, rak_number_value(data), err);
}

static inline void rak_vm_push_value(RakFiber *fiber, RakValue val, RakError *err)
{
  rak_vm_push(fiber, val, err);
  if (!rak_is_ok(err)) return;
  rak_value_retain(val);
}

static inline void rak_vm_push_object(RakFiber *fiber, RakValue val, RakError *err)
{
  rak_vm_push(fiber, val, err);
  if (!rak_is_ok(err)) return;
  rak_object_retain(rak_as_object(val));
}

static inline void rak_vm_load_const(RakFiber *fiber, RakChunk *chunk, uint8_t idx, RakError *err)
{
  RakValue val = rak_slice_get(&chunk->consts, idx);
  rak_vm_push_value(fiber, val, err);
}

static inline void rak_vm_load_global(RakFiber *fiber, uint8_t idx, RakError *err)
{
  RakValue val = rak_array_get(fiber->globals, idx);
  rak_vm_push_value(fiber, val, err);
}

static inline void rak_vm_load_local(RakFiber *fiber, RakValue *slots, uint8_t idx, RakError *err)
{
  RakValue val = slots[idx];
  rak_vm_push_value(fiber, val, err);
}

static inline void rak_vm_store_local(RakFiber *fiber, RakValue *slots, uint8_t idx)
{
  RakValue val = rak_vm_get(fiber, 0);
  rak_value_release(slots[idx]);
  slots[idx] = val;
  rak_stack_pop(&fiber->vstk);
}

static inline void rak_vm_fetch_local(RakFiber *fiber, RakValue *slots, uint8_t idx, RakError *err)
{
  RakValue val = slots[idx];
  if (rak_is_object(val) && rak_as_object(val)->refCount > 1)
    val.flags |= RAK_FLAG_SHARED;
  rak_vm_push_value(fiber, val, err);
}

static inline void rak_vm_ref_local(RakFiber *fiber, RakValue *slots, uint8_t idx, RakError *err)
{
  RakValue *slot = &slots[idx];
  RakValue val = *slot;
  if (rak_is_object(val) && rak_as_object(val)->refCount > 1)
    slot->flags |= RAK_FLAG_SHARED;
  rak_vm_push_value(fiber, rak_ref_value(slot), err);
}

static inline void rak_vm_load_local_ref(RakFiber *fiber, RakValue *slots, uint8_t idx, RakError *err)
{
  RakValue *slot = rak_as_ref(slots[idx]);
  rak_vm_push_value(fiber, *slot, err);
}

static inline void rak_vm_store_local_ref(RakFiber *fiber, RakValue *slots, uint8_t idx)
{
  RakValue *slot = rak_as_ref(slots[idx]);
  RakValue val = rak_vm_get(fiber, 0);
  rak_value_release(*slot);
  *slot = val;
  rak_stack_pop(&fiber->vstk);
}

static inline void rak_vm_check_ref(RakValue *slots, uint8_t idx, RakError *err)
{
  RakValue val = slots[idx];
  if (rak_is_ref(val)) return;
  rak_error_set(err, "argument #%d must be a reference, got %s", idx,
    rak_type_to_cstr(val.type));
}

static inline void rak_vm_new_array(RakFiber *fiber, uint8_t len, RakError *err)
{
  RakArray *arr = rak_array_new_with_capacity(len, err);
  if (!len)
  {
    RakValue res = rak_array_value(arr);
    rak_vm_push_object(fiber, res, err);
    return;
  }
  int n = len - 1;
  RakValue *slots = &rak_stack_get(&fiber->vstk, n);
  if (!rak_is_ok(err)) return;
  for (int i = 0; i < len; ++i)
    rak_slice_set(&arr->slice, i, slots[i]);
  arr->slice.len = len;
  slots[0] = rak_array_value(arr);
  rak_object_retain(&arr->obj);
  fiber->vstk.top -= n;
}

static inline void rak_vm_new_range(RakFiber *fiber, RakError *err)
{
  RakValue val1 = rak_vm_get(fiber, 1);
  RakValue val2 = rak_vm_get(fiber, 0);
  if (!rak_is_number(val1) || !rak_is_number(val2)
   || !rak_is_integer(val1) || !rak_is_integer(val2))
  {
    rak_error_set(err, "range must be of type integer numbers");
    return;
  }
  double start = rak_as_number(val1);
  double end = rak_as_number(val2);
  RakRange *range = rak_range_new(start, end, err);
  if (!rak_is_ok(err)) return;
  RakValue res = rak_range_value(range);
  rak_stack_set(&fiber->vstk, 1, res);
  rak_object_retain(&range->obj);
  rak_vm_pop(fiber);
}

static inline void rak_vm_new_record(RakFiber *fiber, uint8_t len, RakError *err)
{
  RakRecord *rec = rak_record_new_with_capacity(len, err);
  if (!len)
  {
    RakValue res = rak_record_value(rec);
    rak_vm_push_object(fiber, res, err);
    return;
  }
  int n = (len << 1) - 1;
  RakValue *slots = &rak_stack_get(&fiber->vstk, n);
  if (!rak_is_ok(err)) return;
  for (int i = 0; i < len; ++i)
  {
    int j = i << 1;
    RakString *name = rak_as_string(slots[j]);
    RakValue val = slots[j + 1];
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
  slots[0] = rak_record_value(rec);
  rak_object_retain(&rec->obj);
  fiber->vstk.top -= n;
}

static inline void rak_vm_new_closure(RakFiber *fiber, RakFunction *fn, uint8_t idx, RakError *err)
{
  RakFunction *_fn = rak_slice_get(&fn->nested, idx);
  RakClosure *cl = rak_closure_new(RAK_CALLABLE_TYPE_FUNCTION, &_fn->callable, err);
  if (!rak_is_ok(err)) return;
  RakValue val = rak_closure_value(cl);
  rak_vm_push_object(fiber, val, err);
  if (rak_is_ok(err)) return;
  rak_closure_free(cl);
}

static inline void rak_vm_dup(RakFiber *fiber, RakError *err)
{
  RakValue val = rak_vm_get(fiber, 0);
  rak_vm_push_value(fiber, val, err);
}

static inline void rak_vm_pop(RakFiber *fiber)
{
  RakValue val = rak_vm_get(fiber, 0);
  rak_stack_pop(&fiber->vstk);
  rak_value_release(val);
}

static inline void rak_vm_get_element(RakFiber *fiber, RakError *err)
{
  RakValue val1 = rak_vm_get(fiber, 1);
  RakValue val2 = rak_vm_get(fiber, 0);
  if (rak_is_string(val1))
  {
    RakString *str = rak_as_string(val1);
    if (!rak_is_range(val2))
    {
      rak_error_set(err, "cannot index string with value of type %s",
        rak_type_to_cstr(val2.type));
      return;
    }
    RakRange *range = rak_as_range(val2);
    int start = (int) range->start;
    int end = (int) range->end;
    RakString *_str = rak_string_slice(str, start, end, err);
    if (!rak_is_ok(err)) return;
    RakValue res = rak_string_value(_str);
    rak_vm_set_object(fiber, 1, res);
    rak_vm_pop(fiber);
    return;
  }
  if (rak_is_array(val1))
  {
    RakArray *arr = rak_as_array(val1);
    if (rak_is_number(val2))
    {
      if (!rak_is_integer(val2))
      {
        rak_error_set(err, "cannot index array with non-integer number");
        return;
      }
      int64_t idx = rak_as_integer(val2);
      if (idx < 0 || idx >= rak_array_len(arr))
      {
        rak_error_set(err, "index out of bounds");
        return;
      }
      RakValue res = rak_array_get(arr, (int) idx);
      rak_vm_set_value(fiber, 1, res);
      rak_vm_pop(fiber);
      return;
    }
    if (!rak_is_range(val2))
    {
      rak_error_set(err, "cannot index array with value of type %s",
        rak_type_to_cstr(val2.type));
      return;
    }
    RakRange *range = rak_as_range(val2);
    int start = (int) range->start;
    int end = (int) range->end;
    RakArray *_arr = rak_array_slice(arr, start, end, err);
    if (!rak_is_ok(err)) return;
    RakValue res = rak_array_value(_arr);
    rak_vm_set_object(fiber, 1, res);
    rak_vm_pop(fiber);
    return;
  }
  if (rak_is_range(val1))
  {
    RakRange *range = rak_as_range(val1);
    if (!rak_is_number(val2) || !rak_is_integer(val2))
    {
      rak_error_set(err, "cannot index range with non-integer number");
      return;
    }
    int64_t idx = rak_as_integer(val2);
    if (idx < 0 || idx >= rak_range_len(range))
    {
      rak_error_set(err, "index out of bounds");
      return;
    }
    RakValue res = rak_number_value(rak_range_get(range, (int) idx));
    rak_vm_set_value(fiber, 1, res);
    rak_vm_pop(fiber);
    return;
  }
  if (!rak_is_record(val1))
  {
    rak_error_set(err, "cannot index value of type %s",
      rak_type_to_cstr(val1.type));
    return;
  }
  RakRecord *rec = rak_as_record(val1);
  if (!rak_is_string(val2))
  {
    rak_error_set(err, "cannot index record with value of type %s",
      rak_type_to_cstr(val2.type));
    return;
  }
  RakString *name = rak_as_string(val2);
  int idx = rak_record_index_of(rec, name);
  if (idx == -1)
  {
    rak_error_set(err, "record has no field named '%.*s'",
      rak_string_len(name), rak_string_chars(name));
    return;
  }
  RakValue res = rak_record_get(rec, idx).val;
  rak_vm_set_value(fiber, 1, res);
  rak_vm_pop(fiber);
}

static inline void rak_vm_set_element(RakFiber *fiber, RakError *err)
{
  RakValue val1 = rak_vm_get(fiber, 2);
  RakValue val2 = rak_vm_get(fiber, 1);
  RakValue val3 = rak_vm_get(fiber, 0);
  if (rak_is_array(val1))
  {
    RakArray *arr = rak_as_array(val1);
    if (!rak_is_number(val2) || !rak_is_integer(val2))
    {
      rak_error_set(err, "cannot index array with non-integer number");
      return;
    }
    int64_t idx = rak_as_integer(val2);
    if (idx < 0 || idx >= rak_array_len(arr))
    {
      rak_error_set(err, "index out of bounds");
      return;
    }
    if (rak_is_shared(val1))
    {
      RakArray *_arr = rak_array_set(arr, (int) idx, val3, err);
      if (!rak_is_ok(err)) return;
      RakValue res = rak_array_value(_arr);
      rak_vm_set_object(fiber, 2, res);
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
    rak_error_set(err, "cannot index value of type %s",
      rak_type_to_cstr(val1.type));
    return;
  }
  RakRecord *rec = rak_as_record(val1);
  if (!rak_is_string(val2))
  {
    rak_error_set(err, "cannot index record with value of type %s",
      rak_type_to_cstr(val2.type));
    return;
  }
  RakString *name = rak_as_string(val2);
  if (rak_is_shared(val1))
  {
    RakRecord *_rec = rak_record_put(rec, name, val3, err);
    if (!rak_is_ok(err)) return;
    RakValue res = rak_record_value(_rec);
    rak_vm_set_object(fiber, 2, res);
    rak_value_release(val3);
    fiber->vstk.top -= 2;
    return;
  }
  rak_record_inplace_put(rec, name, val3, err);
  if (!rak_is_ok(err)) return;
  rak_value_release(val3);
  fiber->vstk.top -= 2;
}

static inline void rak_vm_load_element(RakFiber *fiber, RakError *err)
{
  RakValue val1 = rak_vm_get(fiber, 1);
  RakValue val2 = rak_vm_get(fiber, 0);
  if (rak_is_array(val1))
  {
    RakArray *arr = rak_as_array(val1);
    if (!rak_is_number(val2) || !rak_is_integer(val2))
    {
      rak_error_set(err, "cannot index array with non-integer number");
      return;
    }
    int64_t idx = rak_as_integer(val2);
    if (idx < 0 || idx >= rak_array_len(arr))
    {
      rak_error_set(err, "index out of bounds");
      return;
    }
    RakValue res = rak_array_get(arr, (int) idx);
    rak_vm_push_value(fiber, res, err);
    return;
  }
  if (!rak_is_record(val1))
  {
    rak_error_set(err, "cannot index value of type %s", rak_type_to_cstr(val1.type));
    return;
  }
  RakRecord *rec = rak_as_record(val1);
  if (!rak_is_string(val2))
  {
    rak_error_set(err, "cannot index record with value of type %s",
      rak_type_to_cstr(val2.type));
    return;
  }
  RakString *name = rak_as_string(val2);
  int idx = rak_record_index_of(rec, name);
  if (idx == -1)
  {
    rak_error_set(err, "record has no field named '%.*s'",
      rak_string_len(name), rak_string_chars(name));
    return;
  }
  rak_stack_set(&fiber->vstk, 0, rak_number_value(idx));
  rak_string_release(name);
  RakValue res = rak_record_get(rec, idx).val;
  rak_vm_push_value(fiber, res, err);
}

static inline void rak_vm_fetch_element(RakFiber *fiber, RakError *err)
{
  RakValue val1 = rak_vm_get(fiber, 1);
  RakValue val2 = rak_vm_get(fiber, 0);
  if (rak_is_array(val1))
  {
    RakArray *arr = rak_as_array(val1);
    if (!rak_is_number(val2) || !rak_is_integer(val2))
    {
      rak_error_set(err, "cannot index array with non-integer number");
      return;
    }
    int64_t idx = rak_as_integer(val2);
    if (idx < 0 || idx >= rak_array_len(arr))
    {
      rak_error_set(err, "index out of bounds");
      return;
    }
    RakValue res = rak_array_get(arr, (int) idx);
    if (rak_is_shared(val1) || (rak_is_object(res) && rak_as_object(res)->refCount > 1))
      res.flags |= RAK_FLAG_SHARED;
    rak_vm_push_value(fiber, res, err);
    return;
  }
  if (!rak_is_record(val1))
  {
    rak_error_set(err, "cannot index value of type %s", rak_type_to_cstr(val1.type));
    return;
  }
  RakRecord *rec = rak_as_record(val1);
  if (!rak_is_string(val2))
  {
    rak_error_set(err, "cannot index record with value of type %s",
      rak_type_to_cstr(val2.type));
    return;
  }
  RakString *name = rak_as_string(val2);
  int idx = rak_record_index_of(rec, name);
  if (idx == -1)
  {
    rak_error_set(err, "record has no field named '%.*s'",
      rak_string_len(name), rak_string_chars(name));
    return;
  }
  rak_stack_set(&fiber->vstk, 0, rak_number_value(idx));
  rak_string_release(name);
  RakValue res = rak_record_get(rec, idx).val;
  if (rak_is_shared(val1) || (rak_is_object(res) && rak_as_object(res)->refCount > 1))
    res.flags |= RAK_FLAG_SHARED;
  rak_vm_push_value(fiber, res, err);
}

static inline void rak_vm_update_element(RakFiber *fiber, RakError *err)
{
  RakValue val1 = rak_vm_get(fiber, 2);
  RakValue val2 = rak_vm_get(fiber, 1);
  RakValue val3 = rak_vm_get(fiber, 0);
  int idx = (int) rak_as_number(val2);
  if (rak_is_array(val1))
  {
    RakArray *arr = rak_as_array(val1);
    if (rak_is_shared(val1))
    {
      RakArray *_arr = rak_array_set(arr, idx, val3, err);
      if (!rak_is_ok(err)) return;
      RakValue res = rak_array_value(_arr);
      rak_vm_set_object(fiber, 2, res);
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
    rak_vm_set_object(fiber, 2, res);
    rak_value_release(val3);
    fiber->vstk.top -= 2;
    return;
  }
  rak_record_inplace_set(rec, idx, val3);
  rak_value_release(val3);
  fiber->vstk.top -= 2;
}

static inline void rak_vm_get_field(RakFiber *fiber, RakChunk *chunk, uint8_t idx, RakError *err)
{
  RakValue val = rak_vm_get(fiber, 0);
  if (!rak_is_record(val))
  {
    rak_error_set(err, "cannot index value of type %s",
      rak_type_to_cstr(val.type));
    return;
  }
  RakRecord *rec = rak_as_record(val);
  RakString *name = rak_as_string(rak_slice_get(&chunk->consts, idx));
  int _idx = rak_record_index_of(rec, name);
  if (_idx == -1)
  {
    rak_error_set(err, "record has no field named '%.*s'",
      rak_string_len(name), rak_string_chars(name));
    return;
  }
  RakValue res = rak_record_get(rec, _idx).val;
  rak_vm_set_value(fiber, 0, res);
}

static inline void rak_vm_put_field(RakFiber *fiber, RakChunk *chunk, uint8_t idx, RakError *err)
{
  RakValue val1 = rak_vm_get(fiber, 1);
  RakValue val2 = rak_vm_get(fiber, 0);
  if (!rak_is_record(val1))
  {
    rak_error_set(err, "cannot index value of type %s",
      rak_type_to_cstr(val1.type));
    return;
  }
  RakRecord *rec = rak_as_record(val1);
  RakString *name = rak_as_string(rak_slice_get(&chunk->consts, idx));
  if (rak_is_shared(val1))
  {
    RakRecord *_rec = rak_record_put(rec, name, val2, err);
    if (!rak_is_ok(err)) return;
    RakValue res = rak_record_value(_rec);
    rak_vm_set_object(fiber, 1, res);
    rak_value_release(val2);
    rak_stack_pop(&fiber->vstk);
    return;
  }
  rak_record_inplace_put(rec, name, val2, err);
  if (!rak_is_ok(err)) return;
  rak_value_release(val2);
  rak_stack_pop(&fiber->vstk);
}

static inline void rak_vm_load_field(RakFiber *fiber, RakChunk *chunk, uint8_t idx, RakError *err)
{
  RakValue val = rak_vm_get(fiber, 0);
  if (!rak_is_record(val))
  {
    rak_error_set(err, "cannot index value of type %s",
      rak_type_to_cstr(val.type));
    return;
  }
  RakRecord *rec = rak_as_record(val);
  RakString *name = rak_as_string(rak_slice_get(&chunk->consts, idx));
  int _idx = rak_record_index_of(rec, name);
  if (_idx == -1)
  {
    rak_error_set(err, "record has no field named '%.*s'",
      rak_string_len(name), rak_string_chars(name));
    return;
  }
  rak_vm_push(fiber, rak_number_value(_idx), err);
  if (!rak_is_ok(err)) return;
  RakValue res = rak_record_get(rec, _idx).val;
  rak_vm_push_value(fiber, res, err);
}

static inline void rak_vm_fetch_field(RakFiber *fiber, RakChunk *chunk, uint8_t idx, RakError *err)
{
  RakValue val = rak_vm_get(fiber, 0);
  if (!rak_is_record(val))
  {
    rak_error_set(err, "cannot index value of type %s",
      rak_type_to_cstr(val.type));
    return;
  }
  RakRecord *rec = rak_as_record(val);
  RakString *name = rak_as_string(rak_slice_get(&chunk->consts, idx));
  int _idx = rak_record_index_of(rec, name);
  if (_idx == -1)
  {
    rak_error_set(err, "record has no field named '%.*s'",
      rak_string_len(name), rak_string_chars(name));
    return;
  }
  rak_vm_push(fiber, rak_number_value(_idx), err);
  if (!rak_is_ok(err)) return;
  RakValue res = rak_record_get(rec, _idx).val;
  if (rak_is_shared(val) || (rak_is_object(res) && rak_as_object(res)->refCount > 1))
    res.flags |= RAK_FLAG_SHARED;
  rak_vm_push_value(fiber, res, err);
}

static inline void rak_vm_update_field(RakFiber *fiber, RakError *err)
{
  RakValue val1 = rak_vm_get(fiber, 2);
  RakValue val2 = rak_vm_get(fiber, 1);
  RakValue val3 = rak_vm_get(fiber, 0);
  RakRecord *rec = rak_as_record(val1);
  int idx = (int) rak_as_number(val2);
  if (rak_is_shared(val1))
  {
    RakRecord *_rec = rak_record_set(rec, idx, val3, err);
    if (!rak_is_ok(err)) return;
    RakValue res = rak_record_value(_rec);
    rak_vm_set_object(fiber, 2, res);
    rak_value_release(val3);
    fiber->vstk.top -= 2;
    return;
  }
  rak_record_inplace_set(rec, idx, val3);
  rak_value_release(val3);
  fiber->vstk.top -= 2;
}

static inline void rak_vm_unpack_elements(RakFiber *fiber, uint8_t n, RakError *err)
{
  RakValue *slots = &rak_stack_get(&fiber->vstk, n);
  RakValue val = slots[0];
  if (!rak_is_array(val))
  {
    rak_error_set(err, "cannot unpack elements from value of type %s",
      rak_type_to_cstr(val.type));
    return;
  }
  RakArray *arr = rak_as_array(val);
  int len = rak_array_len(arr);
  for (int i = 0; i < n && i < len; ++i)
  {
    int idx = (int) rak_as_integer(slots[i + 1]);
    RakValue _val = rak_array_get(arr, idx);
    slots[i] = _val;
    rak_value_retain(_val);
  }
  for (int i = len; i < n; ++i)
    slots[i] = rak_nil_value();
  rak_stack_pop(&fiber->vstk);
  rak_array_release(arr);
}

static inline void rak_vm_unpack_fields(RakFiber *fiber, uint8_t n, RakError *err)
{
  RakValue *slots = &rak_stack_get(&fiber->vstk, n);
  RakValue val = slots[0];
  if (!rak_is_record(val))
  {
    rak_error_set(err, "cannot unpack fields from value of type %s",
      rak_type_to_cstr(val.type));
    return;
  }
  RakRecord *rec = rak_as_record(val);
  for (int i = 0; i < n; ++i)
  {
    RakString *name = rak_as_string(slots[i + 1]);
    int idx = rak_record_index_of(rec, name);
    if (idx == -1)
    {
      rak_error_set(err, "record has no field named '%.*s'",
        rak_string_len(name), rak_string_chars(name));
      slots[i] = rak_nil_value();
      rak_string_release(name);
      continue;
    }
    RakValue _val = rak_record_get(rec, idx).val;
    slots[i] = _val;
    rak_value_retain(_val);
    rak_string_release(name);
  }
  rak_stack_pop(&fiber->vstk);
  rak_record_release(rec);
}

static inline RakValue rak_vm_get(RakFiber *fiber, uint8_t idx)
{
  return rak_stack_get(&fiber->vstk, idx);
}

static inline void rak_vm_set(RakFiber *fiber, uint8_t idx, RakValue val)
{
  RakValue _val = rak_stack_get(&fiber->vstk, idx);
  rak_value_release(_val);
  rak_stack_set(&fiber->vstk, idx, val);
}

static inline void rak_vm_set_value(RakFiber *fiber, uint8_t idx, RakValue val)
{
  rak_value_retain(val);
  rak_vm_set(fiber, idx, val);
}

static inline void rak_vm_set_object(RakFiber *fiber, uint8_t idx, RakValue val)
{
  rak_object_retain(rak_as_object(val));
  rak_vm_set(fiber, idx, val);
}

static inline void rak_vm_eq(RakFiber *fiber)
{
  RakValue val1 = rak_vm_get(fiber, 1);
  RakValue val2 = rak_vm_get(fiber, 0);
  RakValue res = rak_bool_value(rak_value_equals(val1, val2));
  rak_vm_set(fiber, 1, res);
  rak_vm_pop(fiber);
}

static inline void rak_vm_gt(RakFiber *fiber, RakError *err)
{
  RakValue val1 = rak_vm_get(fiber, 1);
  RakValue val2 = rak_vm_get(fiber, 0);
  int cmp = rak_value_compare(val1, val2, err);
  if (!rak_is_ok(err)) return;
  RakValue res = rak_bool_value(cmp > 0);
  rak_vm_set(fiber, 1, res);
  rak_vm_pop(fiber);
}

static inline void rak_vm_lt(RakFiber *fiber, RakError *err)
{
  RakValue val1 = rak_vm_get(fiber, 1);
  RakValue val2 = rak_vm_get(fiber, 0);
  int cmp = rak_value_compare(val1, val2, err);
  if (!rak_is_ok(err)) return;
  RakValue res = rak_bool_value(cmp < 0);
  rak_vm_set(fiber, 1, res);
  rak_vm_pop(fiber);
}

static inline void rak_vm_add(RakFiber *fiber, RakError *err)
{
  RakValue val1 = rak_vm_get(fiber, 1);
  RakValue val2 = rak_vm_get(fiber, 0);
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
    rak_vm_set(fiber, 1, res);
    rak_vm_pop(fiber);
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
    rak_vm_set_object(fiber, 1, res);
    rak_vm_pop(fiber);
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
    rak_vm_set_object(fiber, 1, res);
    rak_vm_pop(fiber);
    return;
  }
  rak_error_set(err, "cannot add %s and %s", rak_type_to_cstr(val1.type),
    rak_type_to_cstr(val2.type));
}

static inline void rak_vm_sub(RakFiber *fiber, RakError *err)
{
  RakValue val1 = rak_vm_get(fiber, 1);
  RakValue val2 = rak_vm_get(fiber, 0);
  if (!rak_is_number(val1) || !rak_is_number(val2))
  {
    rak_error_set(err, "cannot subtract non-number values");
    return;
  }
  double num1 = rak_as_number(val1);
  double num2 = rak_as_number(val2);
  RakValue res = rak_number_value(num1 - num2);
  rak_vm_set(fiber, 1, res);
  rak_vm_pop(fiber);
}

static inline void rak_vm_mul(RakFiber *fiber, RakError *err)
{
  RakValue val1 = rak_vm_get(fiber, 1);
  RakValue val2 = rak_vm_get(fiber, 0);
  if (!rak_is_number(val1) || !rak_is_number(val2))
  {
    rak_error_set(err, "cannot multiply non-number values");
    return;
  }
  double num1 = rak_as_number(val1);
  double num2 = rak_as_number(val2);
  RakValue res = rak_number_value(num1 * num2);
  rak_vm_set(fiber, 1, res);
  rak_vm_pop(fiber);
}

static inline void rak_vm_div(RakFiber *fiber, RakError *err)
{
  RakValue val1 = rak_vm_get(fiber, 1);
  RakValue val2 = rak_vm_get(fiber, 0);
  if (!rak_is_number(val1) || !rak_is_number(val2))
  {
    rak_error_set(err, "cannot divide non-number values");
    return;
  }
  double num1 = rak_as_number(val1);
  double num2 = rak_as_number(val2);
  RakValue res = rak_number_value(num1 / num2);
  rak_vm_set(fiber, 1, res);
  rak_vm_pop(fiber);
}

static inline void rak_vm_mod(RakFiber *fiber, RakError *err)
{
  RakValue val1 = rak_vm_get(fiber, 1);
  RakValue val2 = rak_vm_get(fiber, 0);
  if (!rak_is_number(val1) || !rak_is_number(val2))
  {
    rak_error_set(err, "cannot calculate modulo of non-number values");
    return;
  }
  double num1 = rak_as_number(val1);
  double num2 = rak_as_number(val2);
  RakValue res = rak_number_value(fmod(num1, num2));
  rak_vm_set(fiber, 1, res);
  rak_vm_pop(fiber);
}

static inline void rak_vm_not(RakFiber *fiber)
{
  RakValue val = rak_vm_get(fiber, 0);
  RakValue res = rak_is_falsy(val) ? rak_bool_value(true) : rak_bool_value(false);
  rak_vm_set(fiber, 0, res);
}

static inline void rak_vm_neg(RakFiber *fiber, RakError *err)
{
  RakValue val = rak_vm_get(fiber, 0);
  if (!rak_is_number(val))
  {
    rak_error_set(err, "cannot negate a non-number value");
    return;
  }
  double num = rak_as_number(val);
  RakValue res = rak_number_value(-num);
  rak_vm_set(fiber, 0, res);
}

static inline void rak_vm_call(RakFiber *fiber, uint8_t nargs, RakError *err)
{
  RakValue *slots = &rak_stack_get(&fiber->vstk, nargs);
  RakValue val = slots[0];
  if (!rak_is_closure(val))
  {
    rak_error_set(err, "cannot call non-closure value");
    return;
  }
  if (rak_stack_is_full(&fiber->cstk))
  {
    rak_error_set(err, "too many nested calls");
    return;
  }
  RakClosure *cl = rak_as_closure(val);
  int arity = cl->callable->arity;
  while (nargs > arity)
  {
    rak_vm_pop(fiber);
    --nargs;
  }
  while (nargs < arity)
  {
    rak_vm_push_nil(fiber, err);
    if (!rak_is_ok(err)) return;
    ++nargs;
  }
  RakCallFrame frame = {
    .cl = cl,
    .slots = slots
  };
  if (cl->type == RAK_CALLABLE_TYPE_FUNCTION)
  {
    RakFunction *fn = (RakFunction *) cl->callable;
    frame.ip = fn->chunk.instrs.data;
    rak_stack_push(&fiber->cstk, frame);
    return;
  }
  frame.state = 0;
  rak_stack_push(&fiber->cstk, frame);
}

static inline void rak_vm_tail_call(RakFiber *fiber, RakValue *slots, uint8_t nargs, RakError *err)
{
  RakValue *_slots = &rak_stack_get(&fiber->vstk, nargs);
  RakValue val = _slots[0];
  if (!rak_is_closure(val))
  {
    rak_error_set(err, "cannot call non-closure value");
    return;
  }
  RakClosure *cl = rak_as_closure(val);
  int arity = cl->callable->arity;
  while (nargs > arity)
  {
    rak_vm_pop(fiber);
    --nargs;
  }
  while (nargs < arity)
  {
    rak_vm_push_nil(fiber, err);
    if (!rak_is_ok(err)) return;
    ++nargs;
  }
  rak_closure_release(rak_as_closure(slots[0]));
  slots[0] = _slots[0];
  for (int i = 1; i <= arity; ++i)
  {
    RakValue _val = _slots[i];
    rak_value_release(slots[i]);
    slots[i] = _val;
  }
  fiber->vstk.top = &slots[arity];
  RakCallFrame *frame = &rak_stack_get(&fiber->cstk, 0);
  frame->cl = cl;
  if (cl->type == RAK_CALLABLE_TYPE_FUNCTION)
  {
    RakFunction *fn = (RakFunction *) cl->callable;
    frame->ip = fn->chunk.instrs.data;
    return;
  }
  frame->state = 0;
}

static inline void rak_vm_yield(RakFiber *fiber)
{
  fiber->status = RAK_FIBER_STATUS_SUSPENDED;
}

static inline void rak_vm_return(RakFiber *fiber, RakClosure *cl, RakValue *slots)
{
  slots[0] = rak_vm_get(fiber, 0);
  rak_closure_release(cl);
  rak_stack_pop(&fiber->vstk);
  while (fiber->vstk.top > slots)
    rak_vm_pop(fiber);
  rak_stack_pop(&fiber->cstk);
}

#endif // RAK_VM_H
