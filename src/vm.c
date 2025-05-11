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

typedef void (*InstrHandler)(RakVM *, RakClosure *, uint32_t *, RakValue *, RakError *);

static inline void release_values(RakVM *vm);
static inline void dispatch(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);

static void do_nop(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_push_nil(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_push_false(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_push_true(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_push_int(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_load_const(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_load_global(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_load_local(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_store_local(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_fetch_local(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_new_array(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_new_range(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_new_record(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_new_closure(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_dup(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_pop(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_get_element(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_set_element(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_load_element(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_fetch_element(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_update_element(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_get_field(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_put_field(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_load_field(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_fetch_field(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_update_field(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_unpack_elements(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_unpack_fields(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_jump(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_jump_if_false(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_jump_if_true(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_eq(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_gt(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_lt(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_add(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_sub(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_mul(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_div(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_mod(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_not(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_neg(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_call(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_tail_call(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);
static void do_return(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err);

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
  [RAK_OP_NEW_ARRAY]       = do_new_array,
  [RAK_OP_NEW_RANGE]       = do_new_range,
  [RAK_OP_NEW_RECORD]      = do_new_record,
  [RAK_OP_NEW_CLOSURE]     = do_new_closure,
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
  [RAK_OP_RETURN]          = do_return
};

static inline void release_values(RakVM *vm)
{
  while (!rak_stack_is_empty(&vm->vstk))
    rak_vm_pop(vm);
}

static inline void dispatch(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  uint32_t instr = *ip;
  RakOpcode op = rak_instr_opcode(instr);
  InstrHandler handler = dispatchTable[op];
  handler(vm, cl, ip, slots, err);
}

static void do_nop(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  dispatch(vm, cl, ip + 1, slots, err);
}

static void do_push_nil(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  rak_vm_push_nil(vm, err);
  if (!rak_is_ok(err)) return;
  dispatch(vm, cl, ip + 1, slots, err);
}

static void do_push_false(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  rak_vm_push_bool(vm, false, err);
  if (!rak_is_ok(err)) return;
  dispatch(vm, cl, ip + 1, slots, err);
}

static void do_push_true(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  rak_vm_push_bool(vm, true, err);
  if (!rak_is_ok(err)) return;
  dispatch(vm, cl, ip + 1, slots, err);
}

static void do_push_int(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  uint16_t data = rak_instr_ab(*ip);
  rak_vm_push_number(vm, data, err);
  if (!rak_is_ok(err)) return;
  dispatch(vm, cl, ip + 1, slots, err);
}

static void do_load_const(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  uint8_t idx = rak_instr_a(*ip);
  RakChunk *chunk = &((RakFunction *) cl->callable)->chunk;
  rak_vm_load_const(vm, chunk, idx, err);
  if (!rak_is_ok(err)) return;
  dispatch(vm, cl, ip + 1, slots, err);
}

static void do_load_global(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  uint8_t idx = rak_instr_a(*ip);
  rak_vm_load_global(vm, idx, err);
  if (!rak_is_ok(err)) return;
  dispatch(vm, cl, ip + 1, slots, err);
}

static void do_load_local(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  uint8_t idx = rak_instr_a(*ip);
  rak_vm_load_local(vm, slots, idx, err);
  if (!rak_is_ok(err)) return;
  dispatch(vm, cl, ip + 1, slots, err);
}

static void do_store_local(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  uint8_t idx = rak_instr_a(*ip);
  rak_vm_store_local(vm, slots, idx);
  dispatch(vm, cl, ip + 1, slots, err);
}

static void do_fetch_local(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  uint8_t idx = rak_instr_a(*ip);
  rak_vm_fetch_local(vm, slots, idx, err);
  if (!rak_is_ok(err)) return;
  dispatch(vm, cl, ip + 1, slots, err);
}

static void do_new_array(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  uint8_t n = rak_instr_a(*ip);
  rak_vm_new_array(vm, n, err);
  if (!rak_is_ok(err)) return;
  dispatch(vm, cl, ip + 1, slots, err);
}

static void do_new_range(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  rak_vm_new_range(vm, err);
  if (!rak_is_ok(err)) return;
  dispatch(vm, cl, ip + 1, slots, err);
}

static void do_new_record(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  uint8_t n = rak_instr_a(*ip);
  rak_vm_new_record(vm, n, err);
  if (!rak_is_ok(err)) return;
  dispatch(vm, cl, ip + 1, slots, err);
}

static void do_new_closure(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  uint8_t idx = rak_instr_a(*ip);
  RakFunction *fn = (RakFunction *) cl->callable;
  rak_vm_new_closure(vm, fn, idx, err);
  if (!rak_is_ok(err)) return;
  dispatch(vm, cl, ip + 1, slots, err);
}

static void do_dup(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  rak_vm_dup(vm, err);
  if (!rak_is_ok(err)) return;
  dispatch(vm, cl, ip + 1, slots, err);
}

static void do_pop(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  rak_vm_pop(vm);
  dispatch(vm, cl, ip + 1, slots, err);
}

static void do_get_element(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  rak_vm_get_element(vm, err);
  if (!rak_is_ok(err)) return;
  dispatch(vm, cl, ip + 1, slots, err);
}

static void do_set_element(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  rak_vm_set_element(vm, err);
  if (!rak_is_ok(err)) return;
  dispatch(vm, cl, ip + 1, slots, err);
}

static void do_load_element(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  rak_vm_load_element(vm, err);
  if (!rak_is_ok(err)) return;
  dispatch(vm, cl, ip + 1, slots, err);
}

static void do_fetch_element(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  rak_vm_fetch_element(vm, err);
  if (!rak_is_ok(err)) return;
  dispatch(vm, cl, ip + 1, slots, err);
}

static void do_update_element(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  rak_vm_update_element(vm, err);
  if (!rak_is_ok(err)) return;
  dispatch(vm, cl, ip + 1, slots, err);
}

static void do_get_field(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  uint8_t idx = rak_instr_a(*ip);
  RakChunk *chunk = &((RakFunction *) cl->callable)->chunk;
  rak_vm_get_field(vm, chunk, idx, err);
  if (!rak_is_ok(err)) return;
  dispatch(vm, cl, ip + 1, slots, err);
}

static void do_put_field(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  uint8_t idx = rak_instr_a(*ip);
  RakChunk *chunk = &((RakFunction *) cl->callable)->chunk;
  rak_vm_put_field(vm, chunk, idx, err);
  if (!rak_is_ok(err)) return;
  dispatch(vm, cl, ip + 1, slots, err);
}

static void do_load_field(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  uint8_t idx = rak_instr_a(*ip);
  RakChunk *chunk = &((RakFunction *) cl->callable)->chunk;
  rak_vm_load_field(vm, chunk, idx, err);
  if (!rak_is_ok(err)) return;
  dispatch(vm, cl, ip + 1, slots, err);
}

static void do_fetch_field(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  uint8_t idx = rak_instr_a(*ip);
  RakChunk *chunk = &((RakFunction *) cl->callable)->chunk;
  rak_vm_fetch_field(vm, chunk, idx, err);
  if (!rak_is_ok(err)) return;
  dispatch(vm, cl, ip + 1, slots, err);
}

static void do_update_field(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  rak_vm_update_field(vm, err);
  if (!rak_is_ok(err)) return;
  dispatch(vm, cl, ip + 1, slots, err);
}

static void do_unpack_elements(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  uint8_t n = rak_instr_a(*ip);
  rak_vm_unpack_elements(vm, n, err);
  if (!rak_is_ok(err)) return;
  dispatch(vm, cl, ip + 1, slots, err);
}

static void do_unpack_fields(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  uint8_t n = rak_instr_a(*ip);
  rak_vm_unpack_fields(vm, n, err);
  if (!rak_is_ok(err)) return;
  dispatch(vm, cl, ip + 1, slots, err);
}

static void do_jump(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  uint16_t off = rak_instr_ab(*ip);
  RakChunk *chunk = &((RakFunction *) cl->callable)->chunk;
  ip = &chunk->instrs.data[off];
  dispatch(vm, cl, ip, slots, err);
}

static void do_jump_if_false(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  uint16_t off = rak_instr_ab(*ip);
  RakValue val = rak_vm_get(vm, 0);
  RakChunk *chunk = &((RakFunction *) cl->callable)->chunk;
  ip = rak_is_falsy(val) ? chunk->instrs.data + off : ip + 1;
  dispatch(vm, cl, ip, slots, err);
}

static void do_jump_if_true(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  uint16_t off = rak_instr_ab(*ip);
  RakValue val = rak_vm_get(vm, 0);
  RakChunk *chunk = &((RakFunction *) cl->callable)->chunk;
  ip = rak_is_falsy(val) ? ip + 1 : chunk->instrs.data + off;
  dispatch(vm, cl, ip, slots, err);
}

static void do_eq(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  rak_vm_eq(vm);
  dispatch(vm, cl, ip + 1, slots, err);
}

static void do_gt(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  rak_vm_gt(vm, err);
  if (!rak_is_ok(err)) return;
  dispatch(vm, cl, ip + 1, slots, err);
}

static void do_lt(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  rak_vm_lt(vm, err);
  if (!rak_is_ok(err)) return;
  dispatch(vm, cl, ip + 1, slots, err);
}

static void do_add(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  rak_vm_add(vm, err);
  if (!rak_is_ok(err)) return;
  dispatch(vm, cl, ip + 1, slots, err);
}

static void do_sub(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  rak_vm_sub(vm, err);
  if (!rak_is_ok(err)) return;
  dispatch(vm, cl, ip + 1, slots, err);
}

static void do_mul(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  rak_vm_mul(vm, err);
  if (!rak_is_ok(err)) return;
  dispatch(vm, cl, ip + 1, slots, err);
}

static void do_div(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  rak_vm_div(vm, err);
  if (!rak_is_ok(err)) return;
  dispatch(vm, cl, ip + 1, slots, err);
}

static void do_mod(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  rak_vm_mod(vm, err);
  if (!rak_is_ok(err)) return;
  dispatch(vm, cl, ip + 1, slots, err);
}

static void do_not(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  rak_vm_not(vm);
  dispatch(vm, cl, ip + 1, slots, err);
}

static void do_neg(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  rak_vm_neg(vm, err);
  if (!rak_is_ok(err)) return;
  dispatch(vm, cl, ip + 1, slots, err);
}

static void do_call(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  (void) cl;
  (void) slots;
  uint8_t n = rak_instr_a(*ip);
  RakCallFrame *frame = &rak_stack_get(&vm->cstk, 0);
  frame->ip = ip + 1;
  rak_vm_call(vm, n, err);
}

static void do_tail_call(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  (void) cl;
  uint8_t n = rak_instr_a(*ip);
  rak_vm_tail_call(vm, slots, n, err);
}

static void do_return(RakVM *vm, RakClosure *cl, uint32_t *ip, RakValue *slots, RakError *err)
{
  (void) ip;
  (void) err;
  rak_vm_return(vm, cl, slots);
}

void rak_vm_init(RakVM *vm, int vstkSize, int cstkSize, RakError *err)
{
  rak_stack_init(&vm->vstk, vstkSize, err);
  if (!rak_is_ok(err)) return;
  rak_stack_init(&vm->cstk, cstkSize, err);
  if (rak_is_ok(err)) return;
  rak_stack_deinit(&vm->vstk);
}

void rak_vm_deinit(RakVM *vm)
{
  release_values(vm);
  rak_stack_deinit(&vm->vstk);
  rak_stack_deinit(&vm->cstk);
}

void rak_vm_run(RakVM *vm, RakFunction *fn, RakError *err)
{
  RakClosure *cl = rak_closure_new(RAK_CALLABLE_TYPE_FUNCTION, &fn->callable, err);
  if (!rak_is_ok(err)) return;
  rak_vm_push_object(vm, rak_closure_value(cl), err);
  if (!rak_is_ok(err))
  {
    rak_closure_free(cl);
    return;
  }
  rak_vm_call(vm, 0, err);
  if (!rak_is_ok(err)) return;
  rak_vm_resume(vm, err);
}

void rak_vm_resume(RakVM *vm, RakError *err)
{
  while (!rak_stack_is_empty(&vm->cstk))
  {
    RakCallFrame frame = rak_stack_get(&vm->cstk, 0);
    RakClosure *cl = frame.cl;
    uint32_t *ip = frame.ip;
    RakValue *slots = frame.slots;
    if (cl->type == RAK_CALLABLE_TYPE_FUNCTION)
    {
      dispatch(vm, cl, ip, slots, err);
      if (!rak_is_ok(err)) return;
      continue;
    }
    RakNativeFunction *native = (RakNativeFunction *) cl->callable;
    native->call(vm, cl, slots, err);
    if (!rak_is_ok(err)) return;
  }
}
