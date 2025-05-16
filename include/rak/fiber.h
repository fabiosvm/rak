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

#include "closure.h"
#include "stack.h"

#define RAK_FIBER_VSTK_DEFAULT_SIZE (1024)
#define RAK_FIBER_CSTK_DEFAULT_SIZE (128)

typedef enum
{
  RAK_FIBER_STATE_READY,
  RAK_FIBER_STATE_RUNNING,
  RAK_FIBER_STATE_SUSPENDED,
  RAK_FIBER_STATE_DONE
} RakFiberState;

typedef struct
{
  RakClosure *cl;
  union
  {
    uint32_t *ip;
    int       state;
  };
  RakValue *slots;
} RakCallFrame;

typedef struct RakFiber
{
  RakObject              obj;
  RakFiberState          state;
  RakStack(RakValue)     vstk;
  RakStack(RakCallFrame) cstk;
} RakFiber;

// TODO: rak_fiber_init(RakFiber *fiber, int vstkSize, int cstkSize, RakClosure *cl, RakError *err);
void rak_fiber_init(RakFiber *fiber, int vstkSize, int cstkSize, RakError *err);

void rak_fiber_deinit(RakFiber *fiber);

// TODO: RakFiber *rak_fiber_new(int vstkSize, int cstkSize, RakClosure *cl, RakError *err);
RakFiber *rak_fiber_new(int vstkSize, int cstkSize, RakError *err);

void rak_fiber_free(RakFiber *fiber);
void rak_fiber_release(RakFiber *fiber);

// TODO: void rak_fiber_run(RakFiber *fiber, RakError *err);
void rak_fiber_run(RakFiber *fiber, RakCallable *callable, RakError *err);

void rak_fiber_resume(RakFiber *fiber, RakError *err);

#endif // RAK_FIBER_H
