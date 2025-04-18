//
// range.h
//
// Copyright 2025 Fábio de Souza Villaça Medeiros
//
// This file is part of the Rak Project.
// For detailed license information, please refer to the LICENSE file
// located in the root directory of this project.
//

#ifndef RAK_RANGE_H
#define RAK_RANGE_H

#include "value.h"

#define rak_range_len(r)    ((r)->start < (r)->end ? (r)->end - (r)->start : 0)
#define rak_range_get(r, i) ((r)->start + (i))

typedef struct
{
  RakObject obj;
  double    start;
  double    end;
} RakRange;

void rak_range_init(RakRange *range, double start, double end);
void rak_range_init_copy(RakRange *range1, RakRange *range2);
RakRange *rak_range_new(double start, double end, RakError *err);
RakRange *rak_range_new_copy(RakRange *range, RakError *err);
void rak_range_free(RakRange *range);
void rak_range_release(RakRange *range);
void rak_range_inplace_set(RakRange *range, double start, double end);
bool rak_range_equals(RakRange *range1, RakRange *range2);
void rak_range_print(RakRange *range);

#endif // RAK_RANGE_H
