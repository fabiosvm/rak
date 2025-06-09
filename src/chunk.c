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
  case RAK_OP_NOP:                  cstr = "NOP";                  break;
  case RAK_OP_PUSH_NIL:             cstr = "PUSH_NIL";             break;
  case RAK_OP_PUSH_FALSE:           cstr = "PUSH_FALSE";           break;
  case RAK_OP_PUSH_TRUE:            cstr = "PUSH_TRUE";            break;
  case RAK_OP_PUSH_INT:             cstr = "PUSH_INT";             break;
  case RAK_OP_LOAD_CONST:           cstr = "LOAD_CONST";           break;
  case RAK_OP_LOAD_GLOBAL:          cstr = "LOAD_GLOBAL";          break;
  case RAK_OP_LOAD_LOCAL:           cstr = "LOAD_LOCAL";           break;
  case RAK_OP_STORE_LOCAL:          cstr = "STORE_LOCAL";          break;
  case RAK_OP_FETCH_LOCAL:          cstr = "FETCH_LOCAL";          break;
  case RAK_OP_REF_LOCAL:            cstr = "REF_LOCAL";            break;
  case RAK_OP_LOAD_LOCAL_REF:       cstr = "LOAD_LOCAL_REF";       break;
  case RAK_OP_STORE_LOCAL_REF:      cstr = "STORE_LOCAL_REF";      break;
  case RAK_OP_NEW_ARRAY:            cstr = "NEW_ARRAY";            break;
  case RAK_OP_NEW_RANGE:            cstr = "NEW_RANGE";            break;
  case RAK_OP_NEW_RECORD:           cstr = "NEW_RECORD";           break;
  case RAK_OP_NEW_CLOSURE:          cstr = "NEW_CLOSURE";          break;
  case RAK_OP_MOVE:                 cstr = "MOVE";                 break;
  case RAK_OP_POP:                  cstr = "POP";                  break;
  case RAK_OP_GET_ELEMENT:          cstr = "GET_ELEMENT";          break;
  case RAK_OP_SET_ELEMENT:          cstr = "SET_ELEMENT";          break;
  case RAK_OP_LOAD_ELEMENT:         cstr = "LOAD_ELEMENT";         break;
  case RAK_OP_FETCH_ELEMENT:        cstr = "FETCH_ELEMENT";        break;
  case RAK_OP_UPDATE_ELEMENT:       cstr = "UPDATE_ELEMENT";       break;
  case RAK_OP_GET_FIELD:            cstr = "GET_FIELD";            break;
  case RAK_OP_PUT_FIELD:            cstr = "PUT_FIELD";            break;
  case RAK_OP_LOAD_FIELD:           cstr = "LOAD_FIELD";           break;
  case RAK_OP_FETCH_FIELD:          cstr = "FETCH_FIELD";          break;
  case RAK_OP_UPDATE_FIELD:         cstr = "UPDATE_FIELD";         break;
  case RAK_OP_UNPACK_ELEMENTS:      cstr = "UNPACK_ELEMENTS";      break;
  case RAK_OP_UNPACK_FIELDS:        cstr = "UNPACK_FIELDS";        break;
  case RAK_OP_JUMP:                 cstr = "JUMP";                 break;
  case RAK_OP_JUMP_IF_FALSE:        cstr = "JUMP_IF_FALSE";        break;
  case RAK_OP_JUMP_IF_TRUE:         cstr = "JUMP_IF_TRUE";         break;
  case RAK_OP_JUMP_IF_FALSE_OR_POP: cstr = "JUMP_IF_FALSE_OR_POP"; break;
  case RAK_OP_JUMP_IF_TRUE_OR_POP:  cstr = "JUMP_IF_TRUE_OR_POP";  break;
  case RAK_OP_EQ:                   cstr = "EQ";                   break;
  case RAK_OP_NE:                   cstr = "NE";                   break;
  case RAK_OP_GT:                   cstr = "GT";                   break;
  case RAK_OP_GE:                   cstr = "GE";                   break;
  case RAK_OP_LT:                   cstr = "LT";                   break;
  case RAK_OP_LE:                   cstr = "LE";                   break;
  case RAK_OP_ADD:                  cstr = "ADD";                  break;
  case RAK_OP_ADD2:                 cstr = "ADD2";                 break;
  case RAK_OP_ADD3:                 cstr = "ADD3";                 break;
  case RAK_OP_SUB:                  cstr = "SUB";                  break;
  case RAK_OP_SUB2:                 cstr = "SUB2";                 break;
  case RAK_OP_SUB3:                 cstr = "SUB3";                 break;
  case RAK_OP_MUL:                  cstr = "MUL";                  break;
  case RAK_OP_MUL2:                 cstr = "MUL2";                 break;
  case RAK_OP_MUL3:                 cstr = "MUL3";                 break;
  case RAK_OP_DIV:                  cstr = "DIV";                  break;
  case RAK_OP_DIV2:                 cstr = "DIV2";                 break;
  case RAK_OP_DIV3:                 cstr = "DIV3";                 break;
  case RAK_OP_MOD:                  cstr = "MOD";                  break;
  case RAK_OP_MOD2:                 cstr = "MOD2";                 break;
  case RAK_OP_MOD3:                 cstr = "MOD3";                 break;
  case RAK_OP_NOT:                  cstr = "NOT";                  break;
  case RAK_OP_NEG:                  cstr = "NEG";                  break;
  case RAK_OP_CALL:                 cstr = "CALL";                 break;
  case RAK_OP_TAIL_CALL:            cstr = "TAIL_CALL";            break;
  case RAK_OP_YIELD:                cstr = "YIELD";                break;
  case RAK_OP_RETURN:               cstr = "RETURN";               break;
  case RAK_OP_RETURN_NIL:           cstr = "RETURN_NIL";           break;
  }
  return cstr;
}

void rak_chunk_init(RakChunk *chunk, RakError *err)
{
  rak_slice_init(&chunk->consts, err);
  if (!rak_is_ok(err)) return;
  rak_slice_init(&chunk->instrs, err);
  if (!rak_is_ok(err))
  {
    rak_slice_deinit(&chunk->consts);
    return;
  }
  rak_slice_init(&chunk->maps, err);
  if (rak_is_ok(err)) return;
  rak_slice_deinit(&chunk->consts);
  rak_slice_deinit(&chunk->instrs);
}

void rak_chunk_deinit(RakChunk *chunk)
{
  release_consts(chunk);
  rak_slice_deinit(&chunk->consts);
  rak_slice_deinit(&chunk->instrs);
  rak_slice_deinit(&chunk->maps);
}

uint8_t rak_chunk_append_const(RakChunk *chunk, RakValue val, RakError *err)
{
  int len = chunk->consts.len;
  if (len == RAK_CHUNK_MAX_CONSTS)
  {
    rak_error_set(err, "too many constants");
    return 0;
  }
  rak_slice_ensure_append(&chunk->consts, val, err);
  if (!rak_is_ok(err)) return 0;
  rak_value_retain(val);
  return (uint8_t) len;
}

uint16_t rak_chunk_append_instr(RakChunk *chunk, uint32_t instr, int ln, RakError *err)
{
  int len = chunk->instrs.len;
  if (len == RAK_CHUNK_MAX_INSTRS)
  {
    rak_error_set(err, "too many instructions");
    return 0;
  }
  rak_slice_ensure_append(&chunk->instrs, instr, err);
  if (!rak_is_ok(err)) return 0;
  RakSourceMap map = {
    .off = (uint16_t) len,
    .ln  = ln,
    .col = 0
  };
  if (!len)
  {
    rak_slice_ensure_append(&chunk->maps, map, err);
    if (!rak_is_ok(err)) return 0;
    return map.off;
  }
  len = chunk->maps.len;
  RakSourceMap _map = rak_slice_get(&chunk->maps, len - 1);
  if (_map.ln == ln) return map.off;
  rak_slice_ensure_append(&chunk->maps, map, err);
  if (!rak_is_ok(err)) return 0;
  return map.off;
}

int rak_chunk_get_line(const RakChunk *chunk, uint16_t off)
{
  int len = chunk->maps.len;
  int ln = -1;
  for (int i = 0; i < len; ++i)
  {
    RakSourceMap map = rak_slice_get(&chunk->maps, i);
    if (map.off < off)
    {
      ln = map.ln;
      continue;
    }
    if (map.off == off)
      ln = map.ln;
    break;
  }
  return ln;
}

void rak_chunk_clear(RakChunk *chunk)
{
  release_consts(chunk);
  rak_slice_clear(&chunk->consts);
  rak_slice_clear(&chunk->instrs);
  rak_slice_clear(&chunk->maps);
}
