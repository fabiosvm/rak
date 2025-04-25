//
// slice.h
//
// Copyright 2025 Fábio de Souza Villaça Medeiros
// 
// This file is part of the Rak Project.
// For detailed license information, please refer to the LICENSE file
// located in the root directory of this project.
// 

#ifndef RAK_SLICE_H
#define RAK_SLICE_H

#include "memory.h"

#define RAK_SLICE_MIN_CAPACITY ((int) 1 << 3)

#define RakStaticSlice(T, c) \
  struct { \
    int cap; \
    int len; \
    T   data[c]; \
  }

#define RakSlice(T) \
  struct { \
    int  cap; \
    int  len; \
    T   *data; \
  }

#define rak_static_slice_init(s) \
  do { \
    (s)->cap = sizeof((s)->data) / sizeof(*(s)->data); \
    (s)->len = 0; \
  } while (0)

#define rak_slice_init(s, e) \
  do { \
    int cap = RAK_SLICE_MIN_CAPACITY; \
    int size = sizeof(*(s)->data) * cap; \
    void *data = rak_memory_alloc(size, (e)); \
    if (!rak_is_ok(e)) break; \
    (s)->cap = cap; \
    (s)->len = 0; \
    (s)->data = data; \
  } while (0)

#define rak_slice_init_with_capacity(s, c, e) \
  do { \
    int _cap = RAK_SLICE_MIN_CAPACITY; \
    while (_cap < (c)) _cap <<= 1; \
    int size = sizeof(*(s)->data) * _cap; \
    void *data = rak_memory_alloc(size, (e)); \
    if (!rak_is_ok(e)) break; \
    (s)->cap = _cap; \
    (s)->len = 0; \
    (s)->data = data; \
  } while (0)

#define rak_slice_deinit(s) \
  do { \
    rak_memory_free((s)->data); \
  } while (0)

#define rak_slice_ensure_capacity(s, c, e) \
  do { \
    if ((c) <= (s)->cap) break; \
    int _cap = (s)->cap; \
    while (_cap < (c)) _cap <<= 1; \
    int size = sizeof(*(s)->data) * _cap; \
    void *data = rak_memory_realloc((s)->data, size, (e)); \
    if (!rak_is_ok(e)) break; \
    (s)->cap = _cap; \
    (s)->data = data; \
  } while (0)

#define rak_slice_is_empty(s) (!(s)->len)

#define rak_slice_is_full(s) ((s)->len == (s)->cap)

#define rak_slice_get(s, i) ((s)->data[(i)])

#define rak_slice_append(s, d) \
  do { \
    (s)->data[(s)->len] = (d); \
    ++(s)->len; \
  } while (0)

#define rak_slice_ensure_append(s, d, e) \
  do { \
    rak_slice_ensure_capacity((s), (s)->len + 1, (e)); \
    if (!rak_is_ok(e)) break; \
    rak_slice_append((s), (d)); \
  } while (0)

#define rak_slice_set(s, i, d) \
  do { \
    (s)->data[(i)] = (d); \
  } while (0)

#define rak_slice_remove_at(s, i) \
  do { \
    --(s)->len; \
    for (int j = (i); j < (s)->len; ++j) \
      (s)->data[j] = (s)->data[j + 1]; \
  } while (0)

#define rak_slice_clear(s) \
  do { \
    (s)->len = 0; \
  } while (0)

#endif // RAK_SLICE_H
