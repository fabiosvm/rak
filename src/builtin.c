//
// builtin.c
//
// Copyright 2025 Fábio de Souza Villaça Medeiros
//
// This file is part of the Rak Project.
// For detailed license information, please refer to the LICENSE file
// located in the root directory of this project.
//

#include "rak/builtin.h"
#include <float.h>
#include <stdio.h>
#include <string.h>

static const char *globals[] = {
  "TYPE_NIL",
  "TYPE_BOOL",
  "TYPE_NUMBER",
  "TYPE_STRING",
  "TYPE_ARRAY",
  "TYPE_RANGE",
  "TYPE_RECORD",
  "TYPE_CLOSURE",
  "NUMBER_EPSILON",
  "NUMBER_MIN",
  "NUMBER_MAX",
  "INTEGER_MIN",
  "INTEGER_MAX",
  "type",
  "is_nil",
  "is_bool",
  "is_number",
  "is_integer",
  "is_string",
  "is_array",
  "is_range",
  "is_record",
  "is_closure",
  "is_falsy",
  "is_object",
  "ptr",
  "ref_count",
  "make_array",
  "append",
  "cap",
  "len",
  "is_empty",
  "print",
  "println"
};

static inline void push_builtin_function(RakVM *vm, const char *name, int arity,
  RakNativeFunctionCall call, RakError *err);

static void type_native_call(RakVM *vm, RakClosure *cl, RakValue *slots, RakError *err);
static void is_nil_native_call(RakVM *vm, RakClosure *cl, RakValue *slots, RakError *err);
static void is_bool_native_call(RakVM *vm, RakClosure *cl, RakValue *slots, RakError *err);
static void is_number_native_call(RakVM *vm, RakClosure *cl, RakValue *slots, RakError *err);
static void is_integer_native_call(RakVM *vm, RakClosure *cl, RakValue *slots, RakError *err);
static void is_string_native_call(RakVM *vm, RakClosure *cl, RakValue *slots, RakError *err);
static void is_array_native_call(RakVM *vm, RakClosure *cl, RakValue *slots, RakError *err);
static void is_range_native_call(RakVM *vm, RakClosure *cl, RakValue *slots, RakError *err);
static void is_record_native_call(RakVM *vm, RakClosure *cl, RakValue *slots, RakError *err);
static void is_closure_native_call(RakVM *vm, RakClosure *cl, RakValue *slots, RakError *err);
static void is_falsy_native_call(RakVM *vm, RakClosure *cl, RakValue *slots, RakError *err);
static void is_object_native_call(RakVM *vm, RakClosure *cl, RakValue *slots, RakError *err);
static void ptr_native_call(RakVM *vm, RakClosure *cl, RakValue *slots, RakError *err);
static void ref_count_native_call(RakVM *vm, RakClosure *cl, RakValue *slots, RakError *err);
static void make_array_native_call(RakVM *vm, RakClosure *cl, RakValue *slots, RakError *err);
static void append_native_call(RakVM *vm, RakClosure *cl, RakValue *slots, RakError *err);
static void cap_native_call(RakVM *vm, RakClosure *cl, RakValue *slots, RakError *err);
static void len_native_call(RakVM *vm, RakClosure *cl, RakValue *slots, RakError *err);
static void is_empty_native_call(RakVM *vm, RakClosure *cl, RakValue *slots, RakError *err);
static void print_native_call(RakVM *vm, RakClosure *cl, RakValue *slots, RakError *err);
static void println_native_call(RakVM *vm, RakClosure *cl, RakValue *slots, RakError *err);

static inline void push_builtin_function(RakVM *vm, const char *name, int arity,
  RakNativeFunctionCall call, RakError *err)
{
  RakString *_name = rak_string_new_from_cstr(-1, name, err);
  if (!rak_is_ok(err)) return;
  RakNativeFunction *native = rak_native_function_new(_name, arity, call, err);
  if (!rak_is_ok(err))
  {
    rak_string_free(_name);
    return;
  }
  RakClosure *cl = rak_closure_new(RAK_CALLABLE_TYPE_NATIVE_FUNCTION, &native->callable, err);
  if (!rak_is_ok(err))
  {
    rak_native_function_free(native);
    return;
  }
  rak_vm_push_object(vm, rak_closure_value(cl), err);
  if (rak_is_ok(err)) return;
  rak_closure_free(cl);
}

static void type_native_call(RakVM *vm, RakClosure *cl, RakValue *slots, RakError *err)
{
  RakValue val = slots[1];
  rak_vm_push_number(vm, val.type, err);
  if (!rak_is_ok(err)) return;
  rak_vm_return(vm, cl, slots);
}

static void is_nil_native_call(RakVM *vm, RakClosure *cl, RakValue *slots, RakError *err)
{
  RakValue val = slots[1];
  rak_vm_push_bool(vm, (bool) rak_is_nil(val), err);
  if (!rak_is_ok(err)) return;
  rak_vm_return(vm, cl, slots);
}

static void is_bool_native_call(RakVM *vm, RakClosure *cl, RakValue *slots, RakError *err)
{
  RakValue val = slots[1];
  rak_vm_push_bool(vm, (bool) rak_is_bool(val), err);
  if (!rak_is_ok(err)) return;
  rak_vm_return(vm, cl, slots);
}

static void is_number_native_call(RakVM *vm, RakClosure *cl, RakValue *slots, RakError *err)
{
  RakValue val = slots[1];
  rak_vm_push_bool(vm, (bool) rak_is_number(val), err);
  if (!rak_is_ok(err)) return;
  rak_vm_return(vm, cl, slots);
}

static void is_integer_native_call(RakVM *vm, RakClosure *cl, RakValue *slots, RakError *err)
{
  RakValue val = slots[1];
  bool data = (bool) (rak_is_number(val) && rak_is_integer(val));
  rak_vm_push_bool(vm, data, err);
  if (!rak_is_ok(err)) return;
  rak_vm_return(vm, cl, slots);
}

static void is_string_native_call(RakVM *vm, RakClosure *cl, RakValue *slots, RakError *err)
{
  RakValue val = slots[1];
  rak_vm_push_bool(vm, (bool) rak_is_string(val), err);
  if (!rak_is_ok(err)) return;
  rak_vm_return(vm, cl, slots);
}

static void is_array_native_call(RakVM *vm, RakClosure *cl, RakValue *slots, RakError *err)
{
  RakValue val = slots[1];
  rak_vm_push_bool(vm, (bool) rak_is_array(val), err);
  if (!rak_is_ok(err)) return;
  rak_vm_return(vm, cl, slots);
}

static void is_range_native_call(RakVM *vm, RakClosure *cl, RakValue *slots, RakError *err)
{
  RakValue val = slots[1];
  rak_vm_push_bool(vm, (bool) rak_is_range(val), err);
  if (!rak_is_ok(err)) return;
  rak_vm_return(vm, cl, slots);
}

static void is_record_native_call(RakVM *vm, RakClosure *cl, RakValue *slots, RakError *err)
{
  RakValue val = slots[1];
  rak_vm_push_bool(vm, (bool) rak_is_record(val), err);
  if (!rak_is_ok(err)) return;
  rak_vm_return(vm, cl, slots);
}

static void is_closure_native_call(RakVM *vm, RakClosure *cl, RakValue *slots, RakError *err)
{
  RakValue val = slots[1];
  rak_vm_push_bool(vm, (bool) rak_is_closure(val), err);
  if (!rak_is_ok(err)) return;
  rak_vm_return(vm, cl, slots);
}

static void is_falsy_native_call(RakVM *vm, RakClosure *cl, RakValue *slots, RakError *err)
{
  RakValue val = slots[1];
  rak_vm_push_bool(vm, (bool) rak_is_falsy(val), err);
  if (!rak_is_ok(err)) return;
  rak_vm_return(vm, cl, slots);
}

static void is_object_native_call(RakVM *vm, RakClosure *cl, RakValue *slots, RakError *err)
{
  RakValue val = slots[1];
  rak_vm_push_bool(vm, (bool) rak_is_object(val), err);
  if (!rak_is_ok(err)) return;
  rak_vm_return(vm, cl, slots);
}

static void ptr_native_call(RakVM *vm, RakClosure *cl, RakValue *slots, RakError *err)
{
  RakValue val = slots[1];
  void *ptr = rak_is_object(val) ? val.opaque.ptr : NULL;
  rak_vm_push_number(vm, (double) (uintptr_t) ptr, err);
  if (!rak_is_ok(err)) return;
  rak_vm_return(vm, cl, slots);
}

static void ref_count_native_call(RakVM *vm, RakClosure *cl, RakValue *slots, RakError *err)
{
  RakValue val = slots[1];
  int refCount = rak_is_object(val) ? rak_as_object(val)->refCount : -1;
  rak_vm_push_number(vm, refCount, err);
  if (!rak_is_ok(err)) return;
  rak_vm_return(vm, cl, slots);
}

static void make_array_native_call(RakVM *vm, RakClosure *cl, RakValue *slots, RakError *err)
{
  RakValue val1 = slots[1];
  RakValue val2 = slots[2];
  if (rak_is_nil(val2))
  {
    RakArray *arr = rak_array_new(err);
    if (!rak_is_ok(err)) return;
    rak_vm_push_object(vm, rak_array_value(arr), err);    
    if (!rak_is_ok(err)) return;
    rak_vm_return(vm, cl, slots);
    return;
  }
  if (!rak_is_number(val2) || !rak_is_integer(val2))
  {
    rak_error_set(err, "argument #2 must be nil or an integer number, got %s",
      rak_type_to_cstr(val2.type));
    return;
  }
  int len = (int) rak_as_number(val2);
  len = len < 0 ? 0 : len;
  RakValue val3 = slots[3];
  if (rak_is_nil(val3))
  {
    RakArray *arr = rak_array_new_with_capacity(len, err);
    if (!rak_is_ok(err)) return;
    for (int i = 0; i < len; ++i)
      rak_slice_set(&arr->slice, i, val1);
    arr->slice.len = len;
    rak_vm_push_object(vm, rak_array_value(arr), err);
    if (!rak_is_ok(err)) return;
    rak_vm_return(vm, cl, slots);
    return;
  }
  if (!rak_is_number(val3) || !rak_is_integer(val3))
  {
    rak_error_set(err, "argument #3 must be nil or an integer number, got %s",
      rak_type_to_cstr(val3.type));
    return;
  }
  int cap = (int) rak_as_number(val3);
  cap = cap < len ? len : cap;
  RakArray *arr = rak_array_new_with_capacity(cap, err);
  if (!rak_is_ok(err)) return;
  for (int i = 0; i < len; ++i)
    rak_slice_set(&arr->slice, i, val1);
  arr->slice.len = len;
  rak_vm_push_object(vm, rak_array_value(arr), err);
  if (!rak_is_ok(err)) return;
  rak_vm_return(vm, cl, slots);
}

static void append_native_call(RakVM *vm, RakClosure *cl, RakValue *slots, RakError *err)
{
  RakValue val = slots[1];
  if (!rak_is_array(val))
  {
    rak_error_set(err, "argument #1 must be an array, got %s",
      rak_type_to_cstr(val.type));
    return;
  }
  RakArray *arr = rak_as_array(val);
  RakValue val2 = slots[2];
  RakArray *_arr = rak_array_append(arr, val2, err);
  if (!rak_is_ok(err)) return;
  rak_vm_push_object(vm, rak_array_value(_arr), err);
  if (!rak_is_ok(err)) return;
  rak_vm_return(vm, cl, slots);
}

static void cap_native_call(RakVM *vm, RakClosure *cl, RakValue *slots, RakError *err)
{
  RakValue val = slots[1];
  if (rak_is_string(val))
  {
    RakString *str = rak_as_string(val);
    rak_vm_push_number(vm, rak_string_cap(str), err);
    if (!rak_is_ok(err)) return;
    rak_vm_return(vm, cl, slots);
    return;
  }
  if (rak_is_array(val))
  {
    RakArray *arr = rak_as_array(val);
    rak_vm_push_number(vm, rak_array_cap(arr), err);
    if (!rak_is_ok(err)) return;
    rak_vm_return(vm, cl, slots);
    return;
  }
  if (rak_is_record(val))
  {
    RakRecord *rec = rak_as_record(val);
    rak_vm_push_number(vm, rak_record_cap(rec), err);
    if (!rak_is_ok(err)) return;
    rak_vm_return(vm, cl, slots);
    return;
  }
  rak_error_set(err, "%s does not have a capacity", rak_type_to_cstr(val.type));
}

static void len_native_call(RakVM *vm, RakClosure *cl, RakValue *slots, RakError *err)
{
  RakValue val = slots[1];
  if (rak_is_string(val))
  {
    RakString *str = rak_as_string(val);
    rak_vm_push_number(vm, rak_string_len(str), err);
    if (!rak_is_ok(err)) return;
    rak_vm_return(vm, cl, slots);
    return;
  }
  if (rak_is_array(val))
  {
    RakArray *arr = rak_as_array(val);
    rak_vm_push_number(vm, rak_array_len(arr), err);
    if (!rak_is_ok(err)) return;
    rak_vm_return(vm, cl, slots);
    return;
  }
  if (rak_is_range(val))
  {
    RakRange *rng = rak_as_range(val);
    rak_vm_push_number(vm, rak_range_len(rng), err);
    if (!rak_is_ok(err)) return;
    rak_vm_return(vm, cl, slots);
    return;
  }
  if (rak_is_record(val))
  {
    RakRecord *rec = rak_as_record(val);
    rak_vm_push_number(vm, rak_record_len(rec), err);
    if (!rak_is_ok(err)) return;
    rak_vm_return(vm, cl, slots);
    return;
  }
  rak_error_set(err, "%s does not have a length", rak_type_to_cstr(val.type));
}

static void is_empty_native_call(RakVM *vm, RakClosure *cl, RakValue *slots, RakError *err)
{
  RakValue val = slots[1];
  if (rak_is_string(val))
  {
    RakString *str = rak_as_string(val);
    rak_vm_push_bool(vm, rak_string_is_empty(str), err);
    if (!rak_is_ok(err)) return;
    rak_vm_return(vm, cl, slots);
    return;
  }
  if (rak_is_array(val))
  {
    RakArray *arr = rak_as_array(val);
    rak_vm_push_bool(vm, rak_array_is_empty(arr), err);
    if (!rak_is_ok(err)) return;
    rak_vm_return(vm, cl, slots);
    return;
  }
  if (rak_is_record(val))
  {
    RakRecord *rec = rak_as_record(val);
    rak_vm_push_bool(vm, rak_record_is_empty(rec), err);
    if (!rak_is_ok(err)) return;
    rak_vm_return(vm, cl, slots);
    return;
  }
  rak_error_set(err, "%s does not have a length", rak_type_to_cstr(val.type));
}

static void print_native_call(RakVM *vm, RakClosure *cl, RakValue *slots, RakError *err)
{
  RakValue val = slots[1];
  rak_value_print(val);
  rak_vm_push_nil(vm, err);
  if (!rak_is_ok(err)) return;
  rak_vm_return(vm, cl, slots);
}

static void println_native_call(RakVM *vm, RakClosure *cl, RakValue *slots, RakError *err)
{
  RakValue val = slots[1];
  rak_value_print(val);
  printf("\n");
  rak_vm_push_nil(vm, err);
  if (!rak_is_ok(err)) return;
  rak_vm_return(vm, cl, slots);
}

void rak_builtin_load_globals(RakVM *vm, RakError *err)
{
  rak_vm_push_number(vm, RAK_TYPE_NIL, err);
  if (!rak_is_ok(err)) return;
  rak_vm_push_number(vm, RAK_TYPE_BOOL, err);
  if (!rak_is_ok(err)) return;
  rak_vm_push_number(vm, RAK_TYPE_NUMBER, err);
  if (!rak_is_ok(err)) return;
  rak_vm_push_number(vm, RAK_TYPE_STRING, err);
  if (!rak_is_ok(err)) return;
  rak_vm_push_number(vm, RAK_TYPE_ARRAY, err);
  if (!rak_is_ok(err)) return;
  rak_vm_push_number(vm, RAK_TYPE_RANGE, err);
  if (!rak_is_ok(err)) return;
  rak_vm_push_number(vm, RAK_TYPE_RECORD, err);
  if (!rak_is_ok(err)) return;
  rak_vm_push_number(vm, RAK_TYPE_CLOSURE, err);
  if (!rak_is_ok(err)) return;
  rak_vm_push_number(vm, RAK_NUMBER_EPSILON, err);
  if (!rak_is_ok(err)) return;
  rak_vm_push_number(vm, DBL_MIN, err);
  if (!rak_is_ok(err)) return;
  rak_vm_push_number(vm, DBL_MAX, err);
  if (!rak_is_ok(err)) return;
  rak_vm_push_number(vm, RAK_INTEGER_MIN, err);
  if (!rak_is_ok(err)) return;
  rak_vm_push_number(vm, RAK_INTEGER_MAX, err);
  if (!rak_is_ok(err)) return;
  push_builtin_function(vm, globals[13], 1, type_native_call, err);
  if (!rak_is_ok(err)) return;
  push_builtin_function(vm, globals[14], 1, is_nil_native_call, err);
  if (!rak_is_ok(err)) return;
  push_builtin_function(vm, globals[15], 1, is_bool_native_call, err);
  if (!rak_is_ok(err)) return;
  push_builtin_function(vm, globals[16], 1, is_number_native_call, err);
  if (!rak_is_ok(err)) return;
  push_builtin_function(vm, globals[17], 1, is_integer_native_call, err);
  if (!rak_is_ok(err)) return;
  push_builtin_function(vm, globals[18], 1, is_string_native_call, err);
  if (!rak_is_ok(err)) return;
  push_builtin_function(vm, globals[19], 1, is_array_native_call, err);
  if (!rak_is_ok(err)) return;
  push_builtin_function(vm, globals[20], 1, is_range_native_call, err);
  if (!rak_is_ok(err)) return;
  push_builtin_function(vm, globals[21], 1, is_record_native_call, err);
  if (!rak_is_ok(err)) return;
  push_builtin_function(vm, globals[22], 1, is_closure_native_call, err);
  if (!rak_is_ok(err)) return;
  push_builtin_function(vm, globals[23], 1, is_falsy_native_call, err);
  if (!rak_is_ok(err)) return;
  push_builtin_function(vm, globals[24], 1, is_object_native_call, err);
  if (!rak_is_ok(err)) return;
  push_builtin_function(vm, globals[25], 1, ptr_native_call, err);
  if (!rak_is_ok(err)) return;
  push_builtin_function(vm, globals[26], 1, ref_count_native_call, err);
  if (!rak_is_ok(err)) return;
  push_builtin_function(vm, globals[27], 3, make_array_native_call, err);
  if (!rak_is_ok(err)) return;
  push_builtin_function(vm, globals[28], 2, append_native_call, err);
  if (!rak_is_ok(err)) return;
  push_builtin_function(vm, globals[29], 1, cap_native_call, err);
  if (!rak_is_ok(err)) return;
  push_builtin_function(vm, globals[30], 1, len_native_call, err);
  if (!rak_is_ok(err)) return;
  push_builtin_function(vm, globals[31], 1, is_empty_native_call, err);
  if (!rak_is_ok(err)) return;
  push_builtin_function(vm, globals[32], 1, print_native_call, err);
  if (!rak_is_ok(err)) return;
  push_builtin_function(vm, globals[33], 1, println_native_call, err);
}

int rak_builtin_resolve_global(int len, char *chars)
{
  int n = (int) (sizeof(globals) / sizeof(*globals));
  for (int i = 0; i < n; ++i)
  {
    const char *cstr = globals[i];
    // FIX: Buffer overflow.
    if (!memcmp(cstr, chars, len) && !cstr[len])
      return i;
  }
  return -1;
}
