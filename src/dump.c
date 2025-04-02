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

void rak_dump_chunk(RakChunk *chunk)
{
  printf("%d constant(s)\n", chunk->consts.len);
  int n = chunk->instrs.len;
  printf("%d instruction(s)\n", n);
  for (int i = 0; i < n; ++i)
  {
    uint32_t instr = rak_slice_get(&chunk->instrs, i);
    RakOpcode op = rak_instr_opcode(instr);
    printf("[%04d] ", i);
    switch (op)
    {
    case RAK_OP_PUSH_NIL:
    case RAK_OP_POP:
    case RAK_OP_ADD:
    case RAK_OP_SUB:
    case RAK_OP_MUL:
    case RAK_OP_DIV:
    case RAK_OP_MOD:
    case RAK_OP_NEG:
    case RAK_OP_HALT:
      printf("%-15s\n", rak_opcode_to_cstr(op));
      break;
    case RAK_OP_LOAD_CONST:
      {
        uint8_t a = rak_instr_a(instr);
        printf("%-15s %-5d\n", rak_opcode_to_cstr(op), a);
      }
      break;
    }
  }
  printf("\n");
}
