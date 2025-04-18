//
// range.c
//
// Copyright 2025 Fábio de Souza Villaça Medeiros
//
// This file is part of the Rak Project.
// For detailed license information, please refer to the LICENSE file
// located in the root directory of this project.
//

#include "rak/range.h"
#include <inttypes.h>
#include <stdio.h>
#include "rak/memory.h"

void rak_range_init(RakRange *range, double start, double end)
{
  rak_object_init(&range->obj);
  range->start = start;
  range->end = end;
}

void rak_range_init_copy(RakRange *range1, RakRange *range2)
{
  rak_object_init(&range1->obj);
  range1->start = range2->start;
  range1->end = range2->end;
}

RakRange *rak_range_new(double start, double end, RakError *err)
{
  RakRange *range = rak_memory_alloc(sizeof(*range), err);
  if (!rak_is_ok(err)) return NULL;
  rak_range_init(range, start, end);
  return range;
}

RakRange *rak_range_new_copy(RakRange *range, RakError *err)
{
  RakRange *_range = rak_memory_alloc(sizeof(*_range), err);
  if (!rak_is_ok(err)) return NULL;
  rak_range_init_copy(_range, range);
  return _range;
}

void rak_range_free(RakRange *range)
{
  rak_memory_free(range);
}

void rak_range_release(RakRange *range)
{
  RakObject *obj = &range->obj;
  --obj->refCount;
  if (obj->refCount) return;
  rak_range_free(range);
}

void rak_range_inplace_set(RakRange *range, double start, double end)
{
  range->start = start;
  range->end = end;
}

bool rak_range_equals(RakRange *range1, RakRange *range2)
{
  return range1->start == range2->start && range1->end == range2->end;
}

void rak_range_print(RakRange *range)
{
  printf("%" PRId64 "..%" PRId64, (int64_t) range->start, (int64_t) range->end);
}
