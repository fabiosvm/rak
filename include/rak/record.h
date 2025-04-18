//
// record.h
//
// Copyright 2025 Fábio de Souza Villaça Medeiros
//
// This file is part of the Rak Project.
// For detailed license information, please refer to the LICENSE file
// located in the root directory of this project.
//

#ifndef RAK_RECORD_H
#define RAK_RECORD_H

#include "string.h"

#define rak_record_cap(r)      ((r)->slice.cap)
#define rak_record_len(r)      ((r)->slice.len)
#define rak_record_fields(r)   ((r)->slice.data)
#define rak_record_is_empty(r) (!rak_record_len(r))
#define rak_record_get(r, i)   rak_slice_get(&(r)->slice, (i))

typedef struct
{
  RakString *name;
  RakValue   val;
} RakRecordField;

typedef struct
{
  RakObject                obj;
  RakSlice(RakRecordField) slice;
} RakRecord;

void rak_record_init(RakRecord *rec, RakError *err);
void rak_record_init_with_capacity(RakRecord *rec, int cap, RakError *err);
void rak_record_init_copy(RakRecord *rec1, RakRecord *rec2, RakError *err);
void rak_record_deinit(RakRecord *rec);
RakRecord *rak_record_new(RakError *err);
RakRecord *rak_record_new_with_capacity(int cap, RakError *err);
RakRecord *rak_record_new_copy(RakRecord *arr, RakError *err);
void rak_record_free(RakRecord *rec);
void rak_record_release(RakRecord *rec);
void rak_record_ensure_capacity(RakRecord *rec, int cap, RakError *err);
int rak_record_index_of(RakRecord *rec, RakString *name);
RakRecord *rak_record_put(RakRecord *rec, RakString *name, RakValue val, RakError *err);
RakRecord *rak_record_set(RakRecord *rec, int idx, RakValue val, RakError *err);
RakRecord *rak_record_remove_at(RakRecord *rec, int idx, RakError *err);
void rak_record_inplace_put(RakRecord *rec, RakString *name, RakValue val, RakError *err);
void rak_record_inplace_set(RakRecord *rec, int idx, RakValue val);
void rak_record_inplace_remove_at(RakRecord *rec, int idx);
void rak_record_inplace_clear(RakRecord *rec);
bool rak_record_equals(RakRecord *rec1, RakRecord *rec2);
void rak_record_print(RakRecord *rec);

#endif // RAK_RECORD_H
