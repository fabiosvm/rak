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
#include "rak/native.h"
#include "rak/vm.h"

static void run(RakFiber *fiber, RakError *err);
static void resume(RakFiber *fiber, RakError *err);

static void run(RakFiber *fiber, RakError *err)
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
    fiber->status = RAK_FIBER_STATUS_DONE;
    return;
  }
  if (rak_stack_is_empty(&fiber->cstk))
  {
    fiber->status = RAK_FIBER_STATUS_DONE;
    return;
  }
  run(fiber, err);
}

static void resume(RakFiber *fiber, RakError *err)
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
    fiber->status = RAK_FIBER_STATUS_DONE;
    return;
  }
  if (fiber->status == RAK_FIBER_STATUS_SUSPENDED)
    return;
  if (rak_stack_is_empty(&fiber->cstk))
  {
    fiber->status = RAK_FIBER_STATUS_DONE;
    return;
  }
  resume(fiber, err);
}

void rak_fiber_init(RakFiber *fiber, RakArray *globals, int vstkSize, int cstkSize,
  RakClosure *cl, uint8_t nargs, RakValue *args, RakError *err)
{
  fiber->status = RAK_FIBER_STATUS_READY;
  fiber->globals = globals;
  rak_stack_init(&fiber->vstk, vstkSize, err);
  if (!rak_is_ok(err)) return;
  rak_stack_init(&fiber->cstk, cstkSize, err);
  if (!rak_is_ok(err))
  {
    rak_stack_deinit(&fiber->vstk);
    return;
  }
  rak_vm_push_object(fiber, rak_closure_value(cl), err);
  if (!rak_is_ok(err))
  {
    rak_stack_deinit(&fiber->vstk);
    rak_stack_deinit(&fiber->cstk);
    return;
  }
  RakValue *slots = &rak_stack_get(&fiber->vstk, nargs);
  for (int i = 0; i < nargs; ++i)
  {
    RakValue val = args[i];
    rak_vm_push_value(fiber, val, err);
    if (!rak_is_ok(err)) goto fail;
  }
  int arity = cl->callable->arity;
  while (nargs > arity)
  {
    rak_vm_pop(fiber);
    --nargs;
  }
  while (nargs < arity)
  {
    rak_vm_push_nil(fiber, err);
    if (!rak_is_ok(err)) goto fail;
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
    rak_object_retain(&globals->obj);
    return;
  }
  frame.state = 0;
  rak_stack_push(&fiber->cstk, frame);
  rak_object_retain(&globals->obj);
  return;
fail:
  while (!rak_stack_is_empty(&fiber->vstk))
    rak_vm_pop(fiber);
  rak_stack_deinit(&fiber->vstk);
  rak_stack_deinit(&fiber->cstk);
}

void rak_fiber_deinit(RakFiber *fiber)
{
  rak_array_release(fiber->globals);
  while (!rak_stack_is_empty(&fiber->vstk))
    rak_vm_pop(fiber);
  rak_stack_deinit(&fiber->vstk);
  rak_stack_deinit(&fiber->cstk);
}

void rak_fiber_run(RakFiber *fiber, RakError *err)
{
  fiber->status = RAK_FIBER_STATUS_RUNNING;
  run(fiber, err);
}

void rak_fiber_resume(RakFiber *fiber, RakError *err)
{
  fiber->status = RAK_FIBER_STATUS_RUNNING;
  resume(fiber, err);
}
