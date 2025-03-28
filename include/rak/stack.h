//
// stack.h
//
// Copyright 2025 Fábio de Souza Villaça Medeiros
//
// This file is part of the Rak Project.
// For detailed license information, please refer to the LICENSE file
// located in the root directory of this project.
//

#ifndef RAK_STACK_H
#define RAK_STACK_H

#include "memory.h"

#define RakStack(T) \
  struct { \
    T *base; \
    T *top; \
    T *limit; \
  }

#define rak_stack_init(stk, sz, e) \
  do { \
    size_t size = sizeof(*(stk)->base) * (sz); \
    void *base = rak_memory_alloc(size, (e)); \
    if (!rak_is_ok(e)) break; \
    (stk)->base = base; \
    (stk)->top = &(stk)->base[-1]; \
    (stk)->limit = &(stk)->base[(sz) - 1]; \
  } while (0)

#define rak_stack_deinit(stk) \
  do { \
    rak_memory_free((stk)->base); \
  } while (0)

#define rak_stack_is_empty(stk) ((stk)->top == &(stk)->base[-1])

#define rak_stack_is_full(stk) ((stk)->top == (stk)->limit)

#define rak_stack_get(stk, i) ((stk)->top[- (i)])

#define rak_stack_set(stk, i, d) \
  do { \
    (stk)->top[- (i)] = (d); \
  } while (0)

#define rak_stack_push(stk, d) \
  do { \
    ++(stk)->top; \
    *(stk)->top = (d); \
  } while (0)

#define rak_stack_pop(stk) \
  do { \
    --(stk)->top; \
  } while (0)

#endif // RAK_STACK_H
