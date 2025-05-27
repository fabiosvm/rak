//
// fiber.h
//
// Copyright 2025 Fábio de Souza Villaça Medeiros
//
// This file is part of the Rak Project.
// For detailed license information, please refer to the LICENSE file
// located in the root directory of this project.
//

#ifndef RAK_FIBER_H
#define RAK_FIBER_H

#include "array.h"
#include "closure.h"
#include "stack.h"

#define RAK_FIBER_VSTK_DEFAULT_SIZE (1024)
#define RAK_FIBER_CSTK_DEFAULT_SIZE (128)

typedef enum
{
  RAK_FIBER_STATUS_SUSPENDED,
  RAK_FIBER_STATUS_RUNNING,
  RAK_FIBER_STATUS_DONE
} RakFiberStatus;

typedef struct
{
  RakClosure *cl;
  void       *state;
  RakValue   *slots;
} RakCallFrame;

typedef struct RakFiber
{
  RakObject               obj;
  RakFiberStatus          status;
  RakArray               *globals;
  RakStack(RakValue)      vstk;
  RakStack(RakCallFrame)  cstk;
} RakFiber;

static inline void rak_fiber_push(RakFiber *fiber, RakValue val, RakError *err);
static inline void rak_fiber_push_nil(RakFiber *fiber, RakError *err);
static inline void rak_fiber_push_bool(RakFiber *fiber, bool data, RakError *err);
static inline void rak_fiber_push_number(RakFiber *fiber, double data, RakError *err);
static inline void rak_fiber_push_value(RakFiber *fiber, RakValue val, RakError *err);
static inline void rak_fiber_push_object(RakFiber *fiber, RakValue val, RakError *err);
static inline RakValue rak_fiber_get(RakFiber *fiber, uint8_t idx);
static inline void rak_fiber_set(RakFiber *fiber, uint8_t idx, RakValue val);
static inline void rak_fiber_set_value(RakFiber *fiber, uint8_t idx, RakValue val);
static inline void rak_fiber_set_object(RakFiber *fiber, uint8_t idx, RakValue val);
static inline void rak_fiber_pop(RakFiber *fiber);
static inline void rak_fiber_yield(RakFiber *fiber, uint32_t *ip);
static inline void rak_fiber_return(RakFiber *fiber, RakClosure *cl, RakValue *slots);

void rak_fiber_init(RakFiber *fiber, RakArray *globals, int vstkSize, int cstkSize,
  RakClosure *cl, uint8_t nargs, RakValue *args, RakError *err);
void rak_fiber_deinit(RakFiber *fiber);
RakFiber *rak_fiber_new(RakArray *globals, int vstkSize, int cstkSize,
  RakClosure *cl, uint8_t nargs, RakValue *args, RakError *err);
void rak_fiber_free(RakFiber *fiber);
void rak_fiber_release(RakFiber *fiber);
void rak_fiber_run(RakFiber *fiber, RakError *err);
void rak_fiber_resume(RakFiber *fiber, RakError *err);

static inline void rak_fiber_push(RakFiber *fiber, RakValue val, RakError *err)
{
  if (rak_stack_is_full(&fiber->vstk))
  {
    rak_error_set(err, "stack overflow");
    return;
  }
  rak_stack_push(&fiber->vstk, val);
}

static inline void rak_fiber_push_nil(RakFiber *fiber, RakError *err)
{
  rak_fiber_push(fiber, rak_nil_value(), err);
}

static inline void rak_fiber_push_bool(RakFiber *fiber, bool data, RakError *err)
{
  rak_fiber_push(fiber, rak_bool_value(data), err);
}

static inline void rak_fiber_push_number(RakFiber *fiber, double data, RakError *err)
{
  rak_fiber_push(fiber, rak_number_value(data), err);
}

static inline void rak_fiber_push_value(RakFiber *fiber, RakValue val, RakError *err)
{
  rak_fiber_push(fiber, val, err);
  if (!rak_is_ok(err)) return;
  rak_value_retain(val);
}

static inline void rak_fiber_push_object(RakFiber *fiber, RakValue val, RakError *err)
{
  rak_fiber_push(fiber, val, err);
  if (!rak_is_ok(err)) return;
  rak_object_retain(rak_as_object(val));
}

static inline RakValue rak_fiber_get(RakFiber *fiber, uint8_t idx)
{
  return rak_stack_get(&fiber->vstk, idx);
}

static inline void rak_fiber_set(RakFiber *fiber, uint8_t idx, RakValue val)
{
  RakValue _val = rak_stack_get(&fiber->vstk, idx);
  rak_value_release(_val);
  rak_stack_set(&fiber->vstk, idx, val);
}

static inline void rak_fiber_set_value(RakFiber *fiber, uint8_t idx, RakValue val)
{
  rak_value_retain(val);
  rak_fiber_set(fiber, idx, val);
}

static inline void rak_fiber_set_object(RakFiber *fiber, uint8_t idx, RakValue val)
{
  rak_object_retain(rak_as_object(val));
  rak_fiber_set(fiber, idx, val);
}

static inline void rak_fiber_pop(RakFiber *fiber)
{
  RakValue val = rak_fiber_get(fiber, 0);
  rak_stack_pop(&fiber->vstk);
  rak_value_release(val);
}

static inline void rak_fiber_yield(RakFiber *fiber, uint32_t *ip)
{
  fiber->status = RAK_FIBER_STATUS_SUSPENDED;
  RakCallFrame *frame = &rak_stack_get(&fiber->cstk, 0);
  frame->state = ip + 1;
}

static inline void rak_fiber_return(RakFiber *fiber, RakClosure *cl, RakValue *slots)
{
  slots[0] = rak_fiber_get(fiber, 0);
  rak_closure_release(cl);
  rak_stack_pop(&fiber->vstk);
  while (fiber->vstk.top > slots)
    rak_fiber_pop(fiber);
  rak_stack_pop(&fiber->cstk);
}

#endif // RAK_FIBER_H
