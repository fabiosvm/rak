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

typedef void (*InstrHandle)(RakVM *, RakChunk *, uint32_t *, RakError *);

static inline void dispatch(RakVM *vm, RakChunk *chunk, uint32_t *ip, RakError *err);

static void do_nop(RakVM *vm, RakChunk *chunk, uint32_t *ip, RakError *err);
static void do_push_nil(RakVM *vm, RakChunk *chunk, uint32_t *ip, RakError *err);
static void do_push_false(RakVM *vm, RakChunk *chunk, uint32_t *ip, RakError *err);
static void do_push_true(RakVM *vm, RakChunk *chunk, uint32_t *ip, RakError *err);
static void do_load_const(RakVM *vm, RakChunk *chunk, uint32_t *ip, RakError *err);
static void do_pop(RakVM *vm, RakChunk *chunk, uint32_t *ip, RakError *err);
static void do_jump(RakVM *vm, RakChunk *chunk, uint32_t *ip, RakError *err);
static void do_jump_if_false(RakVM *vm, RakChunk *chunk, uint32_t *ip, RakError *err);
static void do_jump_if_true(RakVM *vm, RakChunk *chunk, uint32_t *ip, RakError *err);
static void do_eq(RakVM *vm, RakChunk *chunk, uint32_t *ip, RakError *err);
static void do_gt(RakVM *vm, RakChunk *chunk, uint32_t *ip, RakError *err);
static void do_lt(RakVM *vm, RakChunk *chunk, uint32_t *ip, RakError *err);
static void do_add(RakVM *vm, RakChunk *chunk, uint32_t *ip, RakError *err);
static void do_sub(RakVM *vm, RakChunk *chunk, uint32_t *ip, RakError *err);
static void do_mul(RakVM *vm, RakChunk *chunk, uint32_t *ip, RakError *err);
static void do_div(RakVM *vm, RakChunk *chunk, uint32_t *ip, RakError *err);
static void do_mod(RakVM *vm, RakChunk *chunk, uint32_t *ip, RakError *err);
static void do_not(RakVM *vm, RakChunk *chunk, uint32_t *ip, RakError *err);
static void do_neg(RakVM *vm, RakChunk *chunk, uint32_t *ip, RakError *err);
static void do_echo(RakVM *vm, RakChunk *chunk, uint32_t *ip, RakError *err);
static void do_halt(RakVM *vm, RakChunk *chunk, uint32_t *ip, RakError *err);

static InstrHandle dispatchTable[] = {
  [RAK_OP_NOP]           = do_nop,
  [RAK_OP_PUSH_NIL]      = do_push_nil,
  [RAK_OP_PUSH_FALSE]    = do_push_false,
  [RAK_OP_PUSH_TRUE]     = do_push_true,
  [RAK_OP_LOAD_CONST]    = do_load_const,
  [RAK_OP_POP]           = do_pop,
  [RAK_OP_JUMP]          = do_jump,
  [RAK_OP_JUMP_IF_FALSE] = do_jump_if_false,
  [RAK_OP_JUMP_IF_TRUE]  = do_jump_if_true,
  [RAK_OP_EQ]            = do_eq,
  [RAK_OP_GT]            = do_gt,
  [RAK_OP_LT]            = do_lt,
  [RAK_OP_ADD]           = do_add,
  [RAK_OP_SUB]           = do_sub,
  [RAK_OP_MUL]           = do_mul,
  [RAK_OP_DIV]           = do_div,
  [RAK_OP_MOD]           = do_mod,
  [RAK_OP_NOT]           = do_not,
  [RAK_OP_NEG]           = do_neg,
  [RAK_OP_ECHO]          = do_echo,
  [RAK_OP_HALT]          = do_halt
};

static inline void dispatch(RakVM *vm, RakChunk *chunk, uint32_t *ip, RakError *err)
{
  uint32_t instr = *ip;
  RakOpcode op = rak_instr_opcode(instr);
  InstrHandle handle = dispatchTable[op];
  handle(vm, chunk, ip, err);
}

static void do_nop(RakVM *vm, RakChunk *chunk, uint32_t *ip, RakError *err)
{
  dispatch(vm, chunk, ip + 1, err);
}

static void do_push_nil(RakVM *vm, RakChunk *chunk, uint32_t *ip, RakError *err)
{
  rak_vm_push_nil(vm, err);
  if (!rak_is_ok(err)) return;
  dispatch(vm, chunk, ip + 1, err);
}

static void do_push_false(RakVM *vm, RakChunk *chunk, uint32_t *ip, RakError *err)
{
  rak_vm_push_bool(vm, false, err);
  if (!rak_is_ok(err)) return;
  dispatch(vm, chunk, ip + 1, err);
}

static void do_push_true(RakVM *vm, RakChunk *chunk, uint32_t *ip, RakError *err)
{
  rak_vm_push_bool(vm, true, err);
  if (!rak_is_ok(err)) return;
  dispatch(vm, chunk, ip + 1, err);
}

static void do_load_const(RakVM *vm, RakChunk *chunk, uint32_t *ip, RakError *err)
{
  uint8_t idx = rak_instr_a(*ip);
  rak_vm_load_const(vm, chunk, idx, err);
  if (!rak_is_ok(err)) return;
  dispatch(vm, chunk, ip + 1, err);
}

static void do_pop(RakVM *vm, RakChunk *chunk, uint32_t *ip, RakError *err)
{
  rak_vm_pop(vm);
  dispatch(vm, chunk, ip + 1, err);
}

static void do_jump(RakVM *vm, RakChunk *chunk, uint32_t *ip, RakError *err)
{
  uint16_t off = rak_instr_ab(*ip);
  dispatch(vm, chunk, chunk->instrs.data + off, err);
}

static void do_jump_if_false(RakVM *vm, RakChunk *chunk, uint32_t *ip, RakError *err)
{
  uint16_t off = rak_instr_ab(*ip);
  RakValue val = rak_vm_get(vm, 0);
  ip = rak_is_falsy(val) ? chunk->instrs.data + off : ip + 1;
  dispatch(vm, chunk, ip, err);
}

static void do_jump_if_true(RakVM *vm, RakChunk *chunk, uint32_t *ip, RakError *err)
{
  uint16_t off = rak_instr_ab(*ip);
  RakValue val = rak_vm_get(vm, 0);
  ip = rak_is_falsy(val) ? ip + 1 : chunk->instrs.data + off;
  dispatch(vm, chunk, ip, err);
}

static void do_eq(RakVM *vm, RakChunk *chunk, uint32_t *ip, RakError *err)
{
  rak_vm_eq(vm);
  dispatch(vm, chunk, ip + 1, err);
}

static void do_gt(RakVM *vm, RakChunk *chunk, uint32_t *ip, RakError *err)
{
  rak_vm_gt(vm, err);
  if (!rak_is_ok(err)) return;
  dispatch(vm, chunk, ip + 1, err);
}

static void do_lt(RakVM *vm, RakChunk *chunk, uint32_t *ip, RakError *err)
{
  rak_vm_lt(vm, err);
  if (!rak_is_ok(err)) return;
  dispatch(vm, chunk, ip + 1, err);
}

static void do_add(RakVM *vm, RakChunk *chunk, uint32_t *ip, RakError *err)
{
  rak_vm_add(vm, err);
  if (!rak_is_ok(err)) return;
  dispatch(vm, chunk, ip + 1, err);
}

static void do_sub(RakVM *vm, RakChunk *chunk, uint32_t *ip, RakError *err)
{
  rak_vm_sub(vm, err);
  if (!rak_is_ok(err)) return;
  dispatch(vm, chunk, ip + 1, err);
}

static void do_mul(RakVM *vm, RakChunk *chunk, uint32_t *ip, RakError *err)
{
  rak_vm_mul(vm, err);
  if (!rak_is_ok(err)) return;
  dispatch(vm, chunk, ip + 1, err);
}

static void do_div(RakVM *vm, RakChunk *chunk, uint32_t *ip, RakError *err)
{
  rak_vm_div(vm, err);
  if (!rak_is_ok(err)) return;
  dispatch(vm, chunk, ip + 1, err);
}

static void do_mod(RakVM *vm, RakChunk *chunk, uint32_t *ip, RakError *err)
{
  rak_vm_mod(vm, err);
  if (!rak_is_ok(err)) return;
  dispatch(vm, chunk, ip + 1, err);
}

static void do_not(RakVM *vm, RakChunk *chunk, uint32_t *ip, RakError *err)
{
  rak_vm_not(vm);
  dispatch(vm, chunk, ip + 1, err);
}

static void do_neg(RakVM *vm, RakChunk *chunk, uint32_t *ip, RakError *err)
{
  rak_vm_neg(vm, err);
  if (!rak_is_ok(err)) return;
  dispatch(vm, chunk, ip + 1, err);
}

static void do_echo(RakVM *vm, RakChunk *chunk, uint32_t *ip, RakError *err)
{
  rak_vm_echo(vm);
  dispatch(vm, chunk, ip + 1, err);
}

static void do_halt(RakVM *vm, RakChunk *chunk, uint32_t *ip, RakError *err)
{
  (void) vm;
  (void) chunk;
  (void) ip;
  (void) err;
}

void rak_vm_init(RakVM *vm, int vstkSize, RakError *err)
{
  rak_stack_init(&vm->vstk, vstkSize, err);
}

void rak_vm_deinit(RakVM *vm)
{
  rak_stack_deinit(&vm->vstk);
}

void rak_vm_run(RakVM *vm, RakChunk *chunk, RakError *err)
{
  uint32_t *ip = chunk->instrs.data;
  dispatch(vm, chunk, ip, err);
}
