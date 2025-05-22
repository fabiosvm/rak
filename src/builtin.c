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
#include "rak/native.h"
#include "rak/vm.h"

static const char *globals[] = {
  "TYPE_NIL",
  "TYPE_BOOL",
  "TYPE_NUMBER",
  "TYPE_STRING",
  "TYPE_ARRAY",
  "TYPE_RANGE",
  "TYPE_RECORD",
  "TYPE_CLOSURE",
  "TYPE_FIBER",
  "TYPE_REF",
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
  "is_fiber",
  "is_ref",
  "is_falsy",
  "is_object",
  "ptr",
  "ref_count",
  "array",
  "append",
  "cap",
  "len",
  "is_empty",
  "fiber",
  "is_suspended",
  "is_done",
  "resume",
  "print",
  "println",
  "panic"
};

static inline void append_native_function(RakArray *arr, const char *name, int arity,
  RakNativeFunctionCall call, RakError *err);

static void type_native_call(RakFiber *fiber, RakClosure *cl, int state, RakValue *slots, RakError *err);
static void is_nil_native_call(RakFiber *fiber, RakClosure *cl, int state, RakValue *slots, RakError *err);
static void is_bool_native_call(RakFiber *fiber, RakClosure *cl, int state, RakValue *slots, RakError *err);
static void is_number_native_call(RakFiber *fiber, RakClosure *cl, int state, RakValue *slots, RakError *err);
static void is_integer_native_call(RakFiber *fiber, RakClosure *cl, int state, RakValue *slots, RakError *err);
static void is_string_native_call(RakFiber *fiber, RakClosure *cl, int state, RakValue *slots, RakError *err);
static void is_array_native_call(RakFiber *fiber, RakClosure *cl, int state, RakValue *slots, RakError *err);
static void is_range_native_call(RakFiber *fiber, RakClosure *cl, int state, RakValue *slots, RakError *err);
static void is_record_native_call(RakFiber *fiber, RakClosure *cl, int state, RakValue *slots, RakError *err);
static void is_closure_native_call(RakFiber *fiber, RakClosure *cl, int state, RakValue *slots, RakError *err);
static void is_fiber_native_call(RakFiber *fiber, RakClosure *cl, int state, RakValue *slots, RakError *err);
static void is_ref_native_call(RakFiber *fiber, RakClosure *cl, int state, RakValue *slots, RakError *err);
static void is_falsy_native_call(RakFiber *fiber, RakClosure *cl, int state, RakValue *slots, RakError *err);
static void is_object_native_call(RakFiber *fiber, RakClosure *cl, int state, RakValue *slots, RakError *err);
static void ptr_native_call(RakFiber *fiber, RakClosure *cl, int state, RakValue *slots, RakError *err);
static void ref_count_native_call(RakFiber *fiber, RakClosure *cl, int state, RakValue *slots, RakError *err);
static void array_native_call(RakFiber *fiber, RakClosure *cl, int state, RakValue *slots, RakError *err);
static void append_native_call(RakFiber *fiber, RakClosure *cl, int state, RakValue *slots, RakError *err);
static void cap_native_call(RakFiber *fiber, RakClosure *cl, int state, RakValue *slots, RakError *err);
static void len_native_call(RakFiber *fiber, RakClosure *cl, int state, RakValue *slots, RakError *err);
static void is_empty_native_call(RakFiber *fiber, RakClosure *cl, int state, RakValue *slots, RakError *err);
static void fiber_native_call(RakFiber *fiber, RakClosure *cl, int state, RakValue *slots, RakError *err);
static void is_suspended_native_call(RakFiber *fiber, RakClosure *cl, int state, RakValue *slots, RakError *err);
static void is_done_native_call(RakFiber *fiber, RakClosure *cl, int state, RakValue *slots, RakError *err);
static void resume_native_call(RakFiber *fiber, RakClosure *cl, int state, RakValue *slots, RakError *err);
static void print_native_call(RakFiber *fiber, RakClosure *cl, int state, RakValue *slots, RakError *err);
static void println_native_call(RakFiber *fiber, RakClosure *cl, int state, RakValue *slots, RakError *err);
static void panic_native_call(RakFiber *fiber, RakClosure *cl, int state, RakValue *slots, RakError *err);

static inline void append_native_function(RakArray *arr, const char *name, int arity,
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
  rak_array_inplace_append(arr, rak_closure_value(cl), err);
  if (rak_is_ok(err)) return;
  rak_closure_free(cl);
}

static void type_native_call(RakFiber *fiber, RakClosure *cl, int state, RakValue *slots, RakError *err)
{
  (void) state;
  RakValue val = slots[1];
  rak_vm_push_number(fiber, val.type, err);
  if (!rak_is_ok(err)) return;
  rak_vm_return(fiber, cl, slots);
}

static void is_nil_native_call(RakFiber *fiber, RakClosure *cl, int state, RakValue *slots, RakError *err)
{
  (void) state;
  RakValue val = slots[1];
  rak_vm_push_bool(fiber, rak_is_nil(val), err);
  if (!rak_is_ok(err)) return;
  rak_vm_return(fiber, cl, slots);
}

static void is_bool_native_call(RakFiber *fiber, RakClosure *cl, int state, RakValue *slots, RakError *err)
{
  (void) state;
  RakValue val = slots[1];
  rak_vm_push_bool(fiber, rak_is_bool(val), err);
  if (!rak_is_ok(err)) return;
  rak_vm_return(fiber, cl, slots);
}

static void is_number_native_call(RakFiber *fiber, RakClosure *cl, int state, RakValue *slots, RakError *err)
{
  (void) state;
  RakValue val = slots[1];
  rak_vm_push_bool(fiber, rak_is_number(val), err);
  if (!rak_is_ok(err)) return;
  rak_vm_return(fiber, cl, slots);
}

static void is_integer_native_call(RakFiber *fiber, RakClosure *cl, int state, RakValue *slots, RakError *err)
{
  (void) state;
  RakValue val = slots[1];
  bool data = rak_is_number(val) && rak_is_integer(val);
  rak_vm_push_bool(fiber, data, err);
  if (!rak_is_ok(err)) return;
  rak_vm_return(fiber, cl, slots);
}

static void is_string_native_call(RakFiber *fiber, RakClosure *cl, int state, RakValue *slots, RakError *err)
{
  (void) state;
  RakValue val = slots[1];
  rak_vm_push_bool(fiber, rak_is_string(val), err);
  if (!rak_is_ok(err)) return;
  rak_vm_return(fiber, cl, slots);
}

static void is_array_native_call(RakFiber *fiber, RakClosure *cl, int state, RakValue *slots, RakError *err)
{
  (void) state;
  RakValue val = slots[1];
  rak_vm_push_bool(fiber, rak_is_array(val), err);
  if (!rak_is_ok(err)) return;
  rak_vm_return(fiber, cl, slots);
}

static void is_range_native_call(RakFiber *fiber, RakClosure *cl, int state, RakValue *slots, RakError *err)
{
  (void) state;
  RakValue val = slots[1];
  rak_vm_push_bool(fiber, rak_is_range(val), err);
  if (!rak_is_ok(err)) return;
  rak_vm_return(fiber, cl, slots);
}

static void is_record_native_call(RakFiber *fiber, RakClosure *cl, int state, RakValue *slots, RakError *err)
{
  (void) state;
  RakValue val = slots[1];
  rak_vm_push_bool(fiber, rak_is_record(val), err);
  if (!rak_is_ok(err)) return;
  rak_vm_return(fiber, cl, slots);
}

static void is_closure_native_call(RakFiber *fiber, RakClosure *cl, int state, RakValue *slots, RakError *err)
{
  (void) state;
  RakValue val = slots[1];
  rak_vm_push_bool(fiber, rak_is_closure(val), err);
  if (!rak_is_ok(err)) return;
  rak_vm_return(fiber, cl, slots);
}

static void is_fiber_native_call(RakFiber *fiber, RakClosure *cl, int state, RakValue *slots, RakError *err)
{
  (void) state;
  RakValue val = slots[1];
  rak_vm_push_bool(fiber, rak_is_fiber(val), err);
  if (!rak_is_ok(err)) return;
  rak_vm_return(fiber, cl, slots);
}

static void is_ref_native_call(RakFiber *fiber, RakClosure *cl, int state, RakValue *slots, RakError *err)
{
  (void) state;
  RakValue val = slots[1];
  rak_vm_push_bool(fiber, rak_is_ref(val), err);
  if (!rak_is_ok(err)) return;
  rak_vm_return(fiber, cl, slots);
}

static void is_falsy_native_call(RakFiber *fiber, RakClosure *cl, int state, RakValue *slots, RakError *err)
{
  (void) state;
  RakValue val = slots[1];
  rak_vm_push_bool(fiber, rak_is_falsy(val), err);
  if (!rak_is_ok(err)) return;
  rak_vm_return(fiber, cl, slots);
}

static void is_object_native_call(RakFiber *fiber, RakClosure *cl, int state, RakValue *slots, RakError *err)
{
  (void) state;
  RakValue val = slots[1];
  rak_vm_push_bool(fiber, rak_is_object(val), err);
  if (!rak_is_ok(err)) return;
  rak_vm_return(fiber, cl, slots);
}

static void ptr_native_call(RakFiber *fiber, RakClosure *cl, int state, RakValue *slots, RakError *err)
{
  (void) state;
  RakValue val = slots[1];
  void *ptr = rak_is_object(val) ? val.opaque.ptr : NULL;
  rak_vm_push_number(fiber, (double) (uintptr_t) ptr, err);
  if (!rak_is_ok(err)) return;
  rak_vm_return(fiber, cl, slots);
}

static void ref_count_native_call(RakFiber *fiber, RakClosure *cl, int state, RakValue *slots, RakError *err)
{
  (void) state;
  RakValue val = slots[1];
  int refCount = rak_is_object(val) ? rak_as_object(val)->refCount : -1;
  rak_vm_push_number(fiber, refCount, err);
  if (!rak_is_ok(err)) return;
  rak_vm_return(fiber, cl, slots);
}

static void array_native_call(RakFiber *fiber, RakClosure *cl, int state, RakValue *slots, RakError *err)
{
  (void) state;
  RakValue val1 = slots[1];
  RakValue val2 = slots[2];
  if (rak_is_nil(val2))
  {
    RakArray *arr = rak_array_new(err);
    if (!rak_is_ok(err)) return;
    rak_vm_push_object(fiber, rak_array_value(arr), err);    
    if (!rak_is_ok(err)) return;
    rak_vm_return(fiber, cl, slots);
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
    rak_vm_push_object(fiber, rak_array_value(arr), err);
    if (!rak_is_ok(err)) return;
    rak_vm_return(fiber, cl, slots);
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
  rak_vm_push_object(fiber, rak_array_value(arr), err);
  if (!rak_is_ok(err)) return;
  rak_vm_return(fiber, cl, slots);
}

static void append_native_call(RakFiber *fiber, RakClosure *cl, int state, RakValue *slots, RakError *err)
{
  (void) state;
  RakValue val1 = slots[1];
  if (!rak_is_ref(val1))
  {
    rak_error_set(err, "argument #1 must be a reference to an array, got %s",
      rak_type_to_cstr(val1.type));
    return;
  }
  RakValue *slot = rak_as_ref(val1);
  RakValue _val1 = *slot;
  if (!rak_is_array(_val1))
  {
    rak_error_set(err, "argument #1 must be a reference to an array, got a reference to %s",
      rak_type_to_cstr(_val1.type));
    return;
  }
  RakArray *arr = rak_as_array(_val1);
  RakValue val2 = slots[2];
  if (rak_is_shared(_val1))
  {
    RakArray *_arr = rak_array_append(arr, val2, err);
    if (!rak_is_ok(err)) return;
    RakValue val3 = rak_array_value(_arr);
    *slot = val3;
    rak_object_retain(&_arr->obj);
    --arr->obj.refCount;
    rak_vm_push_object(fiber, val3, err);
    if (!rak_is_ok(err)) return;
    rak_vm_return(fiber, cl, slots);
    return;
  }
  rak_array_inplace_append(arr, val2, err);
  if (!rak_is_ok(err)) return;
  rak_vm_push_object(fiber, _val1, err);
  if (!rak_is_ok(err)) return;
  rak_vm_return(fiber, cl, slots);
}

static void cap_native_call(RakFiber *fiber, RakClosure *cl, int state, RakValue *slots, RakError *err)
{
  (void) state;
  RakValue val = slots[1];
  if (rak_is_string(val))
  {
    RakString *str = rak_as_string(val);
    rak_vm_push_number(fiber, rak_string_cap(str), err);
    if (!rak_is_ok(err)) return;
    rak_vm_return(fiber, cl, slots);
    return;
  }
  if (rak_is_array(val))
  {
    RakArray *arr = rak_as_array(val);
    rak_vm_push_number(fiber, rak_array_cap(arr), err);
    if (!rak_is_ok(err)) return;
    rak_vm_return(fiber, cl, slots);
    return;
  }
  if (rak_is_record(val))
  {
    RakRecord *rec = rak_as_record(val);
    rak_vm_push_number(fiber, rak_record_cap(rec), err);
    if (!rak_is_ok(err)) return;
    rak_vm_return(fiber, cl, slots);
    return;
  }
  rak_error_set(err, "%s does not have a capacity", rak_type_to_cstr(val.type));
}

static void len_native_call(RakFiber *fiber, RakClosure *cl, int state, RakValue *slots, RakError *err)
{
  (void) state;
  RakValue val = slots[1];
  if (rak_is_string(val))
  {
    RakString *str = rak_as_string(val);
    rak_vm_push_number(fiber, rak_string_len(str), err);
    if (!rak_is_ok(err)) return;
    rak_vm_return(fiber, cl, slots);
    return;
  }
  if (rak_is_array(val))
  {
    RakArray *arr = rak_as_array(val);
    rak_vm_push_number(fiber, rak_array_len(arr), err);
    if (!rak_is_ok(err)) return;
    rak_vm_return(fiber, cl, slots);
    return;
  }
  if (rak_is_range(val))
  {
    RakRange *rng = rak_as_range(val);
    rak_vm_push_number(fiber, rak_range_len(rng), err);
    if (!rak_is_ok(err)) return;
    rak_vm_return(fiber, cl, slots);
    return;
  }
  if (rak_is_record(val))
  {
    RakRecord *rec = rak_as_record(val);
    rak_vm_push_number(fiber, rak_record_len(rec), err);
    if (!rak_is_ok(err)) return;
    rak_vm_return(fiber, cl, slots);
    return;
  }
  rak_error_set(err, "%s does not have a length", rak_type_to_cstr(val.type));
}

static void is_empty_native_call(RakFiber *fiber, RakClosure *cl, int state, RakValue *slots, RakError *err)
{
  (void) state;
  RakValue val = slots[1];
  if (rak_is_string(val))
  {
    RakString *str = rak_as_string(val);
    rak_vm_push_bool(fiber, rak_string_is_empty(str), err);
    if (!rak_is_ok(err)) return;
    rak_vm_return(fiber, cl, slots);
    return;
  }
  if (rak_is_array(val))
  {
    RakArray *arr = rak_as_array(val);
    rak_vm_push_bool(fiber, rak_array_is_empty(arr), err);
    if (!rak_is_ok(err)) return;
    rak_vm_return(fiber, cl, slots);
    return;
  }
  if (rak_is_record(val))
  {
    RakRecord *rec = rak_as_record(val);
    rak_vm_push_bool(fiber, rak_record_is_empty(rec), err);
    if (!rak_is_ok(err)) return;
    rak_vm_return(fiber, cl, slots);
    return;
  }
  rak_error_set(err, "%s does not have a length", rak_type_to_cstr(val.type));
}

static void fiber_native_call(RakFiber *fiber, RakClosure *cl, int state, RakValue *slots, RakError *err)
{
  (void) state;
  RakValue val1 = slots[1];
  RakValue val2 = slots[2];
  if (!rak_is_closure(val1))
  {
    rak_error_set(err, "argument #1 must be a closure, got %s",
      rak_type_to_cstr(val1.type));
    return;
  }
  RakClosure *_cl = rak_as_closure(val1);
  uint8_t nargs = 0;
  RakValue *args = NULL;
  if (rak_is_nil(val2)) goto end;
  if (!rak_is_array(val2))
  {
    rak_error_set(err, "argument #2 must be nil or an array, got %s",
      rak_type_to_cstr(val2.type));
    return;
  }
  RakArray *arr = rak_as_array(val2);
  int len = rak_array_len(arr);
  if (len > UINT8_MAX)
  {
    rak_error_set(err, "argument #2 must be nil or an array with at most %d elements",
      UINT8_MAX);
    return;
  }
  nargs = (uint8_t) len;
  args = rak_array_elements(arr);
  RakFiber *_fiber;
end:
  _fiber = rak_fiber_new(fiber->globals, RAK_FIBER_CSTK_DEFAULT_SIZE,
    RAK_FIBER_CSTK_DEFAULT_SIZE, _cl, nargs, args, err);
  if (!rak_is_ok(err)) return;
  rak_vm_push_object(fiber, rak_fiber_value(_fiber), err);
  if (!rak_is_ok(err))
  {
    rak_fiber_free(_fiber);
    return;
  }
  rak_vm_return(fiber, cl, slots);
}

static void is_suspended_native_call(RakFiber *fiber, RakClosure *cl, int state, RakValue *slots, RakError *err)
{
  (void) state;
  RakValue val = slots[1];
  if (!rak_is_fiber(val))
  {
    rak_error_set(err, "argument #1 must be a fiber, got %s",
      rak_type_to_cstr(val.type));
    return;
  }
  RakFiber *_fiber = rak_as_fiber(val);
  rak_vm_push_bool(fiber, _fiber->status == RAK_FIBER_STATUS_SUSPENDED, err);
  if (!rak_is_ok(err)) return;
  rak_vm_return(fiber, cl, slots);
}

static void is_done_native_call(RakFiber *fiber, RakClosure *cl, int state, RakValue *slots, RakError *err)
{
  (void) state;
  RakValue val = slots[1];
  if (!rak_is_fiber(val))
  {
    rak_error_set(err, "argument #1 must be a fiber, got %s",
      rak_type_to_cstr(val.type));
    return;
  }
  RakFiber *_fiber = rak_as_fiber(val);
  rak_vm_push_bool(fiber, _fiber->status == RAK_FIBER_STATUS_DONE, err);
  if (!rak_is_ok(err)) return;
  rak_vm_return(fiber, cl, slots);
}

static void resume_native_call(RakFiber *fiber, RakClosure *cl, int state, RakValue *slots, RakError *err)
{
  (void) state;
  RakValue val = slots[1];
  if (!rak_is_fiber(val))
  {
    rak_error_set(err, "argument #1 must be a fiber, got %s",
      rak_type_to_cstr(val.type));
    return;
  }
  RakFiber *_fiber = rak_as_fiber(val);
  if (_fiber->status != RAK_FIBER_STATUS_SUSPENDED)
  {
    rak_error_set(err, "cannot resume a fiber that is not suspended");
    return;
  }
  rak_fiber_resume(_fiber, err);
  if (!rak_is_ok(err)) return;
  RakValue res = rak_vm_get(_fiber, 0);
  rak_vm_push_value(fiber, res, err);
  if (!rak_is_ok(err)) return;
  rak_vm_pop(_fiber);
  rak_vm_return(fiber, cl, slots);
}

static void print_native_call(RakFiber *fiber, RakClosure *cl, int state, RakValue *slots, RakError *err)
{
  (void) state;
  RakValue val = slots[1];
  rak_value_print(val);
  rak_vm_push_nil(fiber, err);
  if (!rak_is_ok(err)) return;
  rak_vm_return(fiber, cl, slots);
}

static void println_native_call(RakFiber *fiber, RakClosure *cl, int state, RakValue *slots, RakError *err)
{
  (void) state;
  RakValue val = slots[1];
  rak_value_print(val);
  printf("\n");
  rak_vm_push_nil(fiber, err);
  if (!rak_is_ok(err)) return;
  rak_vm_return(fiber, cl, slots);
}

static void panic_native_call(RakFiber *fiber, RakClosure *cl, int state, RakValue *slots, RakError *err)
{
  (void) state;
  RakValue val = slots[1];
  if (!rak_is_string(val))
  {
    rak_error_set(err, "argument #1 must be a string, got %s",
      rak_type_to_cstr(val.type));
    return;
  }
  RakString *str = rak_as_string(val);
  rak_error_set(err, "%.*s", rak_string_len(str), rak_string_chars(str));
  rak_vm_return(fiber, cl, slots);
}

RakArray *rak_builtin_globals(RakError *err)
{
  int len = (int) (sizeof(globals) / sizeof(*globals));
  RakArray *arr = rak_array_new_with_capacity(len, err);
  if (!rak_is_ok(err)) return NULL;
  rak_array_inplace_append(arr, rak_number_value(RAK_TYPE_NIL), err);
  if (!rak_is_ok(err)) return NULL;
  rak_array_inplace_append(arr, rak_number_value(RAK_TYPE_BOOL), err);
  if (!rak_is_ok(err)) return NULL;
  rak_array_inplace_append(arr, rak_number_value(RAK_TYPE_NUMBER), err);
  if (!rak_is_ok(err)) return NULL;
  rak_array_inplace_append(arr, rak_number_value(RAK_TYPE_STRING), err);
  if (!rak_is_ok(err)) return NULL;
  rak_array_inplace_append(arr, rak_number_value(RAK_TYPE_ARRAY), err);
  if (!rak_is_ok(err)) return NULL;
  rak_array_inplace_append(arr, rak_number_value(RAK_TYPE_RANGE), err);
  if (!rak_is_ok(err)) return NULL;
  rak_array_inplace_append(arr, rak_number_value(RAK_TYPE_RECORD), err);
  if (!rak_is_ok(err)) return NULL;
  rak_array_inplace_append(arr, rak_number_value(RAK_TYPE_CLOSURE), err);
  if (!rak_is_ok(err)) return NULL;
  rak_array_inplace_append(arr, rak_number_value(RAK_TYPE_FIBER), err);
  if (!rak_is_ok(err)) return NULL;
  rak_array_inplace_append(arr, rak_number_value(RAK_TYPE_REF), err);
  if (!rak_is_ok(err)) return NULL;
  rak_array_inplace_append(arr, rak_number_value(RAK_NUMBER_EPSILON), err);
  if (!rak_is_ok(err)) return NULL;
  rak_array_inplace_append(arr, rak_number_value(DBL_MIN), err);
  if (!rak_is_ok(err)) return NULL;
  rak_array_inplace_append(arr, rak_number_value(DBL_MAX), err);
  if (!rak_is_ok(err)) return NULL;
  rak_array_inplace_append(arr, rak_number_value(RAK_INTEGER_MIN), err);
  if (!rak_is_ok(err)) return NULL;
  rak_array_inplace_append(arr, rak_number_value(RAK_INTEGER_MAX), err);
  if (!rak_is_ok(err)) return NULL;
  append_native_function(arr, globals[15], 1, type_native_call, err);
  if (!rak_is_ok(err)) return NULL;
  append_native_function(arr, globals[16], 1, is_nil_native_call, err);
  if (!rak_is_ok(err)) return NULL;
  append_native_function(arr, globals[17], 1, is_bool_native_call, err);
  if (!rak_is_ok(err)) return NULL;
  append_native_function(arr, globals[18], 1, is_number_native_call, err);
  if (!rak_is_ok(err)) return NULL;
  append_native_function(arr, globals[19], 1, is_integer_native_call, err);
  if (!rak_is_ok(err)) return NULL;
  append_native_function(arr, globals[20], 1, is_string_native_call, err);
  if (!rak_is_ok(err)) return NULL;
  append_native_function(arr, globals[21], 1, is_array_native_call, err);
  if (!rak_is_ok(err)) return NULL;
  append_native_function(arr, globals[22], 1, is_range_native_call, err);
  if (!rak_is_ok(err)) return NULL;
  append_native_function(arr, globals[23], 1, is_record_native_call, err);
  if (!rak_is_ok(err)) return NULL;
  append_native_function(arr, globals[24], 1, is_closure_native_call, err);
  if (!rak_is_ok(err)) return NULL;
  append_native_function(arr, globals[25], 1, is_fiber_native_call, err);
  if (!rak_is_ok(err)) return NULL;
  append_native_function(arr, globals[26], 1, is_ref_native_call, err);
  if (!rak_is_ok(err)) return NULL;
  append_native_function(arr, globals[27], 1, is_falsy_native_call, err);
  if (!rak_is_ok(err)) return NULL;
  append_native_function(arr, globals[28], 1, is_object_native_call, err);
  if (!rak_is_ok(err)) return NULL;
  append_native_function(arr, globals[29], 1, ptr_native_call, err);
  if (!rak_is_ok(err)) return NULL;
  append_native_function(arr, globals[30], 1, ref_count_native_call, err);
  if (!rak_is_ok(err)) return NULL;
  append_native_function(arr, globals[31], 3, array_native_call, err);
  if (!rak_is_ok(err)) return NULL;
  append_native_function(arr, globals[32], 2, append_native_call, err);
  if (!rak_is_ok(err)) return NULL;
  append_native_function(arr, globals[33], 1, cap_native_call, err);
  if (!rak_is_ok(err)) return NULL;
  append_native_function(arr, globals[34], 1, len_native_call, err);
  if (!rak_is_ok(err)) return NULL;
  append_native_function(arr, globals[35], 1, is_empty_native_call, err);
  if (!rak_is_ok(err)) return NULL;
  append_native_function(arr, globals[36], 2, fiber_native_call, err);
  if (!rak_is_ok(err)) return NULL;
  append_native_function(arr, globals[37], 1, is_suspended_native_call, err);
  if (!rak_is_ok(err)) return NULL;
  append_native_function(arr, globals[38], 1, is_done_native_call, err);
  if (!rak_is_ok(err)) return NULL;
  append_native_function(arr, globals[39], 1, resume_native_call, err);
  if (!rak_is_ok(err)) return NULL;
  append_native_function(arr, globals[40], 1, print_native_call, err);
  if (!rak_is_ok(err)) return NULL;
  append_native_function(arr, globals[41], 1, println_native_call, err);
  if (!rak_is_ok(err)) return NULL;
  append_native_function(arr, globals[42], 1, panic_native_call, err);
  if (!rak_is_ok(err)) return NULL;
  return arr;
}

int rak_builtin_resolve_global(int len, char *chars)
{
  int n = (int) (sizeof(globals) / sizeof(*globals));
  for (int i = 0; i < n; ++i)
  {
    const char *cstr = globals[i];
    if ((int) strlen(cstr) == len && !memcmp(cstr, chars, len))
      return i;
  }
  return -1;
}
