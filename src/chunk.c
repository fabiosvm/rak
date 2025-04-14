//
// chunk.c
//
// Copyright 2025 Fábio de Souza Villaça Medeiros
//
// This file is part of the Rak Project.
// For detailed license information, please refer to the LICENSE file
// located in the root directory of this project.
//

#include "rak/chunk.h"
#include <assert.h>

static inline void release_consts(RakChunk *chunk);

static inline void release_consts(RakChunk *chunk)
{
  int len = chunk->consts.len;
  for (int i = 0; i < len; ++i)
  {
    RakValue val = rak_slice_get(&chunk->consts, i);
    rak_value_release(val);
  }
}

const char *rak_opcode_to_cstr(RakOpcode op)
{
  char *cstr = NULL;
  switch (op)
  {
  case RAK_OP_NOP:           cstr = "NOP";           break;
  case RAK_OP_PUSH_NIL:      cstr = "PUSH_NIL";      break;
  case RAK_OP_PUSH_FALSE:    cstr = "PUSH_FALSE";    break;
  case RAK_OP_PUSH_TRUE:     cstr = "PUSH_TRUE";     break;
  case RAK_OP_LOAD_CONST:    cstr = "LOAD_CONST";    break;
  case RAK_OP_LOAD_LOCAL:    cstr = "LOAD_LOCAL";    break;
  case RAK_OP_NEW_ARRAY:     cstr = "NEW_ARRAY";     break;
  case RAK_OP_POP:           cstr = "POP";           break;
  case RAK_OP_JUMP:          cstr = "JUMP";          break;
  case RAK_OP_JUMP_IF_FALSE: cstr = "JUMP_IF_FALSE"; break;
  case RAK_OP_JUMP_IF_TRUE:  cstr = "JUMP_IF_TRUE";  break;
  case RAK_OP_EQ:            cstr = "EQ";            break;
  case RAK_OP_GT:            cstr = "GT";            break;
  case RAK_OP_LT:            cstr = "LT";            break;
  case RAK_OP_ADD:           cstr = "ADD";           break;
  case RAK_OP_SUB:           cstr = "SUB";           break;
  case RAK_OP_MUL:           cstr = "MUL";           break;
  case RAK_OP_DIV:           cstr = "DIV";           break;
  case RAK_OP_MOD:           cstr = "MOD";           break;
  case RAK_OP_NOT:           cstr = "NOT";           break;
  case RAK_OP_NEG:           cstr = "NEG";           break;
  case RAK_OP_ECHO:          cstr = "ECHO";          break;
  case RAK_OP_HALT:          cstr = "HALT";          break;
  }
  assert(cstr);
  return cstr;
}

void rak_chunk_init(RakChunk *chunk, RakError *err)
{
  rak_slice_init(&chunk->consts, err);
  if (!rak_is_ok(err)) return;
  rak_slice_init(&chunk->instrs, err);
  if (rak_is_ok(err)) return;
  rak_slice_deinit(&chunk->consts);
}

void rak_chunk_deinit(RakChunk *chunk)
{
  release_consts(chunk);
  rak_slice_deinit(&chunk->consts);
  rak_slice_deinit(&chunk->instrs);
}

uint8_t rak_chunk_append_const(RakChunk *chunk, RakValue val, RakError *err)
{
  int idx = chunk->consts.len;
  if (idx > UINT8_MAX)
  {
    rak_error_set(err, "too many constants");
    return 0;
  }
  rak_slice_append(&chunk->consts, val, err);
  if (!rak_is_ok(err)) return 0;
  rak_value_retain(val);
  return (uint8_t) idx;
}

uint16_t rak_chunk_append_instr(RakChunk *chunk, uint32_t instr, RakError *err)
{
  int idx = chunk->instrs.len;
  if (idx > UINT16_MAX)
  {
    rak_error_set(err, "too many instructions");
    return 0;
  }
  rak_slice_append(&chunk->instrs, instr, err);
  if (!rak_is_ok(err)) return 0;
  return (uint16_t) idx;
}

void rak_chunk_clear(RakChunk *chunk)
{
  release_consts(chunk);
  rak_slice_clear(&chunk->consts);
  rak_slice_clear(&chunk->instrs);
}
