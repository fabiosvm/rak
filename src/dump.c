//
// dump.c
//
// Copyright 2025 Fábio de Souza Villaça Medeiros
//
// This file is part of the Rak Project.
// For detailed license information, please refer to the LICENSE file
// located in the root directory of this project.
//

#include "rak/dump.h"
#include <stdio.h>

void rak_dump_function(RakFunction *fn)
{
  RakChunk *chunk = &fn->chunk;
  RakString *name = fn->callable.name;
  if (name)
    printf("; %.*s\n", rak_string_len(name), rak_string_chars(name));
  else
    printf("; <anonymous>\n");
  int n = chunk->instrs.len;
  int m = fn->nested.len;
  printf("; %d parameter(s), %d constant(s), %d instruction(s), %d function(s)\n",
    fn->callable.arity, chunk->consts.len, n, m);
  int ln = 1;
  for (uint16_t off = 0; off < n; ++off)
  {
    uint32_t instr = rak_slice_get(&chunk->instrs, off);
    RakOpcode op = rak_instr_opcode(instr);
    int _ln = rak_chunk_get_line(chunk, off);
    if (_ln != ln)
    {
      ln = _ln;
      printf("  %-6d %-6d ", off, ln);
    }
    else
      printf("  %-6d        ", off);
    switch (op)
    {
    case RAK_OP_NOP:
    case RAK_OP_PUSH_NIL:
    case RAK_OP_PUSH_FALSE:
    case RAK_OP_PUSH_TRUE:
    case RAK_OP_NEW_RANGE:
    case RAK_OP_POP:
    case RAK_OP_GET_ELEMENT:
    case RAK_OP_SET_ELEMENT:
    case RAK_OP_LOAD_ELEMENT:
    case RAK_OP_FETCH_ELEMENT:
    case RAK_OP_UPDATE_ELEMENT:
    case RAK_OP_UPDATE_FIELD:
    case RAK_OP_EQ:
    case RAK_OP_GT:
    case RAK_OP_GE:
    case RAK_OP_LT:
    case RAK_OP_LE:
    case RAK_OP_ADD:
    case RAK_OP_SUB:
    case RAK_OP_MUL:
    case RAK_OP_DIV:
    case RAK_OP_MOD:
    case RAK_OP_NOT:
    case RAK_OP_NEG:
    case RAK_OP_YIELD:
    case RAK_OP_RETURN:
    case RAK_OP_RETURN_NIL:
      printf("%-15s\n", rak_opcode_to_cstr(op));
      break;
    case RAK_OP_LOAD_CONST:
    case RAK_OP_LOAD_GLOBAL:
    case RAK_OP_LOAD_LOCAL:
    case RAK_OP_STORE_LOCAL:
    case RAK_OP_FETCH_LOCAL:
    case RAK_OP_REF_LOCAL:
    case RAK_OP_LOAD_LOCAL_REF:
    case RAK_OP_STORE_LOCAL_REF:
    case RAK_OP_NEW_ARRAY:
    case RAK_OP_NEW_RECORD:
    case RAK_OP_NEW_CLOSURE:
    case RAK_OP_GET_FIELD:
    case RAK_OP_PUT_FIELD:
    case RAK_OP_LOAD_FIELD:
    case RAK_OP_FETCH_FIELD:
    case RAK_OP_UNPACK_ELEMENTS:
    case RAK_OP_UNPACK_FIELDS:
    case RAK_OP_CALL:
    case RAK_OP_TAIL_CALL:
      {
        uint8_t a = rak_instr_a(instr);
        printf("%-15s %-5d\n", rak_opcode_to_cstr(op), a);
      }
      break;
    case RAK_OP_MOVE:
    case RAK_OP_ADD2:
    case RAK_OP_SUB2:
    case RAK_OP_MUL2:
    case RAK_OP_DIV2:
    case RAK_OP_MOD2:
      {
        uint8_t a = rak_instr_a(instr);
        uint8_t b = rak_instr_b(instr);
        printf("%-15s %-5d %d\n", rak_opcode_to_cstr(op), a, b);
      }
      break;
    case RAK_OP_ADD3:
    case RAK_OP_SUB3:
    case RAK_OP_MUL3:
    case RAK_OP_DIV3:
    case RAK_OP_MOD3:
      {
        uint8_t a = rak_instr_a(instr);
        uint8_t b = rak_instr_b(instr);
        uint8_t c = rak_instr_c(instr);
        printf("%-15s %-5d %-5d %d\n", rak_opcode_to_cstr(op), a, b, c);
      }
      break;
    case RAK_OP_PUSH_INT:
    case RAK_OP_JUMP:
    case RAK_OP_JUMP_IF_FALSE:
    case RAK_OP_JUMP_IF_TRUE:
      {
        uint16_t ab = rak_instr_ab(instr);
        printf("%-15s %-5d\n", rak_opcode_to_cstr(op), ab);
      }
      break;
    }
  }
  printf("\n");
  for (int i = 0; i < m; ++i)
  {
    RakFunction *nested = rak_slice_get(&fn->nested, i);
    rak_dump_function(nested);
  }
}
