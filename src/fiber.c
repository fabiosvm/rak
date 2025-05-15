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

void rak_fiber_run(RakFiber *fiber, RakFunction *fn, RakError *err)
{
  RakClosure *cl = rak_closure_new(RAK_CALLABLE_TYPE_FUNCTION, &fn->callable, err);
  if (!rak_is_ok(err)) return;
  rak_vm_push_object(fiber, rak_closure_value(cl), err);
  if (!rak_is_ok(err))
  {
    rak_closure_free(cl);
    return;
  }
  rak_vm_call(fiber, 0, err);
  if (!rak_is_ok(err)) return;
  rak_fiber_resume(fiber, err);
}

void rak_fiber_resume(RakFiber *fiber, RakError *err)
{
  do
  {
    RakCallFrame frame = rak_stack_get(&fiber->cstk, 0);
    RakClosure *cl = frame.cl;
    RakValue *slots = frame.slots;
    if (cl->type == RAK_CALLABLE_TYPE_FUNCTION)
    {
      rak_vm_dispatch(fiber, cl, frame.ip, slots, err);
      if (!rak_is_ok(err)) return;
      continue;
    }
    RakNativeFunction *native = (RakNativeFunction *) cl->callable;
    native->call(fiber, cl, frame.state, slots, err);
    if (!rak_is_ok(err)) return;
  }
  while (!rak_stack_is_empty(&fiber->cstk));
}
