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

typedef void (*RakInstrHandle)(RakVM *, RakChunk *, uint32_t *, RakError *);

static inline void dispatch(RakVM *vm, RakChunk *chunk, uint32_t *ip, RakError *err);

static void do_push_nil(RakVM *vm, RakChunk *chunk, uint32_t *ip, RakError *err);
static void do_load_const(RakVM *vm, RakChunk *chunk, uint32_t *ip, RakError *err);
static void do_pop(RakVM *vm, RakChunk *chunk, uint32_t *ip, RakError *err);
static void do_add(RakVM *vm, RakChunk *chunk, uint32_t *ip, RakError *err);
static void do_sub(RakVM *vm, RakChunk *chunk, uint32_t *ip, RakError *err);
static void do_mul(RakVM *vm, RakChunk *chunk, uint32_t *ip, RakError *err);
static void do_div(RakVM *vm, RakChunk *chunk, uint32_t *ip, RakError *err);
static void do_mod(RakVM *vm, RakChunk *chunk, uint32_t *ip, RakError *err);
static void do_neg(RakVM *vm, RakChunk *chunk, uint32_t *ip, RakError *err);
static void do_halt(RakVM *vm, RakChunk *chunk, uint32_t *ip, RakError *err);

static RakInstrHandle dispatchTable[] = {
  [RAK_OP_PUSH_NIL]   = do_push_nil,
  [RAK_OP_LOAD_CONST] = do_load_const,
  [RAK_OP_POP]        = do_pop,
  [RAK_OP_ADD]        = do_add,
  [RAK_OP_SUB]        = do_sub,
  [RAK_OP_MUL]        = do_mul,
  [RAK_OP_DIV]        = do_div,
  [RAK_OP_MOD]        = do_mod,
  [RAK_OP_NEG]        = do_neg,
  [RAK_OP_HALT]       = do_halt
};

static inline void dispatch(RakVM *vm, RakChunk *chunk, uint32_t *ip, RakError *err)
{
  uint32_t instr = *ip;
  RakOpcode op = rak_instr_opcode(instr);
  RakInstrHandle handle = dispatchTable[op];
  handle(vm, chunk, ip, err);
}

static void do_push_nil(RakVM *vm, RakChunk *chunk, uint32_t *ip, RakError *err)
{
  rak_vm_push_nil(vm, err);
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

static void do_neg(RakVM *vm, RakChunk *chunk, uint32_t *ip, RakError *err)
{
  rak_vm_neg(vm, err);
  if (!rak_is_ok(err)) return;
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
