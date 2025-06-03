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

#define RAK_CHUNK_MAX_CONSTS (UINT8_MAX + 1)
#define RAK_CHUNK_MAX_INSTRS (UINT16_MAX + 1)

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

#define rak_nop_instr()              rak_instr_fmt0(RAK_OP_NOP)
#define rak_push_nil_instr()         rak_instr_fmt0(RAK_OP_PUSH_NIL)
#define rak_push_false_instr()       rak_instr_fmt0(RAK_OP_PUSH_FALSE)
#define rak_push_true_instr()        rak_instr_fmt0(RAK_OP_PUSH_TRUE)
#define rak_push_int_instr(d)        rak_instr_fmt4(RAK_OP_PUSH_INT, (d))
#define rak_load_const_instr(i)      rak_instr_fmt1(RAK_OP_LOAD_CONST, (i))
#define rak_load_global_instr(i)     rak_instr_fmt1(RAK_OP_LOAD_GLOBAL, (i))
#define rak_load_local_instr(i)      rak_instr_fmt1(RAK_OP_LOAD_LOCAL, (i))
#define rak_store_local_instr(i)     rak_instr_fmt1(RAK_OP_STORE_LOCAL, (i))
#define rak_fetch_local_instr(i)     rak_instr_fmt1(RAK_OP_FETCH_LOCAL, (i))
#define rak_ref_local_instr(i)       rak_instr_fmt1(RAK_OP_REF_LOCAL, (i))
#define rak_load_local_ref_instr(i)  rak_instr_fmt1(RAK_OP_LOAD_LOCAL_REF, (i))
#define rak_store_local_ref_instr(i) rak_instr_fmt1(RAK_OP_STORE_LOCAL_REF, (i))
#define rak_new_array_instr(n)       rak_instr_fmt1(RAK_OP_NEW_ARRAY, (n))
#define rak_new_range_instr()        rak_instr_fmt0(RAK_OP_NEW_RANGE)
#define rak_new_record_instr(n)      rak_instr_fmt1(RAK_OP_NEW_RECORD, (n))
#define rak_new_closure_instr(i)     rak_instr_fmt1(RAK_OP_NEW_CLOSURE, (i))
#define rak_move_instr(d, s)         rak_instr_fmt2(RAK_OP_MOVE, (d), (s))
#define rak_dup_instr()              rak_instr_fmt0(RAK_OP_DUP)
#define rak_pop_instr()              rak_instr_fmt0(RAK_OP_POP)
#define rak_get_element_instr()      rak_instr_fmt0(RAK_OP_GET_ELEMENT)
#define rak_set_element_instr()      rak_instr_fmt0(RAK_OP_SET_ELEMENT)
#define rak_load_element_instr()     rak_instr_fmt0(RAK_OP_LOAD_ELEMENT)
#define rak_fetch_element_instr()    rak_instr_fmt0(RAK_OP_FETCH_ELEMENT)
#define rak_update_element_instr()   rak_instr_fmt0(RAK_OP_UPDATE_ELEMENT)
#define rak_get_field_instr(i)       rak_instr_fmt1(RAK_OP_GET_FIELD, (i))
#define rak_put_field_instr(i)       rak_instr_fmt1(RAK_OP_PUT_FIELD, (i))
#define rak_load_field_instr(i)      rak_instr_fmt1(RAK_OP_LOAD_FIELD, (i))
#define rak_fetch_field_instr(i)     rak_instr_fmt1(RAK_OP_FETCH_FIELD, (i))
#define rak_update_field_instr()     rak_instr_fmt0(RAK_OP_UPDATE_FIELD)
#define rak_unpack_elements_instr(n) rak_instr_fmt1(RAK_OP_UNPACK_ELEMENTS, (n))
#define rak_unpack_fields_instr(n)   rak_instr_fmt1(RAK_OP_UNPACK_FIELDS, (n))
#define rak_jump_instr(o)            rak_instr_fmt4(RAK_OP_JUMP, (o))
#define rak_jump_if_false_instr(o)   rak_instr_fmt4(RAK_OP_JUMP_IF_FALSE, (o))
#define rak_jump_if_true_instr(o)    rak_instr_fmt4(RAK_OP_JUMP_IF_TRUE, (o))
#define rak_eq_instr()               rak_instr_fmt0(RAK_OP_EQ)
#define rak_gt_instr()               rak_instr_fmt0(RAK_OP_GT)
#define rak_ge_instr()               rak_instr_fmt0(RAK_OP_GE)
#define rak_lt_instr()               rak_instr_fmt0(RAK_OP_LT)
#define rak_le_instr()               rak_instr_fmt0(RAK_OP_LE)
#define rak_add_instr()              rak_instr_fmt0(RAK_OP_ADD)
#define rak_add2_instr(l, r)         rak_instr_fmt2(RAK_OP_ADD2, (l), (r))
#define rak_add3_instr(d, l, r)      rak_instr_fmt3(RAK_OP_ADD3, (d), (l), (r))
#define rak_sub_instr()              rak_instr_fmt0(RAK_OP_SUB)
#define rak_sub2_instr(l, r)         rak_instr_fmt2(RAK_OP_SUB2, (l), (r))
#define rak_sub3_instr(d, l, r)      rak_instr_fmt3(RAK_OP_SUB3, (d), (l), (r))
#define rak_mul_instr()              rak_instr_fmt0(RAK_OP_MUL)
#define rak_mul2_instr(l, r)         rak_instr_fmt2(RAK_OP_MUL2, (l), (r))
#define rak_mul3_instr(d, l, r)      rak_instr_fmt3(RAK_OP_MUL3, (d), (l), (r))
#define rak_div_instr()              rak_instr_fmt0(RAK_OP_DIV)
#define rak_div2_instr(l, r)         rak_instr_fmt2(RAK_OP_DIV2, (l), (r))
#define rak_div3_instr(d, l, r)      rak_instr_fmt3(RAK_OP_DIV3, (d), (l), (r))
#define rak_mod_instr()              rak_instr_fmt0(RAK_OP_MOD)
#define rak_mod2_instr(l, r)         rak_instr_fmt2(RAK_OP_MOD2, (l), (r))
#define rak_mod3_instr(d, l, r)      rak_instr_fmt3(RAK_OP_MOD3, (d), (l), (r))
#define rak_not_instr()              rak_instr_fmt0(RAK_OP_NOT)
#define rak_neg_instr()              rak_instr_fmt0(RAK_OP_NEG)
#define rak_call_instr(n)            rak_instr_fmt1(RAK_OP_CALL, (n))
#define rak_tail_call_instr(n)       rak_instr_fmt1(RAK_OP_TAIL_CALL, (n))
#define rak_yield_instr()            rak_instr_fmt0(RAK_OP_YIELD)
#define rak_return_instr()           rak_instr_fmt0(RAK_OP_RETURN)
#define rak_return_nil_instr()       rak_instr_fmt0(RAK_OP_RETURN_NIL)

typedef enum
{
  RAK_OP_NOP,
  RAK_OP_PUSH_NIL,
  RAK_OP_PUSH_FALSE,
  RAK_OP_PUSH_TRUE,
  RAK_OP_PUSH_INT,
  RAK_OP_LOAD_CONST,
  RAK_OP_LOAD_GLOBAL,
  RAK_OP_LOAD_LOCAL,
  RAK_OP_STORE_LOCAL,
  RAK_OP_FETCH_LOCAL,
  RAK_OP_REF_LOCAL,
  RAK_OP_LOAD_LOCAL_REF,
  RAK_OP_STORE_LOCAL_REF,
  RAK_OP_NEW_ARRAY,
  RAK_OP_NEW_RANGE,
  RAK_OP_NEW_RECORD,
  RAK_OP_NEW_CLOSURE,
  RAK_OP_MOVE,
  RAK_OP_DUP,
  RAK_OP_POP,
  RAK_OP_GET_ELEMENT,
  RAK_OP_SET_ELEMENT,
  RAK_OP_LOAD_ELEMENT,
  RAK_OP_FETCH_ELEMENT,
  RAK_OP_UPDATE_ELEMENT,
  RAK_OP_GET_FIELD,
  RAK_OP_PUT_FIELD,
  RAK_OP_LOAD_FIELD,
  RAK_OP_FETCH_FIELD,
  RAK_OP_UPDATE_FIELD,
  RAK_OP_UNPACK_ELEMENTS,
  RAK_OP_UNPACK_FIELDS,
  RAK_OP_JUMP,
  RAK_OP_JUMP_IF_FALSE,
  RAK_OP_JUMP_IF_TRUE,
  RAK_OP_EQ,
  RAK_OP_GT,
  RAK_OP_GE,
  RAK_OP_LT,
  RAK_OP_LE,
  RAK_OP_ADD,
  RAK_OP_ADD2,
  RAK_OP_ADD3,
  RAK_OP_SUB,
  RAK_OP_SUB2,
  RAK_OP_SUB3,
  RAK_OP_MUL,
  RAK_OP_MUL2,
  RAK_OP_MUL3,
  RAK_OP_DIV,
  RAK_OP_DIV2,
  RAK_OP_DIV3,
  RAK_OP_MOD,
  RAK_OP_MOD2,
  RAK_OP_MOD3,
  RAK_OP_NOT,
  RAK_OP_NEG,
  RAK_OP_CALL,
  RAK_OP_TAIL_CALL,
  RAK_OP_YIELD,
  RAK_OP_RETURN,
  RAK_OP_RETURN_NIL
} RakOpcode;

typedef struct
{
  uint16_t off;
  int      ln;
  int      col;
} RakSourceMap;

typedef struct
{
  RakSlice(RakValue)     consts;
  RakSlice(uint32_t)     instrs;
  RakSlice(RakSourceMap) maps;
} RakChunk;

const char *rak_opcode_to_cstr(RakOpcode op);
void rak_chunk_init(RakChunk *chunk, RakError *err);
void rak_chunk_deinit(RakChunk *chunk);
uint8_t rak_chunk_append_const(RakChunk *chunk, RakValue val, RakError *err);
uint16_t rak_chunk_append_instr(RakChunk *chunk, uint32_t instr, int ln, RakError *err);
int rak_chunk_get_line(const RakChunk *chunk, uint16_t off);
void rak_chunk_clear(RakChunk *chunk);

#endif // RAK_CHUNK_H
