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

const char *rak_opcode_to_cstr(RakOpcode op)
{
  char *cstr = NULL;
  switch (op)
  {
  case RAK_OP_NIL:   cstr = "NIL";   break;
  case RAK_OP_CONST: cstr = "CONST"; break;
  case RAK_OP_POP:   cstr = "POP";   break;
  case RAK_OP_ADD:   cstr = "ADD";   break;
  case RAK_OP_SUB:   cstr = "SUB";   break;
  case RAK_OP_MUL:   cstr = "MUL";   break;
  case RAK_OP_DIV:   cstr = "DIV";   break;
  case RAK_OP_MOD:   cstr = "MOD";   break;
  case RAK_OP_NEG:   cstr = "NEG";   break;
  case RAK_OP_HALT:  cstr = "HALT";  break;
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
  rak_slice_deinit(&chunk->consts);
  rak_slice_deinit(&chunk->instrs);
}

int rak_chunk_append_const(RakChunk *chunk, RakValue val, RakError *err)
{
  int idx = chunk->consts.len;
  rak_slice_append(&chunk->consts, val, err);
  if (rak_is_ok(err)) return idx;
  return -1;
}

void rak_chunk_append_instr(RakChunk *chunk, uint32_t instr, RakError *err)
{
  rak_slice_append(&chunk->instrs, instr, err);
}

void rak_chunk_clear(RakChunk *chunk)
{
  rak_slice_clear(&chunk->consts);
  rak_slice_clear(&chunk->instrs);
}
