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
#include "array.h"
#include "chunk.h"
#include "closure.h"
#include "range.h"
#include "record.h"
#include "stack.h"

#define RAK_VM_VSTK_DEFAULT_SIZE (1024)

typedef struct RakVM
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
static inline void rak_vm_load_global(RakVM *vm, uint8_t idx, RakError *err);
static inline void rak_vm_load_local(RakVM *vm, RakValue *slots, uint8_t idx, RakError *err);
static inline void rak_vm_store_local(RakVM *vm, RakValue *slots, uint8_t idx);
static inline void rak_vm_fetch_local(RakVM *vm, RakValue *slots, uint8_t idx, RakError *err);
static inline void rak_vm_new_array(RakVM *vm, uint8_t len, RakError *err);
static inline void rak_vm_new_range(RakVM *vm, RakError *err);
static inline void rak_vm_new_record(RakVM *vm, uint8_t len, RakError *err);
static inline void rak_vm_dup(RakVM *vm, RakError *err);
static inline void rak_vm_pop(RakVM *vm);
static inline void rak_vm_get_element(RakVM *vm, RakError *err);
static inline void rak_vm_set_element(RakVM *vm, RakError *err);
static inline void rak_vm_fetch_element(RakVM *vm, RakError *err);
static inline void rak_vm_update_element(RakVM *vm, RakError *err);
static inline void rak_vm_get_field(RakVM *vm, RakChunk *chunk, uint8_t idx, RakError *err);
static inline void rak_vm_put_field(RakVM *vm, RakChunk *chunk, uint8_t idx, RakError *err);
static inline void rak_vm_fetch_field(RakVM *vm, RakChunk *chunk, uint8_t idx, RakError *err);
static inline void rak_vm_update_field(RakVM *vm, RakError *err);
static inline void rak_vm_unpack_elements(RakVM *vm, uint8_t n, RakError *err);
static inline void rak_vm_unpack_fields(RakVM *vm, uint8_t n, RakError *err);
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
static inline void rak_vm_call(RakVM *vm, uint8_t nargs, RakError *err);

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

static inline void rak_vm_load_global(RakVM *vm, uint8_t idx, RakError *err)
{
  RakValue val = vm->vstk.base[idx];
  rak_vm_push_value(vm, val, err);
}

static inline void rak_vm_load_local(RakVM *vm, RakValue *slots, uint8_t idx, RakError *err)
{
  RakValue val = slots[idx];
  rak_vm_push_value(vm, val, err);
}

static inline void rak_vm_store_local(RakVM *vm, RakValue *slots, uint8_t idx)
{
  RakValue val = rak_vm_get(vm, 0);
  rak_value_release(slots[idx]);
  slots[idx] = val;
  --vm->vstk.top;
}

static inline void rak_vm_fetch_local(RakVM *vm, RakValue *slots, uint8_t idx, RakError *err)
{
  RakValue val = slots[idx];
  if (rak_is_object(val) && rak_as_object(val)->refCount > 1)
    val.flags |= RAK_FLAG_SHARED;
  rak_vm_push_value(vm, val, err);
}

static inline void rak_vm_new_array(RakVM *vm, uint8_t len, RakError *err)
{
  RakArray *arr = rak_array_new_with_capacity(len, err);
  if (!len)
  {
    RakValue res = rak_array_value(arr);
    rak_vm_push_object(vm, res, err);
    return;
  }
  int n = len - 1;
  RakValue *slots = &rak_stack_get(&vm->vstk, n);
  if (!rak_is_ok(err)) return;
  for (int i = 0; i < len; ++i)
    rak_slice_set(&arr->slice, i, slots[i]);
  arr->slice.len = len;
  slots[0] = rak_array_value(arr);
  rak_object_retain(&arr->obj);
  vm->vstk.top -= n;
}

static inline void rak_vm_new_range(RakVM *vm, RakError *err)
{
  RakValue val1 = rak_vm_get(vm, 1);
  RakValue val2 = rak_vm_get(vm, 0);
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
  rak_stack_set(&vm->vstk, 1, res);
  rak_object_retain(&range->obj);
  rak_vm_pop(vm);
}

static inline void rak_vm_new_record(RakVM *vm, uint8_t len, RakError *err)
{
  RakRecord *rec = rak_record_new_with_capacity(len, err);
  if (!len)
  {
    RakValue res = rak_record_value(rec);
    rak_vm_push_object(vm, res, err);
    return;
  }
  int n = (len << 1) - 1;
  RakValue *slots = &rak_stack_get(&vm->vstk, n);
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
  vm->vstk.top -= n;
}

static inline void rak_vm_dup(RakVM *vm, RakError *err)
{
  RakValue val = rak_vm_get(vm, 0);
  rak_vm_push_value(vm, val, err);
}

static inline void rak_vm_pop(RakVM *vm)
{
  RakValue val = rak_vm_get(vm, 0);
  rak_stack_pop(&vm->vstk);
  rak_value_release(val);
}

static inline void rak_vm_get_element(RakVM *vm, RakError *err)
{
  RakValue val1 = rak_vm_get(vm, 1);
  RakValue val2 = rak_vm_get(vm, 0);
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
    rak_vm_set_object(vm, 1, res);
    rak_vm_pop(vm);
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
      rak_vm_set_value(vm, 1, res);
      rak_vm_pop(vm);
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
    rak_vm_set_object(vm, 1, res);
    rak_vm_pop(vm);
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
    rak_vm_set_value(vm, 1, res);
    rak_vm_pop(vm);
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
  rak_vm_set_value(vm, 1, res);
  rak_vm_pop(vm);
}

static inline void rak_vm_set_element(RakVM *vm, RakError *err)
{
  RakValue val1 = rak_vm_get(vm, 2);
  RakValue val2 = rak_vm_get(vm, 1);
  RakValue val3 = rak_vm_get(vm, 0);
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
      rak_vm_set_object(vm, 2, res);
      rak_value_release(val3);
      vm->vstk.top -= 2;
      return;
    }
    rak_array_inplace_set(arr, (int) idx, val3);
    rak_value_release(val3);
    vm->vstk.top -= 2;
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
    rak_vm_set_object(vm, 2, res);
    rak_value_release(val3);
    vm->vstk.top -= 2;
    return;
  }
  rak_record_inplace_put(rec, name, val3, err);
  if (!rak_is_ok(err)) return;
  rak_value_release(val3);
  vm->vstk.top -= 2;
}

static inline void rak_vm_fetch_element(RakVM *vm, RakError *err)
{
  RakValue val1 = rak_vm_get(vm, 1);
  RakValue val2 = rak_vm_get(vm, 0);
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
    rak_vm_push_value(vm, res, err);
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
  rak_stack_set(&vm->vstk, 0, rak_number_value(idx));
  rak_string_release(name);
  RakValue res = rak_record_get(rec, idx).val;
  if (rak_is_shared(val1) || (rak_is_object(res) && rak_as_object(res)->refCount > 1))
    res.flags |= RAK_FLAG_SHARED;
  rak_vm_push_value(vm, res, err);
}

static inline void rak_vm_update_element(RakVM *vm, RakError *err)
{
  RakValue val1 = rak_vm_get(vm, 2);
  RakValue val2 = rak_vm_get(vm, 1);
  RakValue val3 = rak_vm_get(vm, 0);
  int idx = (int) rak_as_number(val2);
  if (rak_is_array(val1))
  {
    RakArray *arr = rak_as_array(val1);
    if (rak_is_shared(val1))
    {
      RakArray *_arr = rak_array_set(arr, idx, val3, err);
      if (!rak_is_ok(err)) return;
      RakValue res = rak_array_value(_arr);
      rak_vm_set_object(vm, 2, res);
      rak_value_release(val3);
      vm->vstk.top -= 2;
      return;
    }
    rak_array_inplace_set(arr, idx, val3);
    rak_value_release(val3);
    vm->vstk.top -= 2;
    return;
  }
  RakRecord *rec = rak_as_record(val1);
  if (rak_is_shared(val1))
  {
    RakRecord *_rec = rak_record_set(rec, idx, val3, err);
    if (!rak_is_ok(err)) return;
    RakValue res = rak_record_value(_rec);
    rak_vm_set_object(vm, 2, res);
    rak_value_release(val3);
    vm->vstk.top -= 2;
    return;
  }
  rak_record_inplace_set(rec, idx, val3);
  rak_value_release(val3);
  vm->vstk.top -= 2;
}

static inline void rak_vm_get_field(RakVM *vm, RakChunk *chunk, uint8_t idx, RakError *err)
{
  RakValue val = rak_vm_get(vm, 0);
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
  rak_vm_set_value(vm, 0, res);
}

static inline void rak_vm_put_field(RakVM *vm, RakChunk *chunk, uint8_t idx, RakError *err)
{
  RakValue val1 = rak_vm_get(vm, 1);
  RakValue val2 = rak_vm_get(vm, 0);
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
    rak_vm_set_object(vm, 1, res);
    rak_value_release(val2);
    --vm->vstk.top;
    return;
  }
  rak_record_inplace_put(rec, name, val2, err);
  if (!rak_is_ok(err)) return;
  rak_value_release(val2);
  --vm->vstk.top;
}

static inline void rak_vm_fetch_field(RakVM *vm, RakChunk *chunk, uint8_t idx, RakError *err)
{
  RakValue val = rak_vm_get(vm, 0);
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
  rak_vm_push(vm, rak_number_value(_idx), err);
  if (!rak_is_ok(err)) return;
  RakValue res = rak_record_get(rec, _idx).val;
  if (rak_is_shared(val) || (rak_is_object(res) && rak_as_object(res)->refCount > 1))
    res.flags |= RAK_FLAG_SHARED;
  rak_vm_push_value(vm, res, err);
}

static inline void rak_vm_update_field(RakVM *vm, RakError *err)
{
  RakValue val1 = rak_vm_get(vm, 2);
  RakValue val2 = rak_vm_get(vm, 1);
  RakValue val3 = rak_vm_get(vm, 0);
  RakRecord *rec = rak_as_record(val1);
  int idx = (int) rak_as_number(val2);
  if (rak_is_shared(val1))
  {
    RakRecord *_rec = rak_record_set(rec, idx, val3, err);
    if (!rak_is_ok(err)) return;
    RakValue res = rak_record_value(_rec);
    rak_vm_set_object(vm, 2, res);
    rak_value_release(val3);
    vm->vstk.top -= 2;
    return;
  }
  rak_record_inplace_set(rec, idx, val3);
  rak_value_release(val3);
  vm->vstk.top -= 2;
}

static inline void rak_vm_unpack_elements(RakVM *vm, uint8_t n, RakError *err)
{
  RakValue *slots = &rak_stack_get(&vm->vstk, n);
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
  --vm->vstk.top;
  rak_array_release(arr);
}

static inline void rak_vm_unpack_fields(RakVM *vm, uint8_t n, RakError *err)
{
  RakValue *slots = &rak_stack_get(&vm->vstk, n);
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
      return;
    }
    RakValue _val = rak_record_get(rec, idx).val;
    slots[i] = _val;
    rak_value_retain(_val);
  }
  --vm->vstk.top;
  rak_record_release(rec);
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

static inline void rak_vm_call(RakVM *vm, uint8_t nargs, RakError *err)
{
  RakValue *slots = &rak_stack_get(&vm->vstk, nargs);
  RakValue val = slots[0];
  if (!rak_is_closure(val))
  {
    rak_error_set(err, "cannot call non-closure value");
    return;
  }
  RakClosure *closure = rak_as_closure(val);
  int arity = closure->arity;
  while (nargs > arity)
  {
    rak_vm_pop(vm);
    --nargs;
  }
  while (nargs < arity)
  {
    rak_vm_push_nil(vm, err);
    if (!rak_is_ok(err)) return;
    ++nargs;
  }
  RakNativeFunction native = closure->as.native;
  native(vm, slots, err);
  if (!rak_is_ok(err)) return;
  rak_closure_release(closure);
  slots[0] = rak_vm_get(vm, 0);
  rak_stack_pop(&vm->vstk);
  while (vm->vstk.top > slots)
    rak_vm_pop(vm);
}

#endif // RAK_VM_H
