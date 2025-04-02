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

#define match(c, t) ((c)->lex.tok.kind == (t))

#define next(c, e) \
  do { \
    rak_lexer_next(&(c)->lex, (e)); \
    if (!rak_is_ok(e)) return; \
  } while (0)

#define consume(c, t, e) \
  do { \
    if (!match((c), (t))) { \
      RakToken tok = (c)->lex.tok; \
      expected_token_error((e), (t), tok); \
      return; \
    } \
    next((c), (e)); \
  } while (0)

static inline void compile_chunk(RakCompiler *comp, RakError *err);
static inline void compile_stmt(RakCompiler *comp, RakError *err);
static inline void compile_expr(RakCompiler *comp, RakError *err);
static inline void compile_mul_expr(RakCompiler *comp, RakError *err);
static inline void compile_unary_expr(RakCompiler *comp, RakError *err);
static inline void compile_prim_expr(RakCompiler *comp, RakError *err);
static inline void unexpected_token_error(RakError *err, RakToken tok);
static inline void expected_token_error(RakError *err, RakTokenKind kind, RakToken tok);
static inline void emit_const_instr(RakCompiler *comp, RakToken tok, RakError *err);

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

static inline void emit_const_instr(RakCompiler *comp, RakToken tok, RakError *err)
{
  int idx = rak_chunk_append_const(&comp->chunk, tok.val, err);
  if (idx > UINT8_MAX)
  {
    rak_error_set(err, "too many constants");
    return;
  }
  if (!rak_is_ok(err)) return;
  rak_chunk_append_instr(&comp->chunk, rak_load_const_instr((uint8_t) idx), err);
}

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
  compile_expr(comp, err);
  if (!rak_is_ok(err)) return;
  consume(comp, RAK_TOKEN_KIND_SEMICOLON, err);
  rak_chunk_append_instr(&comp->chunk, rak_pop_instr(), err);
}

static inline void compile_expr(RakCompiler *comp, RakError *err)
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
  if (match(comp, RAK_TOKEN_KIND_MINUS))
  {
    next(comp, err);
    compile_unary_expr(comp, err);
    if (!rak_is_ok(err)) return;
    rak_chunk_append_instr(&comp->chunk, rak_neg_instr(), err);
    return;
  }
  compile_prim_expr(comp, err);
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
    emit_const_instr(comp, tok, err);
    if (!rak_is_ok(err)) return;
    return;
  }
  if (match(comp, RAK_TOKEN_KIND_LPAREN))
  {
    next(comp, err);
    compile_expr(comp, err);
    if (!rak_is_ok(err)) return;
    consume(comp, RAK_TOKEN_KIND_RPAREN, err);
    return;
  }
  unexpected_token_error(err, comp->lex.tok);
}

void rak_compiler_init(RakCompiler *comp, RakError *err)
{
  rak_chunk_init(&comp->chunk, err);
}

void rak_compiler_deinit(RakCompiler *comp)
{
  rak_chunk_deinit(&comp->chunk);
}

void rak_compiler_compile_chunk(RakCompiler *comp, char *source, RakError *err)
{
  rak_lexer_init(&comp->lex, source, err);
  if (!rak_is_ok(err)) return;
  rak_chunk_clear(&comp->chunk);
  compile_chunk(comp, err);
  rak_chunk_append_instr(&comp->chunk, rak_halt_instr(), err);
}

void rak_compiler_compile_expr(RakCompiler *comp, char *source, RakError *err)
{
  rak_lexer_init(&comp->lex, source, err);
  if (!rak_is_ok(err)) return;
  rak_chunk_clear(&comp->chunk);
  compile_expr(comp, err);
  rak_chunk_append_instr(&comp->chunk, rak_halt_instr(), err);
}
