//
// chunk.h
//
// Copyright 2025 Fábio de Souza Villaça Medeiros
//
// This file is part of the Rak Project.
// For detailed license information, please refer to the LICENSE file
// located in the root directory of this project.
//

#ifndef RAK_CHUNK_H
#define RAK_CHUNK_H

#include "slice.h"
#include "value.h"

#define rak_instr_fmt0(op)          ((uint32_t) ((op) & 0xff))
#define rak_instr_fmt1(op, a)       ((((uint32_t) ((a) & 0xff)) << 8) | ((op) & 0xff))
#define rak_instr_fmt2(op, a, b)    (((((uint32_t) ((b) & 0xff)) << 8) | ((a) & 0xff)) << 8 | ((op) & 0xff))
#define rak_instr_fmt3(op, a, b, c) ((((((uint32_t) ((c) & 0xff)) << 8) | ((b) & 0xff)) << 8 | ((a) & 0xff)) << 8 | ((op) & 0xff))
#define rak_instr_fmt4(op, ab)      ((((uint32_t) ((ab) & 0xffff)) << 8) | ((op) & 0xff))

#define rak_instr_opcode(i) ((RakOpcode) ((i) & 0xff))
#define rak_instr_a(i)      ((uint8_t) (((i) >> 8) & 0xff))
#define rak_instr_b(i)      ((uint8_t) (((i) >> 16) & 0xff))
#define rak_instr_c(i)      ((uint8_t) (((i) >> 24) & 0xff))
#define rak_instr_ab(i)     ((uint16_t) (((i) >> 8) & 0xffff))

#define rak_push_nil_instr()    rak_instr_fmt0(RAK_OP_PUSH_NIL)
#define rak_push_false_instr()  rak_instr_fmt0(RAK_OP_PUSH_FALSE)
#define rak_push_true_instr()   rak_instr_fmt0(RAK_OP_PUSH_TRUE)
#define rak_load_const_instr(i) rak_instr_fmt1(RAK_OP_LOAD_CONST, (i))
#define rak_pop_instr()         rak_instr_fmt0(RAK_OP_POP)
#define rak_eq_instr()          rak_instr_fmt0(RAK_OP_EQ)
#define rak_gt_instr()          rak_instr_fmt0(RAK_OP_GT)
#define rak_lt_instr()          rak_instr_fmt0(RAK_OP_LT)
#define rak_add_instr()         rak_instr_fmt0(RAK_OP_ADD)
#define rak_sub_instr()         rak_instr_fmt0(RAK_OP_SUB)
#define rak_mul_instr()         rak_instr_fmt0(RAK_OP_MUL)
#define rak_div_instr()         rak_instr_fmt0(RAK_OP_DIV)
#define rak_mod_instr()         rak_instr_fmt0(RAK_OP_MOD)
#define rak_not_instr()         rak_instr_fmt0(RAK_OP_NOT)
#define rak_neg_instr()         rak_instr_fmt0(RAK_OP_NEG)
#define rak_halt_instr()        rak_instr_fmt0(RAK_OP_HALT)

typedef enum
{
  RAK_OP_PUSH_NIL,
  RAK_OP_PUSH_FALSE,
  RAK_OP_PUSH_TRUE,
  RAK_OP_LOAD_CONST,
  RAK_OP_POP,
  RAK_OP_EQ,
  RAK_OP_GT,
  RAK_OP_LT,
  RAK_OP_ADD,
  RAK_OP_SUB,
  RAK_OP_MUL,
  RAK_OP_DIV,
  RAK_OP_MOD,
  RAK_OP_NOT,
  RAK_OP_NEG,
  RAK_OP_HALT
} RakOpcode;

typedef struct
{
  RakSlice(RakValue) consts;
  RakSlice(uint32_t) instrs;
} RakChunk;

const char *rak_opcode_to_cstr(RakOpcode op);
void rak_chunk_init(RakChunk *chunk, RakError *err);
void rak_chunk_deinit(RakChunk *chunk);
int rak_chunk_append_const(RakChunk *chunk, RakValue val, RakError *err);
void rak_chunk_append_instr(RakChunk *chunk, uint32_t instr, RakError *err);
void rak_chunk_clear(RakChunk *chunk);

#endif // RAK_CHUNK_H
