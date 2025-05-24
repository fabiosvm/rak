//
// vm.c
//
// Copyright 2025 Fábio de Souza Villaça Medeiros
//
// This file is part of the Rak Project.
// For detailed license information, please refer to the LICENSE file
// located in the root directory of this project.
//

#include "rak/vm.h"

typedef void (*InstrHandler)(RakFiber *, RakClosure *, uint32_t *, RakValue *, RakError *);

static inline void dispatch(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);

static void do_nop(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_push_nil(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_push_false(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_push_true(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_push_int(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_load_const(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_load_global(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_load_local(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_store_local(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_fetch_local(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_ref_local(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_load_local_ref(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_store_local_ref(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_check_ref(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_new_array(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_new_range(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_new_record(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_new_closure(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_move(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_dup(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_pop(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_get_element(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_set_element(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_load_element(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_fetch_element(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_update_element(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_get_field(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_put_field(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_load_field(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_fetch_field(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_update_field(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_unpack_elements(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_unpack_fields(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_jump(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_jump_if_false(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_jump_if_true(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_eq(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_gt(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_lt(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_add(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_sub(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_mul(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_div(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_mod(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_not(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_neg(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_call(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_tail_call(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_yield(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_return(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_return_nil(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);

static InstrHandler dispatchTable[] = {
  [RAK_OP_NOP]             = do_nop,
  [RAK_OP_PUSH_NIL]        = do_push_nil,
  [RAK_OP_PUSH_FALSE]      = do_push_false,
  [RAK_OP_PUSH_TRUE]       = do_push_true,
  [RAK_OP_PUSH_INT]        = do_push_int,
  [RAK_OP_LOAD_CONST]      = do_load_const,
  [RAK_OP_LOAD_GLOBAL]     = do_load_global,
  [RAK_OP_LOAD_LOCAL]      = do_load_local,
  [RAK_OP_STORE_LOCAL]     = do_store_local,
  [RAK_OP_FETCH_LOCAL]     = do_fetch_local,
  [RAK_OP_REF_LOCAL]       = do_ref_local,
  [RAK_OP_LOAD_LOCAL_REF]  = do_load_local_ref,
  [RAK_OP_STORE_LOCAL_REF] = do_store_local_ref,
  [RAK_OP_CHECK_REF]       = do_check_ref,
  [RAK_OP_NEW_ARRAY]       = do_new_array,
  [RAK_OP_NEW_RANGE]       = do_new_range,
  [RAK_OP_NEW_RECORD]      = do_new_record,
  [RAK_OP_NEW_CLOSURE]     = do_new_closure,
  [RAK_OP_MOVE]            = do_move,
  [RAK_OP_DUP]             = do_dup,
  [RAK_OP_POP]             = do_pop,
  [RAK_OP_GET_ELEMENT]     = do_get_element,
  [RAK_OP_SET_ELEMENT]     = do_set_element,
  [RAK_OP_LOAD_ELEMENT]    = do_load_element,
  [RAK_OP_FETCH_ELEMENT]   = do_fetch_element,
  [RAK_OP_UPDATE_ELEMENT]  = do_update_element,
  [RAK_OP_GET_FIELD]       = do_get_field,
  [RAK_OP_PUT_FIELD]       = do_put_field,
  [RAK_OP_LOAD_FIELD]      = do_load_field,
  [RAK_OP_FETCH_FIELD]     = do_fetch_field,
  [RAK_OP_UPDATE_FIELD]    = do_update_field,
  [RAK_OP_UNPACK_ELEMENTS] = do_unpack_elements,
  [RAK_OP_UNPACK_FIELDS]   = do_unpack_fields,
  [RAK_OP_JUMP]            = do_jump,
  [RAK_OP_JUMP_IF_FALSE]   = do_jump_if_false,
  [RAK_OP_JUMP_IF_TRUE]    = do_jump_if_true,
  [RAK_OP_EQ]              = do_eq,
  [RAK_OP_GT]              = do_gt,
  [RAK_OP_LT]              = do_lt,
  [RAK_OP_ADD]             = do_add,
  [RAK_OP_SUB]             = do_sub,
  [RAK_OP_MUL]             = do_mul,
  [RAK_OP_DIV]             = do_div,
  [RAK_OP_MOD]             = do_mod,
  [RAK_OP_NOT]             = do_not,
  [RAK_OP_NEG]             = do_neg,
  [RAK_OP_CALL]            = do_call,
  [RAK_OP_TAIL_CALL]       = do_tail_call,
  [RAK_OP_YIELD]           = do_yield,
  [RAK_OP_RETURN]          = do_return,
  [RAK_OP_RETURN_NIL]      = do_return_nil
};

static inline void dispatch(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  uint32_t instr = *ip;
  RakOpcode op = rak_instr_opcode(instr);
  InstrHandler handler = dispatchTable[op];
  handler(fiber, cl, ip, slots, err);
}

static void do_nop(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  dispatch(fiber, cl, ip + 1, slots, err);
}

static void do_push_nil(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  rak_vm_push_nil(fiber, err);
  if (!rak_is_ok(err)) return;
  dispatch(fiber, cl, ip + 1, slots, err);
}

static void do_push_false(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  rak_vm_push_bool(fiber, false, err);
  if (!rak_is_ok(err)) return;
  dispatch(fiber, cl, ip + 1, slots, err);
}

static void do_push_true(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  rak_vm_push_bool(fiber, true, err);
  if (!rak_is_ok(err)) return;
  dispatch(fiber, cl, ip + 1, slots, err);
}

static void do_push_int(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  uint16_t data = rak_instr_ab(*ip);
  rak_vm_push_number(fiber, data, err);
  if (!rak_is_ok(err)) return;
  dispatch(fiber, cl, ip + 1, slots, err);
}

static void do_load_const(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  uint8_t idx = rak_instr_a(*ip);
  RakChunk *chunk = &((RakFunction *) cl->callable)->chunk;
  rak_vm_load_const(fiber, chunk, idx, err);
  if (!rak_is_ok(err)) return;
  dispatch(fiber, cl, ip + 1, slots, err);
}

static void do_load_global(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  uint8_t idx = rak_instr_a(*ip);
  rak_vm_load_global(fiber, idx, err);
  if (!rak_is_ok(err)) return;
  dispatch(fiber, cl, ip + 1, slots, err);
}

static void do_load_local(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  uint8_t idx = rak_instr_a(*ip);
  rak_vm_load_local(fiber, slots, idx, err);
  if (!rak_is_ok(err)) return;
  dispatch(fiber, cl, ip + 1, slots, err);
}

static void do_store_local(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  uint8_t idx = rak_instr_a(*ip);
  rak_vm_store_local(fiber, slots, idx);
  dispatch(fiber, cl, ip + 1, slots, err);
}

static void do_fetch_local(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  uint8_t idx = rak_instr_a(*ip);
  rak_vm_fetch_local(fiber, slots, idx, err);
  if (!rak_is_ok(err)) return;
  dispatch(fiber, cl, ip + 1, slots, err);
}

static void do_ref_local(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  uint8_t idx = rak_instr_a(*ip);
  rak_vm_ref_local(fiber, slots, idx, err);
  if (!rak_is_ok(err)) return;
  dispatch(fiber, cl, ip + 1, slots, err);
}

static void do_load_local_ref(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  uint8_t idx = rak_instr_a(*ip);
  rak_vm_load_local_ref(fiber, slots, idx, err);
  if (!rak_is_ok(err)) return;
  dispatch(fiber, cl, ip + 1, slots, err);
}

static void do_store_local_ref(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  uint8_t idx = rak_instr_a(*ip);
  rak_vm_store_local_ref(fiber, slots, idx);
  dispatch(fiber, cl, ip + 1, slots, err);
}

static void do_check_ref(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  uint8_t idx = rak_instr_a(*ip);
  rak_vm_check_ref(slots, idx, err);
  if (!rak_is_ok(err)) return;
  dispatch(fiber, cl, ip + 1, slots, err);
}

static void do_new_array(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  uint8_t n = rak_instr_a(*ip);
  rak_vm_new_array(fiber, n, err);
  if (!rak_is_ok(err)) return;
  dispatch(fiber, cl, ip + 1, slots, err);
}

static void do_new_range(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  rak_vm_new_range(fiber, err);
  if (!rak_is_ok(err)) return;
  dispatch(fiber, cl, ip + 1, slots, err);
}

static void do_new_record(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  uint8_t n = rak_instr_a(*ip);
  rak_vm_new_record(fiber, n, err);
  if (!rak_is_ok(err)) return;
  dispatch(fiber, cl, ip + 1, slots, err);
}

static void do_new_closure(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  uint8_t idx = rak_instr_a(*ip);
  RakFunction *fn = (RakFunction *) cl->callable;
  rak_vm_new_closure(fiber, fn, idx, err);
  if (!rak_is_ok(err)) return;
  dispatch(fiber, cl, ip + 1, slots, err);
}

static void do_move(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  uint8_t dst = rak_instr_a(*ip);
  uint8_t src = rak_instr_b(*ip);
  rak_vm_move(slots, dst, src);
  if (!rak_is_ok(err)) return;
  dispatch(fiber, cl, ip + 1, slots, err);
}

static void do_dup(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  rak_vm_dup(fiber, err);
  if (!rak_is_ok(err)) return;
  dispatch(fiber, cl, ip + 1, slots, err);
}

static void do_pop(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  rak_vm_pop(fiber);
  dispatch(fiber, cl, ip + 1, slots, err);
}

static void do_get_element(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  rak_vm_get_element(fiber, err);
  if (!rak_is_ok(err)) return;
  dispatch(fiber, cl, ip + 1, slots, err);
}

static void do_set_element(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  rak_vm_set_element(fiber, err);
  if (!rak_is_ok(err)) return;
  dispatch(fiber, cl, ip + 1, slots, err);
}

static void do_load_element(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  rak_vm_load_element(fiber, err);
  if (!rak_is_ok(err)) return;
  dispatch(fiber, cl, ip + 1, slots, err);
}

static void do_fetch_element(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  rak_vm_fetch_element(fiber, err);
  if (!rak_is_ok(err)) return;
  dispatch(fiber, cl, ip + 1, slots, err);
}

static void do_update_element(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  rak_vm_update_element(fiber, err);
  if (!rak_is_ok(err)) return;
  dispatch(fiber, cl, ip + 1, slots, err);
}

static void do_get_field(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  uint8_t idx = rak_instr_a(*ip);
  RakChunk *chunk = &((RakFunction *) cl->callable)->chunk;
  rak_vm_get_field(fiber, chunk, idx, err);
  if (!rak_is_ok(err)) return;
  dispatch(fiber, cl, ip + 1, slots, err);
}

static void do_put_field(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  uint8_t idx = rak_instr_a(*ip);
  RakChunk *chunk = &((RakFunction *) cl->callable)->chunk;
  rak_vm_put_field(fiber, chunk, idx, err);
  if (!rak_is_ok(err)) return;
  dispatch(fiber, cl, ip + 1, slots, err);
}

static void do_load_field(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  uint8_t idx = rak_instr_a(*ip);
  RakChunk *chunk = &((RakFunction *) cl->callable)->chunk;
  rak_vm_load_field(fiber, chunk, idx, err);
  if (!rak_is_ok(err)) return;
  dispatch(fiber, cl, ip + 1, slots, err);
}

static void do_fetch_field(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  uint8_t idx = rak_instr_a(*ip);
  RakChunk *chunk = &((RakFunction *) cl->callable)->chunk;
  rak_vm_fetch_field(fiber, chunk, idx, err);
  if (!rak_is_ok(err)) return;
  dispatch(fiber, cl, ip + 1, slots, err);
}

static void do_update_field(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  rak_vm_update_field(fiber, err);
  if (!rak_is_ok(err)) return;
  dispatch(fiber, cl, ip + 1, slots, err);
}

static void do_unpack_elements(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  uint8_t n = rak_instr_a(*ip);
  rak_vm_unpack_elements(fiber, n, err);
  if (!rak_is_ok(err)) return;
  dispatch(fiber, cl, ip + 1, slots, err);
}

static void do_unpack_fields(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  uint8_t n = rak_instr_a(*ip);
  rak_vm_unpack_fields(fiber, n, err);
  if (!rak_is_ok(err)) return;
  dispatch(fiber, cl, ip + 1, slots, err);
}

static void do_jump(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  uint16_t off = rak_instr_ab(*ip);
  RakChunk *chunk = &((RakFunction *) cl->callable)->chunk;
  ip = &chunk->instrs.data[off];
  dispatch(fiber, cl, ip, slots, err);
}

static void do_jump_if_false(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  uint16_t off = rak_instr_ab(*ip);
  RakValue val = rak_vm_get(fiber, 0);
  RakChunk *chunk = &((RakFunction *) cl->callable)->chunk;
  ip = rak_is_falsy(val) ? chunk->instrs.data + off : ip + 1;
  dispatch(fiber, cl, ip, slots, err);
}

static void do_jump_if_true(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  uint16_t off = rak_instr_ab(*ip);
  RakValue val = rak_vm_get(fiber, 0);
  RakChunk *chunk = &((RakFunction *) cl->callable)->chunk;
  ip = rak_is_falsy(val) ? ip + 1 : chunk->instrs.data + off;
  dispatch(fiber, cl, ip, slots, err);
}

static void do_eq(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  rak_vm_eq(fiber);
  dispatch(fiber, cl, ip + 1, slots, err);
}

static void do_gt(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  rak_vm_gt(fiber, err);
  if (!rak_is_ok(err)) return;
  dispatch(fiber, cl, ip + 1, slots, err);
}

static void do_lt(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  rak_vm_lt(fiber, err);
  if (!rak_is_ok(err)) return;
  dispatch(fiber, cl, ip + 1, slots, err);
}

static void do_add(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  rak_vm_add(fiber, err);
  if (!rak_is_ok(err)) return;
  dispatch(fiber, cl, ip + 1, slots, err);
}

static void do_sub(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  rak_vm_sub(fiber, err);
  if (!rak_is_ok(err)) return;
  dispatch(fiber, cl, ip + 1, slots, err);
}

static void do_mul(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  rak_vm_mul(fiber, err);
  if (!rak_is_ok(err)) return;
  dispatch(fiber, cl, ip + 1, slots, err);
}

static void do_div(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  rak_vm_div(fiber, err);
  if (!rak_is_ok(err)) return;
  dispatch(fiber, cl, ip + 1, slots, err);
}

static void do_mod(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  rak_vm_mod(fiber, err);
  if (!rak_is_ok(err)) return;
  dispatch(fiber, cl, ip + 1, slots, err);
}

static void do_not(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  rak_vm_not(fiber);
  dispatch(fiber, cl, ip + 1, slots, err);
}

static void do_neg(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  rak_vm_neg(fiber, err);
  if (!rak_is_ok(err)) return;
  dispatch(fiber, cl, ip + 1, slots, err);
}

static void do_call(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  (void) cl;
  (void) slots;
  uint8_t n = rak_instr_a(*ip);
  RakCallFrame *frame = &rak_stack_get(&fiber->cstk, 0);
  frame->ip = ip + 1;
  rak_vm_call(fiber, n, err);
}

static void do_tail_call(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  (void) cl;
  uint8_t n = rak_instr_a(*ip);
  rak_vm_tail_call(fiber, slots, n, err);
}

static void do_yield(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  (void) cl;
  (void) slots;
  (void) err;
  RakCallFrame *frame = &rak_stack_get(&fiber->cstk, 0);
  frame->ip = ip + 1;
  rak_vm_yield(fiber);
}

static void do_return(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  (void) ip;
  (void) err;
  rak_vm_return(fiber, cl, slots);
}

static void do_return_nil(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  (void) ip;
  rak_vm_return_nil(fiber, cl, slots, err);
}

void rak_vm_dispatch(RakFiber *fiber, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  dispatch(fiber, cl, ip, slots, err);
}
