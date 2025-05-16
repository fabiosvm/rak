//
// fiber.c
//
// Copyright 2025 Fábio de Souza Villaça Medeiros
//
// This file is part of the Rak Project.
// For detailed license information, please refer to the LICENSE file
// located in the root directory of this project.
//

#include "rak/fiber.h"
#include "rak/vm.h"

void rak_fiber_init(RakFiber *fiber, int vstkSize, int cstkSize, RakError *err)
{
  rak_object_init(&fiber->obj);
  fiber->state = RAK_FIBER_STATE_READY;
  rak_stack_init(&fiber->vstk, vstkSize, err);
  if (!rak_is_ok(err)) return;
  rak_stack_init(&fiber->cstk, cstkSize, err);
  if (rak_is_ok(err)) return;
  rak_stack_deinit(&fiber->vstk);
}

void rak_fiber_deinit(RakFiber *fiber)
{
  while (!rak_stack_is_empty(&fiber->vstk))
    rak_vm_pop(fiber);
  rak_stack_deinit(&fiber->vstk);
  rak_stack_deinit(&fiber->cstk);
}

RakFiber *rak_fiber_new(int vstkSize, int cstkSize, RakError *err)
{
  RakFiber *fiber = rak_memory_alloc(sizeof(*fiber), err);
  if (!rak_is_ok(err)) return NULL;
  rak_fiber_init(fiber, vstkSize, cstkSize, err);
  if (rak_is_ok(err)) return fiber;
  rak_memory_free(fiber);
  return NULL;
}

void rak_fiber_free(RakFiber *fiber)
{
  rak_fiber_deinit(fiber);
  rak_memory_free(fiber);
}

void rak_fiber_release(RakFiber *fiber)
{
  RakObject *obj = &fiber->obj;
  --obj->refCount;
  if (obj->refCount) return;
  rak_fiber_free(fiber);
}

void rak_fiber_run(RakFiber *fiber, RakCallable *callable, RakError *err)
{
  RakClosure *cl = rak_closure_new(RAK_CALLABLE_TYPE_FUNCTION,
    callable, err);
  if (!rak_is_ok(err)) return;
  rak_vm_push_object(fiber, rak_closure_value(cl), err);
  if (!rak_is_ok(err))
  {
    rak_closure_free(cl);
    return;
  }
  rak_vm_call(fiber, 0, err);
  if (!rak_is_ok(err)) return;
  fiber->state = RAK_FIBER_STATE_RUNNING;
  rak_fiber_resume(fiber, err);
}

void rak_fiber_resume(RakFiber *fiber, RakError *err)
{
  RakCallFrame frame = rak_stack_get(&fiber->cstk, 0);
  RakClosure *cl = frame.cl;
  RakValue *slots = frame.slots;
  if (cl->type == RAK_CALLABLE_TYPE_FUNCTION)
    rak_vm_dispatch(fiber, cl, frame.ip, slots, err);
  else
   ((RakNativeFunction *) cl->callable)->call(fiber, cl,
      frame.state, slots, err);
  if (!rak_is_ok(err))
  {
    fiber->state = RAK_FIBER_STATE_DONE;
    return;
  }
  if (fiber->state == RAK_FIBER_STATE_SUSPENDED) return;
  if (rak_stack_is_empty(&fiber->cstk))
  {
    fiber->state = RAK_FIBER_STATE_DONE;
    return;
  }
  rak_fiber_resume(fiber, err);
}
