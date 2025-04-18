//
// record.c
//
// Copyright 2025 Fábio de Souza Villaça Medeiros
//
// This file is part of the Rak Project.
// For detailed license information, please refer to the LICENSE file
// located in the root directory of this project.
//

#include "rak/record.h"
#include <stdio.h>

static inline void release_fields(RakRecord *rec);

static inline void release_fields(RakRecord *rec)
{
  int len = rak_record_len(rec);
  for (int i = 0; i < len; ++i)
  {
    RakRecordField field = rak_record_get(rec, i);
    rak_string_release(field.name);
    rak_value_release(field.val);
  }
}

void rak_record_init(RakRecord *rec, RakError *err)
{
  rak_object_init(&rec->obj);
  rak_slice_init(&rec->slice, err);
}

void rak_record_init_with_capacity(RakRecord *rec, int cap, RakError *err)
{
  rak_object_init(&rec->obj);
  rak_slice_init_with_capacity(&rec->slice, cap, err);
}

void rak_record_init_copy(RakRecord *rec1, RakRecord *rec2, RakError *err)
{
  int len = rak_record_len(rec2);
  rak_record_init_with_capacity(rec1, len, err);
  if (!rak_is_ok(err)) return;
  for (int i = 0; i < len; ++i)
  {
    RakRecordField field = rak_record_get(rec2, i);
    rak_slice_set(&rec1->slice, i, field);
    rak_object_retain(&field.name->obj);
    rak_value_retain(field.val);
  }
  rec1->slice.len = len;
}

void rak_record_deinit(RakRecord *rec)
{
  release_fields(rec);
  rak_slice_deinit(&rec->slice);
}

RakRecord *rak_record_new(RakError *err)
{
  RakRecord *rec = rak_memory_alloc(sizeof(*rec), err);
  if (!rak_is_ok(err)) return NULL;
  rak_record_init(rec, err);
  if (rak_is_ok(err)) return rec;
  rak_memory_free(rec);
  return NULL;
}

RakRecord *rak_record_new_with_capacity(int cap, RakError *err)
{
  RakRecord *rec = rak_memory_alloc(sizeof(*rec), err);
  if (!rak_is_ok(err)) return NULL;
  rak_record_init_with_capacity(rec, cap, err);
  if (rak_is_ok(err)) return rec;
  rak_memory_free(rec);
  return NULL;
}

RakRecord *rak_record_new_copy(RakRecord *rec, RakError *err)
{
  RakRecord *_rec = rak_memory_alloc(sizeof(*_rec), err);
  if (!rak_is_ok(err)) return NULL;
  rak_record_init_copy(_rec, rec, err);
  if (rak_is_ok(err)) return _rec;
  rak_memory_free(_rec);
  return NULL;
}

void rak_record_free(RakRecord *rec)
{
  rak_record_deinit(rec);
  rak_memory_free(rec);
}

void rak_record_release(RakRecord *rec)
{
  RakObject *obj = &rec->obj;
  --obj->refCount;
  if (obj->refCount) return;
  rak_record_free(rec);
}

void rak_record_ensure_capacity(RakRecord *rec, int cap, RakError *err)
{
  rak_slice_ensure_capacity(&rec->slice, cap, err);
}

int rak_record_index_of(RakRecord *rec, RakString *name)
{
  int len = rak_record_len(rec);
  for (int i = 0; i < len; ++i)
  {
    RakRecordField field = rak_record_get(rec, i);
    if (!rak_string_equals(field.name, name)) continue;
    return i;
  }
  return -1;
}

RakRecord *rak_record_put(RakRecord *rec, RakString *name, RakValue val, RakError *err)
{
  // TODO: Implement this function.
  (void) rec;
  (void) name;
  (void) val;
  (void) err;
  return NULL;
}

RakRecord *rak_record_set(RakRecord *rec, int idx, RakValue val, RakError *err)
{
  // TODO: Implement this function.
  (void) rec;
  (void) idx;
  (void) val;
  (void) err;
  return NULL;
}

RakRecord *rak_record_remove_at(RakRecord *rec, int idx, RakError *err)
{
  // TODO: Implement this function.
  (void) rec;
  (void) idx;
  (void) err;
  return NULL;
}

void rak_record_inplace_put(RakRecord *rec, RakString *name, RakValue val, RakError *err)
{
  int idx = rak_record_index_of(rec, name);
  if (idx >= 0)
  {
    rak_record_inplace_set(rec, idx, val);
    return;
  }
  RakRecordField field = {
    .name = name,
    .val = val,
  };
  rak_slice_append(&rec->slice, field, err);
  if (!rak_is_ok(err)) return;
  rak_object_retain(&name->obj);
  rak_value_retain(val);
}

void rak_record_inplace_set(RakRecord *rec, int idx, RakValue val)
{
  RakRecordField field = rak_record_get(rec, idx);
  rak_value_retain(val);
  rak_value_release(field.val);
  field.val = val;
}

void rak_record_inplace_remove_at(RakRecord *rec, int idx)
{
  RakRecordField field = rak_record_get(rec, idx);
  rak_slice_remove_at(&rec->slice, idx);
  rak_string_release(field.name);
  rak_value_release(field.val);
}

void rak_record_inplace_clear(RakRecord *rec)
{
  release_fields(rec);
  rak_slice_clear(&rec->slice);
}

bool rak_record_equals(RakRecord *rec1, RakRecord *rec2)
{
  if (rec1 == rec2) return true;
  int len = rak_record_len(rec1);
  if (len != rak_record_len(rec2)) return false;
  for (int i = 0; i < len; ++i)
  {
    RakRecordField field1 = rak_record_get(rec1, i);
    RakRecordField field2 = rak_record_get(rec2, i);
    if (!rak_string_equals(field1.name, field2.name)) return false;
    if (!rak_value_equals(field1.val, field2.val)) return false;
  }
  return true;
}

void rak_record_print(RakRecord *rec)
{
  printf("{");
  int len = rak_record_len(rec);
  for (int i = 0; i < len; ++i)
  {
    if (i > 0) printf(", ");
    RakRecordField field = rak_record_get(rec, i);
    rak_string_print(field.name);
    printf(": ");
    rak_value_print(field.val);
  }
  printf("}");
}
