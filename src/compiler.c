//
// compiler.c
//
// Copyright 2025 Fábio de Souza Villaça Medeiros
//
// This file is part of the Rak Project.
// For detailed license information, please refer to the LICENSE file
// located in the root directory of this project.
//

#include "rak/compiler.h"
#include <string.h>
#include "rak/string.h"

#define match(c, t) ((c)->lex.tok.kind == (t))

#define next(c, e) \
  do { \
    rak_lexer_next(&(c)->lex, (e)); \
    if (!rak_is_ok(e)) return; \
  } while (0)

#define consume(c, t, e) \
  do { \
    if (!match((c), (t))) { \
      expected_token_error((e), (t), (c)->lex.tok); \
      return; \
    } \
    next((c), (e)); \
  } while (0)

static inline void compile_chunk(RakCompiler *comp, RakError *err);
static inline void compile_stmt(RakCompiler *comp, RakError *err);
static inline void compile_block(RakCompiler *comp, RakError *err);
static inline void compile_let_decl(RakCompiler *comp, RakError *err);
static inline void compile_if_stmt(RakCompiler *comp, uint16_t *off, RakError *err);
static inline void compile_if_stmt_cont(RakCompiler *comp, uint16_t *off, RakError *err);
static inline void compile_echo_stmt(RakCompiler *comp, RakError *err);
static inline void compile_expr_stmt(RakCompiler *comp, RakError *err);
static inline void compile_expr(RakCompiler *comp, RakError *err);
static inline void compile_or_expr_cont(RakCompiler *comp, uint16_t *off, RakError *err);
static inline void compile_and_expr(RakCompiler *comp, RakError *err);
static inline void compile_and_expr_cont(RakCompiler *comp, uint16_t *off, RakError *err);
static inline void compile_eq_expr(RakCompiler *comp, RakError *err);
static inline void compile_cmp_expr(RakCompiler *comp, RakError *err);
static inline void compile_range_expr(RakCompiler *comp, RakError *err);
static inline void compile_add_expr(RakCompiler *comp, RakError *err);
static inline void compile_mul_expr(RakCompiler *comp, RakError *err);
static inline void compile_unary_expr(RakCompiler *comp, RakError *err);
static inline void compile_subscr_expr(RakCompiler *comp, RakError *err);
static inline void compile_subscr(RakCompiler *comp, RakError *err);
static inline void compile_prim_expr(RakCompiler *comp, RakError *err);
static inline void compile_array(RakCompiler *comp, RakError *err);
static inline void compile_record(RakCompiler *comp, RakError *err);
static inline void compile_field(RakCompiler *comp, RakError *err);
static inline void compile_if_expr(RakCompiler *comp, uint16_t *off, RakError *err);
static inline void compile_block_expr(RakCompiler *comp, RakError *err);
static inline void compile_if_expr_cont(RakCompiler *comp, uint16_t *off, RakError *err);
static inline void compile_group(RakCompiler *comp, RakError *err);
static inline void begin_scope(RakCompiler *comp);
static inline void end_scope(RakCompiler *comp, RakError *err);
static inline void define_local(RakCompiler *comp, RakToken tok, RakError *err);
static inline uint8_t resolve_local(RakCompiler *comp, RakToken tok, RakError *err);
static inline void unexpected_token_error(RakError *err, RakToken tok);
static inline void expected_token_error(RakError *err, RakTokenKind kind, RakToken tok);

static inline void compile_chunk(RakCompiler *comp, RakError *err)
{
  while (!match(comp, RAK_TOKEN_KIND_EOF))
  {
    compile_stmt(comp, err);
    if (!rak_is_ok(err)) return;
  }
}

static inline void compile_stmt(RakCompiler *comp, RakError *err)
{
  if (match(comp, RAK_TOKEN_KIND_LBRACE))
  {
    compile_block(comp, err);
    return;
  }
  if (match(comp, RAK_TOKEN_KIND_LET_KW))
  {
    compile_let_decl(comp, err);
    return;
  }
  if (match(comp, RAK_TOKEN_KIND_IF_KW))
  {
    compile_if_stmt(comp, NULL, err);
    return;
  }
  if (match(comp, RAK_TOKEN_KIND_ECHO_KW))
  {
    compile_echo_stmt(comp, err);
    return;
  }
  compile_expr_stmt(comp, err);
}

static inline void compile_block(RakCompiler *comp, RakError *err)
{
  next(comp, err);
  begin_scope(comp);
  while (!match(comp, RAK_TOKEN_KIND_RBRACE))
  {
    compile_stmt(comp, err);
    if (!rak_is_ok(err)) return;
  }
  next(comp, err);
  end_scope(comp, err);
}

static inline void compile_let_decl(RakCompiler *comp, RakError *err)
{
  next(comp, err);
  if (!match(comp, RAK_TOKEN_KIND_IDENT))
  {
    expected_token_error(err, RAK_TOKEN_KIND_IDENT, comp->lex.tok);
    return;
  }
  RakToken tok = comp->lex.tok;
  next(comp, err);
  if (match(comp, RAK_TOKEN_KIND_EQ))
  {
    next(comp, err);
    compile_expr(comp, err);
    if (!rak_is_ok(err)) return;
    consume(comp, RAK_TOKEN_KIND_SEMICOLON, err);
    define_local(comp, tok, err);
    return;
  }
  consume(comp, RAK_TOKEN_KIND_SEMICOLON, err);
  rak_chunk_append_instr(&comp->chunk, rak_push_nil_instr(), err);
  if (!rak_is_ok(err)) return;
  define_local(comp, tok, err);
}

static inline void compile_if_stmt(RakCompiler *comp, uint16_t *off, RakError *err)
{
  next(comp, err);
  compile_expr(comp, err);
  if (!rak_is_ok(err)) return;
  uint16_t jump1 = rak_chunk_append_instr(&comp->chunk, rak_nop_instr(), err);
  if (!rak_is_ok(err)) return;
  rak_chunk_append_instr(&comp->chunk, rak_pop_instr(), err);
  if (!rak_is_ok(err)) return;
  if (!match(comp, RAK_TOKEN_KIND_LBRACE))
  {
    expected_token_error(err, RAK_TOKEN_KIND_LBRACE, comp->lex.tok);
    return;
  }
  compile_block(comp, err);
  if (!rak_is_ok(err)) return;
  uint16_t jump2 = rak_chunk_append_instr(&comp->chunk, rak_nop_instr(), err);
  if (!rak_is_ok(err)) return;
  uint32_t instr = rak_jump_if_false_instr((uint16_t) comp->chunk.instrs.len);
  rak_slice_set(&comp->chunk.instrs, jump1, instr);
  uint16_t _off;
  compile_if_stmt_cont(comp, &_off, err);
  if (!rak_is_ok(err)) return;
  rak_slice_set(&comp->chunk.instrs, jump2, rak_jump_instr(_off));
  if (off) *off = _off;
}

static inline void compile_if_stmt_cont(RakCompiler *comp, uint16_t *off, RakError *err)
{
  if (!match(comp, RAK_TOKEN_KIND_ELSE_KW))
  {
    rak_chunk_append_instr(&comp->chunk, rak_pop_instr(), err);
    if (!rak_is_ok(err)) return;
    rak_chunk_append_instr(&comp->chunk, rak_push_nil_instr(), err);
    if (!rak_is_ok(err)) return;
    *off = (uint16_t) comp->chunk.instrs.len;
    return;
  }
  next(comp, err);
  if (match(comp, RAK_TOKEN_KIND_IF_KW))
  {
    rak_chunk_append_instr(&comp->chunk, rak_pop_instr(), err);
    if (!rak_is_ok(err)) return;
    compile_if_stmt(comp, off, err);
    return;
  }
  if (!match(comp, RAK_TOKEN_KIND_LBRACE))
  {
    expected_token_error(err, RAK_TOKEN_KIND_LBRACE, comp->lex.tok);
    return;
  }
  compile_block(comp, err);
  if (!rak_is_ok(err)) return;
  *off = (uint16_t) comp->chunk.instrs.len;
}

static inline void compile_echo_stmt(RakCompiler *comp, RakError *err)
{
  next(comp, err);
  compile_expr(comp, err);
  if (!rak_is_ok(err)) return;
  consume(comp, RAK_TOKEN_KIND_SEMICOLON, err);
  rak_chunk_append_instr(&comp->chunk, rak_echo_instr(), err);
}

static inline void compile_expr_stmt(RakCompiler *comp, RakError *err)
{
  compile_expr(comp, err);
  if (!rak_is_ok(err)) return;
  consume(comp, RAK_TOKEN_KIND_SEMICOLON, err);
  rak_chunk_append_instr(&comp->chunk, rak_pop_instr(), err);
}

static inline void compile_expr(RakCompiler *comp, RakError *err)
{
  compile_and_expr(comp, err);
  if (!rak_is_ok(err)) return;
  compile_or_expr_cont(comp, NULL, err);
}

static inline void compile_or_expr_cont(RakCompiler *comp, uint16_t *off, RakError *err)
{
  if (!match(comp, RAK_TOKEN_KIND_PIPEPIPE))
  {
    if (off) *off = (uint16_t) comp->chunk.instrs.len;
    return;
  }
  next(comp, err);
  uint16_t jump = rak_chunk_append_instr(&comp->chunk, rak_nop_instr(), err);
  if (!rak_is_ok(err)) return;
  rak_chunk_append_instr(&comp->chunk, rak_pop_instr(), err);
  if (!rak_is_ok(err)) return;
  compile_and_expr(comp, err);
  if (!rak_is_ok(err)) return;    
  uint16_t _off;
  compile_or_expr_cont(comp, &_off, err);
  if (!rak_is_ok(err)) return;
  rak_slice_set(&comp->chunk.instrs, jump, rak_jump_if_true_instr(_off));
  if (off) *off = _off;
}

static inline void compile_and_expr(RakCompiler *comp, RakError *err)
{
  compile_eq_expr(comp, err);
  if (!rak_is_ok(err)) return;
  compile_and_expr_cont(comp, NULL, err);
}

static inline void compile_and_expr_cont(RakCompiler *comp, uint16_t *off, RakError *err)
{
  if (!match(comp, RAK_TOKEN_KIND_AMPAMP))
  {
    if (off) *off = (uint16_t) comp->chunk.instrs.len;
    return;
  }
  next(comp, err);
  uint16_t jump = rak_chunk_append_instr(&comp->chunk, rak_nop_instr(), err);
  if (!rak_is_ok(err)) return;
  rak_chunk_append_instr(&comp->chunk, rak_pop_instr(), err);
  if (!rak_is_ok(err)) return;
  compile_eq_expr(comp, err);
  if (!rak_is_ok(err)) return;    
  uint16_t _off;
  compile_and_expr_cont(comp, &_off, err);
  if (!rak_is_ok(err)) return;
  rak_slice_set(&comp->chunk.instrs, jump, rak_jump_if_false_instr(_off));
  if (off) *off = _off;
}

static inline void compile_eq_expr(RakCompiler *comp, RakError *err)
{
  compile_cmp_expr(comp, err);
  if (!rak_is_ok(err)) return;
  for (;;)
  {
    if (match(comp, RAK_TOKEN_KIND_EQEQ))
    {
      next(comp, err);
      compile_cmp_expr(comp, err);
      if (!rak_is_ok(err)) return;
      rak_chunk_append_instr(&comp->chunk, rak_eq_instr(), err);
      if (!rak_is_ok(err)) return;
      continue;
    }
    if (match(comp, RAK_TOKEN_KIND_BANGEQ))
    {
      next(comp, err);
      compile_cmp_expr(comp, err);
      if (!rak_is_ok(err)) return;
      rak_chunk_append_instr(&comp->chunk, rak_eq_instr(), err);
      if (!rak_is_ok(err)) return;
      rak_chunk_append_instr(&comp->chunk, rak_not_instr(), err);
      if (!rak_is_ok(err)) return;
      continue;
    }
    break;
  }
}

static inline void compile_cmp_expr(RakCompiler *comp, RakError *err)
{
  compile_range_expr(comp, err);
  if (!rak_is_ok(err)) return;
  for (;;)
  {
    if (match(comp, RAK_TOKEN_KIND_GTEQ))
    {
      next(comp, err);
      compile_range_expr(comp, err);
      if (!rak_is_ok(err)) return;
      rak_chunk_append_instr(&comp->chunk, rak_lt_instr(), err);
      if (!rak_is_ok(err)) return;
      rak_chunk_append_instr(&comp->chunk, rak_not_instr(), err);
      if (!rak_is_ok(err)) return;
      continue;
    }
    if (match(comp, RAK_TOKEN_KIND_GT))
    {
      next(comp, err);
      compile_range_expr(comp, err);
      if (!rak_is_ok(err)) return;
      rak_chunk_append_instr(&comp->chunk, rak_gt_instr(), err);
      if (!rak_is_ok(err)) return;
      continue;
    }
    if (match(comp, RAK_TOKEN_KIND_LTEQ))
    {
      next(comp, err);
      compile_range_expr(comp, err);
      if (!rak_is_ok(err)) return;
      rak_chunk_append_instr(&comp->chunk, rak_gt_instr(), err);
      if (!rak_is_ok(err)) return;
      rak_chunk_append_instr(&comp->chunk, rak_not_instr(), err);
      if (!rak_is_ok(err)) return;
      continue;
    }
    if (match(comp, RAK_TOKEN_KIND_LT))
    {
      next(comp, err);
      compile_range_expr(comp, err);
      if (!rak_is_ok(err)) return;
      rak_chunk_append_instr(&comp->chunk, rak_lt_instr(), err);
      if (!rak_is_ok(err)) return;
      continue;
    }
    break;
  }
}

static inline void compile_range_expr(RakCompiler *comp, RakError *err)
{
  compile_add_expr(comp, err);
  if (!rak_is_ok(err)) return;
  if (!match(comp, RAK_TOKEN_KIND_DOTDOT)) return;
  next(comp, err);
  compile_add_expr(comp, err);
  if (!rak_is_ok(err)) return;
  rak_chunk_append_instr(&comp->chunk, rak_new_range_instr(), err);
}

static inline void compile_add_expr(RakCompiler *comp, RakError *err)
{
  compile_mul_expr(comp, err);
  if (!rak_is_ok(err)) return;
  for (;;)
  {
    if (match(comp, RAK_TOKEN_KIND_PLUS))
    {
      next(comp, err);
      compile_mul_expr(comp, err);
      if (!rak_is_ok(err)) return;
      rak_chunk_append_instr(&comp->chunk, rak_add_instr(), err);
      if (!rak_is_ok(err)) return;
      continue;
    }
    if (match(comp, RAK_TOKEN_KIND_MINUS))
    {
      next(comp, err);
      compile_mul_expr(comp, err);
      if (!rak_is_ok(err)) return;
      rak_chunk_append_instr(&comp->chunk, rak_sub_instr(), err);
      if (!rak_is_ok(err)) return;
      continue;
    }
    break;
  }
}

static inline void compile_mul_expr(RakCompiler *comp, RakError *err)
{
  compile_unary_expr(comp, err);
  if (!rak_is_ok(err)) return;
  for (;;)
  {
    if (match(comp, RAK_TOKEN_KIND_STAR))
    {
      next(comp, err);
      compile_unary_expr(comp, err);
      if (!rak_is_ok(err)) return;
      rak_chunk_append_instr(&comp->chunk, rak_mul_instr(), err);
      if (!rak_is_ok(err)) return;
      continue;
    }
    if (match(comp, RAK_TOKEN_KIND_SLASH))
    {
      next(comp, err);
      compile_unary_expr(comp, err);
      if (!rak_is_ok(err)) return;
      rak_chunk_append_instr(&comp->chunk, rak_div_instr(), err);
      if (!rak_is_ok(err)) return;
      continue;
    }
    if (match(comp, RAK_TOKEN_KIND_PERCENT))
    {
      next(comp, err);
      compile_unary_expr(comp, err);
      if (!rak_is_ok(err)) return;
      rak_chunk_append_instr(&comp->chunk, rak_mod_instr(), err);
      if (!rak_is_ok(err)) return;
      continue;
    }
    break;
  }
}

static inline void compile_unary_expr(RakCompiler *comp, RakError *err)
{
  if (match(comp, RAK_TOKEN_KIND_BANG))
  {
    next(comp, err);
    compile_unary_expr(comp, err);
    if (!rak_is_ok(err)) return;
    rak_chunk_append_instr(&comp->chunk, rak_not_instr(), err);
    return;
  }
  if (match(comp, RAK_TOKEN_KIND_MINUS))
  {
    next(comp, err);
    compile_unary_expr(comp, err);
    if (!rak_is_ok(err)) return;
    rak_chunk_append_instr(&comp->chunk, rak_neg_instr(), err);
    return;
  }
  compile_subscr_expr(comp, err);
}

static inline void compile_subscr_expr(RakCompiler *comp, RakError *err)
{
  compile_prim_expr(comp, err);
  if (!rak_is_ok(err)) return;
  compile_subscr(comp, err);
}

static inline void compile_subscr(RakCompiler *comp, RakError *err)
{
  if (match(comp, RAK_TOKEN_KIND_LBRACKET))
  {
    next(comp, err);
    compile_expr(comp, err);
    if (!rak_is_ok(err)) return;
    consume(comp, RAK_TOKEN_KIND_RBRACKET, err);
    goto end;
  }
  if (!match(comp, RAK_TOKEN_KIND_DOT)) return;
  next(comp, err);
  if (!match(comp, RAK_TOKEN_KIND_IDENT))
  {
    expected_token_error(err, RAK_TOKEN_KIND_IDENT, comp->lex.tok);
    return;
  }
  RakToken tok = comp->lex.tok;
  next(comp, err);
  RakString *str = rak_string_new_from_cstr(tok.len, tok.chars, err);
  if (!rak_is_ok(err)) return;
  RakValue val = rak_string_value(str);
  uint8_t idx = rak_chunk_append_const(&comp->chunk, val, err);
  if (!rak_is_ok(err))
  {
    rak_string_free(str);
    return;
  }
  rak_chunk_append_instr(&comp->chunk, rak_load_const_instr(idx), err);
  if (!rak_is_ok(err))
  {
    rak_string_free(str);
    return;
  }
end:
  rak_chunk_append_instr(&comp->chunk, rak_load_element_instr(), err);
  if (!rak_is_ok(err)) return;
  compile_subscr(comp, err);
}

static inline void compile_prim_expr(RakCompiler *comp, RakError *err)
{
  if (match(comp, RAK_TOKEN_KIND_NIL_KW))
  {
    next(comp, err);
    rak_chunk_append_instr(&comp->chunk, rak_push_nil_instr(), err);
    return;
  }
  if (match(comp, RAK_TOKEN_KIND_FALSE_KW))
  {
    next(comp, err);
    rak_chunk_append_instr(&comp->chunk, rak_push_false_instr(), err);
    return;
  }
  if (match(comp, RAK_TOKEN_KIND_TRUE_KW))
  {
    next(comp, err);
    rak_chunk_append_instr(&comp->chunk, rak_push_true_instr(), err);
    return;
  }
  if (match(comp, RAK_TOKEN_KIND_NUMBER))
  {
    RakToken tok = comp->lex.tok;
    next(comp, err);
    RakValue val = rak_number_value_from_cstr(tok.len, tok.chars, err);
    if (!rak_is_ok(err)) return;
    uint8_t idx = rak_chunk_append_const(&comp->chunk, val, err);
    if (!rak_is_ok(err)) return;
    rak_chunk_append_instr(&comp->chunk, rak_load_const_instr(idx), err);
    return;
  }
  if (match(comp, RAK_TOKEN_KIND_STRING))
  {
    RakToken tok = comp->lex.tok;
    next(comp, err);
    RakString *str = rak_string_new_from_cstr(tok.len, tok.chars, err);
    if (!rak_is_ok(err)) return;
    RakValue val = rak_string_value(str);
    uint8_t idx = rak_chunk_append_const(&comp->chunk, val, err);
    if (!rak_is_ok(err))
    {
      rak_string_free(str);
      return;
    }
    rak_chunk_append_instr(&comp->chunk, rak_load_const_instr(idx), err);
    if (rak_is_ok(err)) return;
    rak_string_free(str);
    return;
  }
  if (match(comp, RAK_TOKEN_KIND_IDENT))
  {
    RakToken tok = comp->lex.tok;
    next(comp, err);
    uint8_t idx = resolve_local(comp, tok, err);
    if (!rak_is_ok(err)) return;
    rak_chunk_append_instr(&comp->chunk, rak_load_local_instr(idx), err);
    return;
  }
  if (match(comp, RAK_TOKEN_KIND_LBRACKET))
  {
    compile_array(comp, err);
    return;
  }
  if (match(comp, RAK_TOKEN_KIND_LBRACE))
  {
    compile_record(comp, err);
    return;
  }
  if (match(comp, RAK_TOKEN_KIND_IF_KW))
  {
    compile_if_expr(comp, NULL, err);
    return;
  }
  if (match(comp, RAK_TOKEN_KIND_LPAREN))
  {
    compile_group(comp, err);
    return;
  }
  unexpected_token_error(err, comp->lex.tok);
}

static inline void compile_array(RakCompiler *comp, RakError *err)
{
  next(comp, err);
  if (match(comp, RAK_TOKEN_KIND_RBRACKET))
  {
    next(comp, err);
    rak_chunk_append_instr(&comp->chunk, rak_new_array_instr(0), err);
    return;
  }
  compile_expr(comp, err);
  if (!rak_is_ok(err)) return;
  int len = 1;
  while (match(comp, RAK_TOKEN_KIND_COMMA))
  {
    next(comp, err);
    compile_expr(comp, err);
    if (!rak_is_ok(err)) return;
    ++len;
  }
  consume(comp, RAK_TOKEN_KIND_RBRACKET, err);
  rak_chunk_append_instr(&comp->chunk, rak_new_array_instr(len), err);
}

static inline void compile_record(RakCompiler *comp, RakError *err)
{
  next(comp, err);
  if (match(comp, RAK_TOKEN_KIND_RBRACE))
  {
    next(comp, err);
    rak_chunk_append_instr(&comp->chunk, rak_new_record_instr(0), err);
    return;
  }
  compile_field(comp, err);
  if (!rak_is_ok(err)) return;
  int len = 1;
  while (match(comp, RAK_TOKEN_KIND_COMMA))
  {
    next(comp, err);
    compile_field(comp, err);
    if (!rak_is_ok(err)) return;
    ++len;
  }
  consume(comp, RAK_TOKEN_KIND_RBRACE, err);
  rak_chunk_append_instr(&comp->chunk, rak_new_record_instr(len), err);
}

static inline void compile_field(RakCompiler *comp, RakError *err)
{
  if (!match(comp, RAK_TOKEN_KIND_IDENT))
  {
    expected_token_error(err, RAK_TOKEN_KIND_IDENT, comp->lex.tok);
    return;
  }
  RakToken tok = comp->lex.tok;
  next(comp, err);
  consume(comp, RAK_TOKEN_KIND_COLON, err);
  RakString *name = rak_string_new_from_cstr(tok.len, tok.chars, err);
  if (!rak_is_ok(err)) return;
  RakValue val = rak_string_value(name);
  uint8_t idx = rak_chunk_append_const(&comp->chunk, val, err);
  if (!rak_is_ok(err))
  {
    rak_string_free(name);
    return;
  }
  rak_chunk_append_instr(&comp->chunk, rak_load_const_instr(idx), err);
  if (!rak_is_ok(err))
  {
    rak_string_free(name);
    return;
  }
  compile_expr(comp, err);
  if (rak_is_ok(err)) return;
  rak_string_free(name);
}

static inline void compile_if_expr(RakCompiler *comp, uint16_t *off, RakError *err)
{
  next(comp, err);
  compile_expr(comp, err);
  if (!rak_is_ok(err)) return;
  uint16_t jump1 = rak_chunk_append_instr(&comp->chunk, rak_nop_instr(), err);
  if (!rak_is_ok(err)) return;
  rak_chunk_append_instr(&comp->chunk, rak_pop_instr(), err);
  if (!rak_is_ok(err)) return;
  compile_block_expr(comp, err);
  if (!rak_is_ok(err)) return;
  uint16_t jump2 = rak_chunk_append_instr(&comp->chunk, rak_nop_instr(), err);
  if (!rak_is_ok(err)) return;
  uint32_t instr = rak_jump_if_false_instr((uint16_t) comp->chunk.instrs.len);
  rak_slice_set(&comp->chunk.instrs, jump1, instr);
  uint16_t _off;
  compile_if_expr_cont(comp, &_off, err);
  if (!rak_is_ok(err)) return;
  rak_slice_set(&comp->chunk.instrs, jump2, rak_jump_instr(_off));
  if (off) *off = _off;
}

static inline void compile_block_expr(RakCompiler *comp, RakError *err)
{
  consume(comp, RAK_TOKEN_KIND_LBRACE, err);
  compile_expr(comp, err);
  if (!rak_is_ok(err)) return;
  consume(comp, RAK_TOKEN_KIND_RBRACE, err);
}

static inline void compile_if_expr_cont(RakCompiler *comp, uint16_t *off, RakError *err)
{
  rak_chunk_append_instr(&comp->chunk, rak_pop_instr(), err);
  if (!rak_is_ok(err)) return;
  if (!match(comp, RAK_TOKEN_KIND_ELSE_KW))
  {
    rak_chunk_append_instr(&comp->chunk, rak_push_nil_instr(), err);
    if (!rak_is_ok(err)) return;
    *off = (uint16_t) comp->chunk.instrs.len;
    return;
  }
  next(comp, err);
  if (match(comp, RAK_TOKEN_KIND_IF_KW))
  {
    compile_if_expr(comp, off, err);
    return;
  }
  compile_block_expr(comp, err);
  if (!rak_is_ok(err)) return;
  *off = (uint16_t) comp->chunk.instrs.len;
}

static inline void compile_group(RakCompiler *comp, RakError *err)
{
  next(comp, err);
  compile_expr(comp, err);
  if (!rak_is_ok(err)) return;
  consume(comp, RAK_TOKEN_KIND_RPAREN, err);
}

static inline void begin_scope(RakCompiler *comp)
{
  ++comp->scopeDepth;
}

static inline void end_scope(RakCompiler *comp, RakError *err)
{
  int len = comp->symbols.len;
  int n = 0;
  for (int i = len - 1; i >= 0; --i)
  {
    RakSymbol sym = rak_slice_get(&comp->symbols, i);
    if (sym.depth != comp->scopeDepth) break;
    rak_chunk_append_instr(&comp->chunk, rak_pop_instr(), err);
    if (!rak_is_ok(err)) return;
    ++n;
  }
  comp->symbols.len -= n;
  --comp->scopeDepth;
}

static inline void define_local(RakCompiler *comp, RakToken tok, RakError *err)
{
  int idx = -1;
  int len = comp->symbols.len;
  for (int i = len - 1; i >= 0; --i)
  {
    RakSymbol sym = rak_slice_get(&comp->symbols, i);
    if (sym.depth != comp->scopeDepth) break;
    if (sym.len != tok.len || memcmp(sym.chars, tok.chars, tok.len))
      continue;
    idx = sym.idx;
    break;
  }
  if (idx != -1)
  {
    rak_error_set(err, "duplicate local variable '%.*s'", tok.len, tok.chars);
    return;
  }
  idx = comp->symbols.len;
  if (idx > UINT8_MAX)
  {
    rak_error_set(err, "too many local variables");
    return;
  }
  RakSymbol sym = {
    .len = tok.len,
    .chars = tok.chars,
    .idx  = (uint8_t) idx,
    .depth = comp->scopeDepth
  };
  rak_slice_append(&comp->symbols, sym, err);
  if (!rak_is_ok(err)) return;
}

static inline uint8_t resolve_local(RakCompiler *comp, RakToken tok, RakError *err)
{
  int idx = -1;
  int len = comp->symbols.len;
  for (int i = len - 1; i >= 0; --i)
  {
    RakSymbol sym = rak_slice_get(&comp->symbols, i);
    if (sym.len != tok.len || memcmp(sym.chars, tok.chars, tok.len))
      continue;
    idx = sym.idx;
    break;
  }
  if (idx == -1)
  {
    rak_error_set(err, "undefined local variable '%.*s'", tok.len, tok.chars);
    return 0;
  }
  return (uint8_t) idx;
}

static inline void unexpected_token_error(RakError *err, RakToken tok)
{
  if (tok.kind == RAK_TOKEN_KIND_EOF)
  {
    rak_error_set(err, "unexpected end of file at %d:%d", tok.ln, tok.col);
    return;
  }
  rak_error_set(err, "unexpected token '%.*s' at %d:%d", tok.len, tok.chars,
    tok.ln, tok.col);
}

static inline void expected_token_error(RakError *err, RakTokenKind kind, RakToken tok)
{
  if (tok.kind == RAK_TOKEN_KIND_EOF)
  {
    rak_error_set(err, "expected %s, but got end of file at %d:%d",
      rak_token_kind_to_cstr(kind), tok.ln, tok.col);
    return;
  }
  rak_error_set(err, "expected %s, but got '%.*s' at %d:%d",
    rak_token_kind_to_cstr(kind), tok.len, tok.chars, tok.ln, tok.col);
}

void rak_compiler_init(RakCompiler *comp, RakError *err)
{
  rak_chunk_init(&comp->chunk, err);
  if (!rak_is_ok(err)) return;
  rak_slice_init(&comp->symbols, err);
  if (!rak_is_ok(err))
  {
    rak_chunk_deinit(&comp->chunk);
    return;
  }
  comp->scopeDepth = 0;
}

void rak_compiler_deinit(RakCompiler *comp)
{
  rak_chunk_deinit(&comp->chunk);
  rak_slice_deinit(&comp->symbols);
}

void rak_compiler_compile(RakCompiler *comp, char *source, RakError *err)
{
  rak_lexer_init(&comp->lex, source, err);
  if (!rak_is_ok(err)) return;
  rak_chunk_clear(&comp->chunk);
  rak_slice_clear(&comp->symbols);
  compile_chunk(comp, err);
  if (!rak_is_ok(err)) return;
  rak_chunk_append_instr(&comp->chunk, rak_halt_instr(), err);
}
