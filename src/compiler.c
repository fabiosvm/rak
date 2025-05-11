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
#include "rak/builtin.h"
#include "rak/lexer.h"

#define match(c, t) ((c)->lex->tok.kind == (t))

#define next(c, e) \
  do { \
    rak_lexer_next((c)->lex, (e)); \
    if (!rak_is_ok(e)) return; \
  } while (0)

#define consume(c, t, e) \
  do { \
    if (!match((c), (t))) { \
      expected_token_error((e), (t), (c)->lex->tok); \
      return; \
    } \
    next((c), (e)); \
  } while (0)

#define emit_instr(c, i, e) rak_chunk_append_instr((c), (i), (e))

#define patch_instr(c, o, i) \
  do { \
    rak_slice_set(&(c)->instrs, (o), (i)); \
  } while (0)

#define is_blank_ident(t) ((t).len == 1 && (t).chars[0] == '_')

typedef struct
{
  RakToken tok;
  uint8_t  idx;
  int      depth;
} Symbol;

typedef struct Loop
{
  struct Loop                         *parent;
  uint16_t                             off;
  RakStaticSlice(uint16_t, UINT8_MAX)  jumps;
} Loop;

typedef RakStaticSlice(Symbol, UINT8_MAX) SymbolSlice;

typedef struct Compiler
{
  struct Compiler *parent;
  RakLexer        *lex;
  SymbolSlice      symbols;
  int              scopeDepth;
  Loop            *loop;
  RakFunction     *fn;
} Compiler;

static inline void compiler_init(Compiler *comp, Compiler *parent, RakLexer *lex,
  RakString *fnName, int arity, RakError *err);
static inline void compiler_deinit(Compiler *comp);
static inline void compile_chunk(Compiler *comp, RakChunk *chunk, RakError *err);
static inline void compile_stmt(Compiler *comp, RakChunk *chunk, RakError *err);
static inline void compile_block(Compiler *comp, RakChunk *chunk, RakError *err);
static inline void compile_let_decl(Compiler *comp, RakChunk *chunk, RakError *err);
static inline void compile_destruct_elements(Compiler *comp, RakChunk *chunk, RakError *err);
static inline void compile_ident_list(Compiler *comp, SymbolSlice *symbols, RakError *err);
static inline void compile_destruct_fields(Compiler *comp, RakChunk *chunk, RakError *err);
static inline void compile_fn_decl(Compiler *comp, RakChunk *chunk, RakError *err);
static inline void compile_params(Compiler *comp, RakError *err);
static inline void compile_if_stmt(Compiler *comp, RakChunk *chunk, uint16_t *off, RakError *err);
static inline void compile_if_stmt_cont(Compiler *comp, RakChunk *chunk, uint16_t *off, RakError *err);
static inline void compile_loop_stmt(Compiler *comp, RakChunk *chunk, RakError *err);
static inline void compile_while_stmt(Compiler *comp, RakChunk *chunk, RakError *err);
static inline void compile_do_while_stmt(Compiler *comp, RakChunk *chunk, RakError *err);
static inline void compile_break_stmt(Compiler *comp, RakChunk *chunk, RakError *err);
static inline void compile_continue_stmt(Compiler *comp, RakChunk *chunk, RakError *err);
static inline void compile_return_stmt(Compiler *comp, RakChunk *chunk, RakError *err);
static inline void compile_expr_stmt(Compiler *comp, RakChunk *chunk, RakError *err);
static inline void compile_expr(Compiler *comp, RakChunk *chunk, RakError *err);
static inline void compile_assign_expr(Compiler *comp, RakChunk *chunk, RakError *err);
static inline void compile_assign_op(Compiler *comp, uint32_t *instr, RakError *err);
static inline void compile_assign_expr_cont(Compiler *comp, RakChunk *chunk, RakError *err);
static inline void compile_or_expr(Compiler *comp, RakChunk *chunk, RakError *err);
static inline void compile_or_expr_cont(Compiler *comp, RakChunk *chunk, uint16_t *off, RakError *err);
static inline void compile_and_expr(Compiler *comp, RakChunk *chunk, RakError *err);
static inline void compile_and_expr_cont(Compiler *comp, RakChunk *chunk, uint16_t *off, RakError *err);
static inline void compile_eq_expr(Compiler *comp, RakChunk *chunk, RakError *err);
static inline void compile_cmp_expr(Compiler *comp, RakChunk *chunk, RakError *err);
static inline void compile_range_expr(Compiler *comp, RakChunk *chunk, RakError *err);
static inline void compile_add_expr(Compiler *comp, RakChunk *chunk, RakError *err);
static inline void compile_mul_expr(Compiler *comp, RakChunk *chunk, RakError *err);
static inline void compile_unary_expr(Compiler *comp, RakChunk *chunk, RakError *err);
static inline void compile_call_expr(Compiler *comp, RakChunk *chunk, RakError *err);
static inline void compile_call(Compiler *comp, RakChunk *chunk, bool *ok, RakError *err);
static inline void compile_expr_list(Compiler *comp, RakChunk *chunk, int *n, RakError *err);
static inline void compile_subscr(Compiler *comp, RakChunk *chunk, bool *ok, RakError *err);
static inline void compile_prim_expr(Compiler *comp, RakChunk *chunk, RakError *err);
static inline void compile_array(Compiler *comp, RakChunk *chunk, RakError *err);
static inline void compile_record(Compiler *comp, RakChunk *chunk, RakError *err);
static inline void compile_fn(Compiler *comp, RakChunk *chunk, RakError *err);
static inline void compile_field(Compiler *comp, RakChunk *chunk, RakError *err);
static inline void compile_if_expr(Compiler *comp, RakChunk *chunk, uint16_t *off, RakError *err);
static inline void compile_block_expr(Compiler *comp, RakChunk *chunk, RakError *err);
static inline void compile_if_expr_cont(Compiler *comp, RakChunk *chunk, uint16_t *off, RakError *err);
static inline void compile_group(Compiler *comp, RakChunk *chunk, RakError *err);
static inline void begin_scope(Compiler *comp);
static inline void end_scope(Compiler *comp, RakChunk *chunk, RakError *err);
static inline void begin_loop(Compiler *comp, RakChunk *chunk, Loop *loop);
static inline void end_loop(Compiler *comp, RakChunk *chunk);
static inline void define_local(Compiler *comp, RakToken tok, RakError *err);
static inline void append_local(Compiler *comp, RakToken tok);
static inline int resolve_local(Compiler *comp, RakToken tok);
static inline bool ident_equals(RakToken tok1, RakToken tok2);
static inline void emit_return(RakChunk *chunk, RakError *err);
static inline void unexpected_token_error(RakError *err, RakToken tok);
static inline void expected_token_error(RakError *err, RakTokenKind kind, RakToken tok);

static inline void compiler_init(Compiler *comp, Compiler *parent, RakLexer *lex,
  RakString *fnName, int arity, RakError *err)
{
  comp->parent = parent;
  comp->lex = lex;
  rak_static_slice_init(&comp->symbols);
  comp->scopeDepth = -1;
  comp->loop = NULL;
  RakFunction *fn = rak_function_new(fnName, arity, err);
  if (!rak_is_ok(err)) return;
  comp->fn = fn;
  rak_object_retain(&fn->callable.obj);
}

static inline void compiler_deinit(Compiler *comp)
{
  rak_function_release(comp->fn);
}

static inline void compile_chunk(Compiler *comp, RakChunk *chunk, RakError *err)
{
  begin_scope(comp);
  while (!match(comp, RAK_TOKEN_KIND_EOF))
  {
    compile_stmt(comp, chunk, err);
    if (!rak_is_ok(err)) return;
  }
  emit_instr(chunk, rak_push_nil_instr(), err);
  if (!rak_is_ok(err)) return;
  emit_instr(chunk, rak_return_instr(), err);
  if (!rak_is_ok(err)) return;
  end_scope(comp, chunk, err);
}

static inline void compile_stmt(Compiler *comp, RakChunk *chunk, RakError *err)
{
  if (match(comp, RAK_TOKEN_KIND_LBRACE))
  {
    compile_block(comp, chunk, err);
    return;
  }
  if (match(comp, RAK_TOKEN_KIND_LET_KW))
  {
    compile_let_decl(comp, chunk, err);
    return;
  }
  if (match(comp, RAK_TOKEN_KIND_FN_KW))
  {
    compile_fn_decl(comp, chunk, err);
    return;
  }
  if (match(comp, RAK_TOKEN_KIND_IF_KW))
  {
    compile_if_stmt(comp, chunk, NULL, err);
    return;
  }
  if (match(comp, RAK_TOKEN_KIND_LOOP_KW))
  {
    compile_loop_stmt(comp, chunk, err);
    return;
  }
  if (match(comp, RAK_TOKEN_KIND_WHILE_KW))
  {
    compile_while_stmt(comp, chunk, err);
    return;
  }
  if (match(comp, RAK_TOKEN_KIND_DO_KW))
  {
    compile_do_while_stmt(comp, chunk, err);
    return;
  }
  if (match(comp, RAK_TOKEN_KIND_BREAK_KW))
  {
    compile_break_stmt(comp, chunk, err);
    return;
  }
  if (match(comp, RAK_TOKEN_KIND_CONTINUE_KW))
  {
    compile_continue_stmt(comp, chunk, err);
    return;
  }
  if (match(comp, RAK_TOKEN_KIND_RETURN_KW))
  {
    compile_return_stmt(comp, chunk, err);
    return;
  }
  compile_expr_stmt(comp, chunk, err);
}

static inline void compile_block(Compiler *comp, RakChunk *chunk, RakError *err)
{
  next(comp, err);
  begin_scope(comp);
  while (!match(comp, RAK_TOKEN_KIND_RBRACE))
  {
    compile_stmt(comp, chunk, err);
    if (!rak_is_ok(err)) return;
  }
  next(comp, err);
  end_scope(comp, chunk, err);
}

static inline void compile_let_decl(Compiler *comp, RakChunk *chunk, RakError *err)
{
  next(comp, err);
  if (match(comp, RAK_TOKEN_KIND_LBRACKET))
  {
    compile_destruct_elements(comp, chunk, err);
    if (!rak_is_ok(err)) return;
    goto end;
  }
  if (match(comp, RAK_TOKEN_KIND_LBRACE))
  {
    compile_destruct_fields(comp, chunk, err);
    if (!rak_is_ok(err)) return;
    goto end;
  }
  if (!match(comp, RAK_TOKEN_KIND_IDENT))
  {
    expected_token_error(err, RAK_TOKEN_KIND_IDENT, comp->lex->tok);
    return;
  }
  RakToken tok = comp->lex->tok;
  next(comp, err);
  bool isBlank = is_blank_ident(tok);
  if (match(comp, RAK_TOKEN_KIND_EQ))
  {
    next(comp, err);
    compile_expr(comp, chunk, err);
    if (!rak_is_ok(err)) return;
    if (isBlank)
    {
      emit_instr(chunk, rak_pop_instr(), err);
      if (!rak_is_ok(err)) return;
      goto end;
    }
    define_local(comp, tok, err);
    if (!rak_is_ok(err)) return;
    goto end;
  }
  if (isBlank) goto end;
  emit_instr(chunk, rak_push_nil_instr(), err);
  if (!rak_is_ok(err)) return;
  define_local(comp, tok, err);
  if (!rak_is_ok(err)) return;
end:
  consume(comp, RAK_TOKEN_KIND_SEMICOLON, err);
}

static inline void compile_destruct_elements(Compiler *comp, RakChunk *chunk, RakError *err)
{
  next(comp, err);
  SymbolSlice symbols;
  rak_static_slice_init(&symbols);
  compile_ident_list(comp, &symbols, err);
  if (!rak_is_ok(err)) return;
  consume(comp, RAK_TOKEN_KIND_RBRACKET, err);
  consume(comp, RAK_TOKEN_KIND_EQ, err);
  compile_expr(comp, chunk, err);
  if (!rak_is_ok(err)) return;
  for (int i = 0; i < symbols.len; ++i)
  {
    Symbol sym = rak_slice_get(&symbols, i);
    define_local(comp, sym.tok, err);
    if (!rak_is_ok(err)) return;
    emit_instr(chunk, rak_push_int_instr(sym.idx), err);
    if (!rak_is_ok(err)) return;
  }
  emit_instr(chunk, rak_unpack_elements_instr((uint8_t) symbols.len), err);
}

static inline void compile_ident_list(Compiler *comp, SymbolSlice *symbols, RakError *err)
{
  if (!match(comp, RAK_TOKEN_KIND_IDENT))
  {
    expected_token_error(err, RAK_TOKEN_KIND_IDENT, comp->lex->tok);
    return;
  }
  RakToken tok = comp->lex->tok;
  uint8_t idx = 0;
  for (;;)
  {
    next(comp, err);
    if (!is_blank_ident(tok))
    {
      if (rak_slice_is_full(symbols))
      {
        rak_error_set(err, "too many destructuring variables at %d:%d",
          tok.ln, tok.col);
        return;
      }
      Symbol sym = {
        .tok = tok,
        .idx = idx
      };
      rak_slice_append(symbols, sym);
    }
    ++idx;
    if (!match(comp, RAK_TOKEN_KIND_COMMA)) break;
    next(comp, err);
    if (!match(comp, RAK_TOKEN_KIND_IDENT))
    {
      expected_token_error(err, RAK_TOKEN_KIND_IDENT, comp->lex->tok);
      return;
    }
    tok = comp->lex->tok;
  }
}

static inline void compile_destruct_fields(Compiler *comp, RakChunk *chunk, RakError *err)
{
  next(comp, err);
  SymbolSlice symbols;
  rak_static_slice_init(&symbols);
  compile_ident_list(comp, &symbols, err);
  if (!rak_is_ok(err)) return;
  consume(comp, RAK_TOKEN_KIND_RBRACE, err);
  consume(comp, RAK_TOKEN_KIND_EQ, err);
  compile_expr(comp, chunk, err);
  if (!rak_is_ok(err)) return;
  for (int i = 0; i < symbols.len; ++i)
  {
    RakToken tok = rak_slice_get(&symbols, i).tok;
    define_local(comp, tok, err);
    if (!rak_is_ok(err)) return;
    RakString *str = rak_string_new_from_cstr(tok.len, tok.chars, err);
    if (!rak_is_ok(err)) return;
    RakValue val = rak_string_value(str);
    uint8_t idx = rak_chunk_append_const(chunk, val, err);
    if (!rak_is_ok(err))
    {
      rak_string_free(str);
      return;
    }
    emit_instr(chunk, rak_load_const_instr(idx), err);
    if (!rak_is_ok(err)) return;
  }
  emit_instr(chunk, rak_unpack_fields_instr((uint8_t) symbols.len), err);
}

static inline void compile_fn_decl(Compiler *comp, RakChunk *chunk, RakError *err)
{
  next(comp, err);
  if (!match(comp, RAK_TOKEN_KIND_IDENT))
  {
    expected_token_error(err, RAK_TOKEN_KIND_IDENT, comp->lex->tok);
    return;
  }
  RakToken tok = comp->lex->tok;
  next(comp, err);
  RakString *name = rak_string_new_from_cstr(tok.len, tok.chars, err);
  if (!rak_is_ok(err)) return;
  Compiler _comp;
  compiler_init(&_comp, comp, comp->lex, name, 0, err);
  if (!rak_is_ok(err))
  {
    rak_string_free(name);
    return;
  }
  begin_scope(&_comp);
  define_local(&_comp, tok, err);
  if (!rak_is_ok(err)) goto fail;
  compile_params(&_comp, err);
  if (!rak_is_ok(err)) goto fail;
  if (!match(&_comp, RAK_TOKEN_KIND_LBRACE))
  {
    expected_token_error(err, RAK_TOKEN_KIND_LBRACE, _comp.lex->tok);
    goto fail;
  }
  RakChunk *_chunk = &_comp.fn->chunk;
  compile_block(&_comp, _chunk, err);
  if (!rak_is_ok(err)) goto fail;
  emit_instr(_chunk, rak_push_nil_instr(), err);
  if (!rak_is_ok(err)) goto fail;
  emit_instr(_chunk, rak_return_instr(), err);
  if (!rak_is_ok(err)) goto fail;
  end_scope(&_comp, _chunk, err);
  if (!rak_is_ok(err)) return;
  uint8_t idx = rak_function_append_nested(comp->fn, _comp.fn, err);
  if (!rak_is_ok(err)) goto fail;
  emit_instr(chunk, rak_new_closure_instr(idx), err);
  if (!rak_is_ok(err)) goto fail;
  define_local(comp, tok, err);
  return;
fail:
  compiler_deinit(&_comp);
}

static inline void compile_params(Compiler *comp, RakError *err)
{
  consume(comp, RAK_TOKEN_KIND_LPAREN, err);
  RakCallable *callable = &comp->fn->callable;
  if (match(comp, RAK_TOKEN_KIND_RPAREN))
  {
    next(comp, err);
    callable->arity = 0;
    return;
  }
  if (!match(comp, RAK_TOKEN_KIND_IDENT))
  {
    expected_token_error(err, RAK_TOKEN_KIND_IDENT, comp->lex->tok);
    return;
  }
  RakToken tok = comp->lex->tok;
  next(comp, err);
  define_local(comp, tok, err);
  if (!rak_is_ok(err)) return;
  int arity = 1;
  while (match(comp, RAK_TOKEN_KIND_COMMA))
  {
    next(comp, err);
    if (!match(comp, RAK_TOKEN_KIND_IDENT))
    {
      expected_token_error(err, RAK_TOKEN_KIND_IDENT, comp->lex->tok);
      return;
    }
    tok = comp->lex->tok;
    next(comp, err);
    if (arity > UINT8_MAX)
    {
      rak_error_set(err, "too many function parameters at %d:%d",
        tok.ln, tok.col);
      return;
    }
    define_local(comp, tok, err);
    if (!rak_is_ok(err)) return;
    ++arity;
  }
  consume(comp, RAK_TOKEN_KIND_RPAREN, err);
  callable->arity = arity;
}

static inline void compile_if_stmt(Compiler *comp, RakChunk *chunk, uint16_t *off, RakError *err)
{
  next(comp, err);
  begin_scope(comp);
  if (match(comp, RAK_TOKEN_KIND_LET_KW))
  {
    compile_let_decl(comp, chunk, err);
    if (!rak_is_ok(err)) return;
  }
  compile_expr(comp, chunk, err);
  if (!rak_is_ok(err)) return;
  uint16_t jump1 = emit_instr(chunk, rak_nop_instr(), err);
  if (!rak_is_ok(err)) return;
  emit_instr(chunk, rak_pop_instr(), err);
  if (!rak_is_ok(err)) return;
  if (!match(comp, RAK_TOKEN_KIND_LBRACE))
  {
    expected_token_error(err, RAK_TOKEN_KIND_LBRACE, comp->lex->tok);
    return;
  }
  compile_block(comp, chunk, err);
  if (!rak_is_ok(err)) return;
  uint16_t jump2 = emit_instr(chunk, rak_nop_instr(), err);
  if (!rak_is_ok(err)) return;
  uint32_t instr = rak_jump_if_false_instr((uint16_t) chunk->instrs.len);
  patch_instr(chunk, jump1, instr);
  if (!rak_is_ok(err)) return;
  emit_instr(chunk, rak_pop_instr(), err);
  uint16_t _off;
  compile_if_stmt_cont(comp, chunk, &_off, err);
  if (!rak_is_ok(err)) return;
  patch_instr(chunk, jump2, rak_jump_instr(_off));
  end_scope(comp, chunk, err);
  if (!rak_is_ok(err)) return;
  if (off) *off = _off;
}

static inline void compile_if_stmt_cont(Compiler *comp, RakChunk *chunk, uint16_t *off, RakError *err)
{
  if (!match(comp, RAK_TOKEN_KIND_ELSE_KW))
  {
    *off = (uint16_t) chunk->instrs.len;
    return;
  }
  next(comp, err);
  if (match(comp, RAK_TOKEN_KIND_IF_KW))
  {
    compile_if_stmt(comp, chunk, off, err);
    *off = (uint16_t) chunk->instrs.len;
    return;
  }
  if (!match(comp, RAK_TOKEN_KIND_LBRACE))
  {
    expected_token_error(err, RAK_TOKEN_KIND_LBRACE, comp->lex->tok);
    return;
  }
  compile_block(comp, chunk, err);
  if (!rak_is_ok(err)) return;
  *off = (uint16_t) chunk->instrs.len;
}

static inline void compile_loop_stmt(Compiler *comp, RakChunk *chunk, RakError *err)
{
  next(comp, err);
  Loop loop;
  begin_loop(comp, chunk, &loop);
  uint16_t off = (uint16_t) chunk->instrs.len;
  if (!match(comp, RAK_TOKEN_KIND_LBRACE))
  {
    expected_token_error(err, RAK_TOKEN_KIND_LBRACE, comp->lex->tok);
    return;
  }
  compile_block(comp, chunk, err);
  if (!rak_is_ok(err)) return;
  emit_instr(chunk, rak_jump_instr(off), err);
  if (!rak_is_ok(err)) return;
  end_loop(comp, chunk);
}

static inline void compile_while_stmt(Compiler *comp, RakChunk *chunk, RakError *err)
{
  next(comp, err);
  begin_scope(comp);
  if (match(comp, RAK_TOKEN_KIND_LET_KW))
  {
    compile_let_decl(comp, chunk, err);
    if (!rak_is_ok(err)) return;
  }
  Loop loop;
  begin_loop(comp, chunk, &loop);
  uint16_t off = (uint16_t) chunk->instrs.len;
  compile_expr(comp, chunk, err);
  if (!rak_is_ok(err)) return;
  uint16_t jump = emit_instr(chunk, rak_nop_instr(), err);
  if (!rak_is_ok(err)) return;
  emit_instr(chunk, rak_pop_instr(), err);
  if (!rak_is_ok(err)) return;
  if (!match(comp, RAK_TOKEN_KIND_LBRACE))
  {
    expected_token_error(err, RAK_TOKEN_KIND_LBRACE, comp->lex->tok);
    return;
  }
  compile_block(comp, chunk, err);
  if (!rak_is_ok(err)) return;
  emit_instr(chunk, rak_jump_instr(off), err);
  if (!rak_is_ok(err)) return;
  uint32_t instr = rak_jump_if_false_instr((uint16_t) chunk->instrs.len);
  patch_instr(chunk, jump, instr);
  emit_instr(chunk, rak_pop_instr(), err);
  if (!rak_is_ok(err)) return;
  end_loop(comp, chunk);
  end_scope(comp, chunk, err);
}

static inline void compile_do_while_stmt(Compiler *comp, RakChunk *chunk, RakError *err)
{
  next(comp, err);
  Loop loop;
  begin_loop(comp, chunk, &loop);
  uint16_t off = (uint16_t) chunk->instrs.len;
  if (!match(comp, RAK_TOKEN_KIND_LBRACE))
  {
    expected_token_error(err, RAK_TOKEN_KIND_LBRACE, comp->lex->tok);
    return;
  }
  compile_block(comp, chunk, err);
  if (!rak_is_ok(err)) return;
  consume(comp, RAK_TOKEN_KIND_WHILE_KW, err);
  compile_expr(comp, chunk, err);
  if (!rak_is_ok(err)) return;
  consume(comp, RAK_TOKEN_KIND_SEMICOLON, err);
  uint16_t jump = emit_instr(chunk, rak_nop_instr(), err);
  if (!rak_is_ok(err)) return;
  emit_instr(chunk, rak_pop_instr(), err);
  if (!rak_is_ok(err)) return;
  emit_instr(chunk, rak_jump_instr(off), err);
  if (!rak_is_ok(err)) return;
  uint32_t instr = rak_jump_if_false_instr((uint16_t) chunk->instrs.len);
  patch_instr(chunk, jump, instr);
  emit_instr(chunk, rak_pop_instr(), err);
  if (!rak_is_ok(err)) return;
  end_loop(comp, chunk);
}

static inline void compile_break_stmt(Compiler *comp, RakChunk *chunk, RakError *err)
{
  RakToken tok = comp->lex->tok;
  next(comp, err);
  consume(comp, RAK_TOKEN_KIND_SEMICOLON, err);
  Loop *loop = comp->loop;
  if (!loop)
  {
    rak_error_set(err, "break statement not in loop at %d:%d",
      tok.ln, tok.col);
    return;
  }
  uint16_t jump = emit_instr(chunk, rak_nop_instr(), err);
  if (!rak_is_ok(err)) return;
  if (rak_slice_is_full(&loop->jumps))
  {
    rak_error_set(err, "too many break statements in loop at %d:%d",
      tok.ln, tok.col);
    return;
  }
  rak_slice_append(&loop->jumps, jump);
}

static inline void compile_continue_stmt(Compiler *comp, RakChunk *chunk, RakError *err)
{
  RakToken tok = comp->lex->tok;
  next(comp, err);
  consume(comp, RAK_TOKEN_KIND_SEMICOLON, err);
  Loop *loop = comp->loop;
  if (!loop)
  {
    rak_error_set(err, "continue statement not in loop at %d:%d",
      tok.ln, tok.col);
    return;
  }
  emit_instr(chunk, rak_jump_instr(loop->off), err);
}

static inline void compile_return_stmt(Compiler *comp, RakChunk *chunk, RakError *err)
{
  next(comp, err);
  if (match(comp, RAK_TOKEN_KIND_SEMICOLON))
  {
    next(comp, err);
    emit_instr(chunk, rak_push_nil_instr(), err);
    if (!rak_is_ok(err)) return;
    emit_instr(chunk, rak_return_instr(), err);
    return;
  }
  compile_expr(comp, chunk, err);
  if (!rak_is_ok(err)) return;
  consume(comp, RAK_TOKEN_KIND_SEMICOLON, err);
  emit_return(chunk, err);
}

static inline void compile_expr_stmt(Compiler *comp, RakChunk *chunk, RakError *err)
{
  compile_expr(comp, chunk, err);
  if (!rak_is_ok(err)) return;
  consume(comp, RAK_TOKEN_KIND_SEMICOLON, err);
  emit_instr(chunk, rak_pop_instr(), err);
}

static inline void compile_expr(Compiler *comp, RakChunk *chunk, RakError *err)
{
  if (match(comp, RAK_TOKEN_KIND_AMP))
  {
    compile_assign_expr(comp, chunk, err);
    return;
  }
  compile_or_expr(comp, chunk, err);
}

static inline void compile_assign_expr(Compiler *comp, RakChunk *chunk, RakError *err)
{
  next(comp, err);
  if (!match(comp, RAK_TOKEN_KIND_IDENT))
  {
    expected_token_error(err, RAK_TOKEN_KIND_IDENT, comp->lex->tok);
    return;
  }
  RakToken tok = comp->lex->tok;
  next(comp, err);
  int idx = resolve_local(comp, tok);
  if (!rak_is_ok(err)) return;
  if (idx == -1)
  {
    rak_error_set(err, "variable '%.*s' used, but not defined at %d:%d",
      tok.len, tok.chars, tok.ln, tok.col);
    return;
  }
  if (match(comp, RAK_TOKEN_KIND_EQ))
  {
    next(comp, err);
    compile_expr(comp, chunk, err);
    if (!rak_is_ok(err)) return;
    emit_instr(chunk, rak_dup_instr(), err);
    if (!rak_is_ok(err)) return;
    emit_instr(chunk, rak_store_local_instr(idx), err);
    return;
  }
  uint32_t instr = 0;
  compile_assign_op(comp, &instr, err);
  if (!rak_is_ok(err)) return;
  if (instr)
  {
    emit_instr(chunk, rak_load_local_instr(idx), err);
    if (!rak_is_ok(err)) return;
    compile_expr(comp, chunk, err);
    if (!rak_is_ok(err)) return;
    emit_instr(chunk, instr, err);
    if (!rak_is_ok(err)) return;
    emit_instr(chunk, rak_dup_instr(), err);
    if (!rak_is_ok(err)) return;
    emit_instr(chunk, rak_store_local_instr(idx), err);
    return;
  }
  emit_instr(chunk, rak_fetch_local_instr(idx), err);
  if (!rak_is_ok(err)) return;
  compile_assign_expr_cont(comp, chunk, err);
  if (!rak_is_ok(err)) return;
  emit_instr(chunk, rak_dup_instr(), err);
  if (!rak_is_ok(err)) return;
  emit_instr(chunk, rak_store_local_instr(idx), err);
}

static inline void compile_assign_op(Compiler *comp, uint32_t *instr, RakError *err)
{
  RakTokenKind kind = comp->lex->tok.kind;
  if (kind == RAK_TOKEN_KIND_PLUSEQ)
  {
    next(comp, err);
    *instr = rak_add_instr();
    return;
  }
  if (kind == RAK_TOKEN_KIND_MINUSEQ)
  {
    next(comp, err);
    *instr = rak_sub_instr();
    return;
  }
  if (kind == RAK_TOKEN_KIND_STAREQ)
  {
    next(comp, err);
    *instr = rak_mul_instr();
    return;
  }
  if (kind == RAK_TOKEN_KIND_SLASHEQ)
  {
    next(comp, err);
    *instr = rak_div_instr();
    return;
  }
  if (kind != RAK_TOKEN_KIND_PERCENTEQ) return;
  next(comp, err);
  *instr = rak_mod_instr();
}

static inline void compile_assign_expr_cont(Compiler *comp, RakChunk *chunk, RakError *err)
{
  if (match(comp, RAK_TOKEN_KIND_LBRACKET))
  {
    next(comp, err);
    compile_expr(comp, chunk, err);
    if (!rak_is_ok(err)) return;
    consume(comp, RAK_TOKEN_KIND_RBRACKET, err);
    if (match(comp, RAK_TOKEN_KIND_EQ))
    {
      next(comp, err);
      compile_expr(comp, chunk, err);
      if (!rak_is_ok(err)) return;
      emit_instr(chunk, rak_set_element_instr(), err);
      return;
    }
    uint32_t instr = 0;
    compile_assign_op(comp, &instr, err);
    if (!rak_is_ok(err)) return;
    if (instr)
    {
      emit_instr(chunk, rak_load_element_instr(), err);
      if (!rak_is_ok(err)) return;
      compile_expr(comp, chunk, err);
      if (!rak_is_ok(err)) return;
      emit_instr(chunk, instr, err);
      if (!rak_is_ok(err)) return;
      emit_instr(chunk, rak_update_element_instr(), err);
      return;
    }
    emit_instr(chunk, rak_fetch_element_instr(), err);
    if (!rak_is_ok(err)) return;
    compile_assign_expr_cont(comp, chunk, err);
    if (!rak_is_ok(err)) return;
    emit_instr(chunk, rak_update_element_instr(), err);
    return;
  }
  if (!match(comp, RAK_TOKEN_KIND_DOT))
  {
    unexpected_token_error(err, comp->lex->tok);
    return;
  }
  next(comp, err);
  if (!match(comp, RAK_TOKEN_KIND_IDENT))
  {
    expected_token_error(err, RAK_TOKEN_KIND_IDENT, comp->lex->tok);
    return;
  }
  RakToken tok = comp->lex->tok;
  next(comp, err);
  RakString *str = rak_string_new_from_cstr(tok.len, tok.chars, err);
  if (!rak_is_ok(err)) return;
  RakValue val = rak_string_value(str);
  uint8_t idx = rak_chunk_append_const(chunk, val, err);
  if (!rak_is_ok(err))
  {
    rak_string_free(str);
    return;
  }
  if (match(comp, RAK_TOKEN_KIND_EQ))
  {
    next(comp, err);
    compile_expr(comp, chunk, err);
    if (!rak_is_ok(err)) return;
    emit_instr(chunk, rak_put_field_instr(idx), err);
    return;
  }
  uint32_t instr = 0;
  compile_assign_op(comp, &instr, err);
  if (!rak_is_ok(err)) return;
  if (instr)
  {
    emit_instr(chunk, rak_load_field_instr(idx), err);
    if (!rak_is_ok(err)) return;
    compile_expr(comp, chunk, err);
    if (!rak_is_ok(err)) return;
    emit_instr(chunk, instr, err);
    if (!rak_is_ok(err)) return;
    emit_instr(chunk, rak_update_field_instr(), err);
    return;
  }
  emit_instr(chunk, rak_fetch_field_instr(idx), err);
  if (!rak_is_ok(err)) return;
  compile_assign_expr_cont(comp, chunk, err);
  if (!rak_is_ok(err)) return;
  emit_instr(chunk, rak_update_field_instr(), err);
}

static inline void compile_or_expr(Compiler *comp, RakChunk *chunk, RakError *err)
{
  compile_and_expr(comp, chunk, err);
  if (!rak_is_ok(err)) return;
  compile_or_expr_cont(comp, chunk, NULL, err);
}

static inline void compile_or_expr_cont(Compiler *comp, RakChunk *chunk, uint16_t *off, RakError *err)
{
  if (!match(comp, RAK_TOKEN_KIND_PIPEPIPE))
  {
    if (off) *off = (uint16_t) chunk->instrs.len;
    return;
  }
  next(comp, err);
  uint16_t jump = emit_instr(chunk, rak_nop_instr(), err);
  if (!rak_is_ok(err)) return;
  emit_instr(chunk, rak_pop_instr(), err);
  if (!rak_is_ok(err)) return;
  compile_and_expr(comp, chunk, err);
  if (!rak_is_ok(err)) return;
  uint16_t _off;
  compile_or_expr_cont(comp, chunk, &_off, err);
  if (!rak_is_ok(err)) return;
  patch_instr(chunk, jump, rak_jump_instr(_off));
  if (off) *off = _off;
}

static inline void compile_and_expr(Compiler *comp, RakChunk *chunk, RakError *err)
{
  compile_eq_expr(comp, chunk, err);
  if (!rak_is_ok(err)) return;
  compile_and_expr_cont(comp, chunk, NULL, err);
}

static inline void compile_and_expr_cont(Compiler *comp, RakChunk *chunk, uint16_t *off, RakError *err)
{
  if (!match(comp, RAK_TOKEN_KIND_AMPAMP))
  {
    if (off) *off = (uint16_t) chunk->instrs.len;
    return;
  }
  next(comp, err);
  uint16_t jump = emit_instr(chunk, rak_nop_instr(), err);
  if (!rak_is_ok(err)) return;
  emit_instr(chunk, rak_pop_instr(), err);
  if (!rak_is_ok(err)) return;
  compile_eq_expr(comp, chunk, err);
  if (!rak_is_ok(err)) return;
  uint16_t _off;
  compile_and_expr_cont(comp, chunk, &_off, err);
  if (!rak_is_ok(err)) return;
  patch_instr(chunk, jump, rak_jump_if_false_instr(_off));
  if (off) *off = _off;
}

static inline void compile_eq_expr(Compiler *comp, RakChunk *chunk, RakError *err)
{
  compile_cmp_expr(comp, chunk, err);
  if (!rak_is_ok(err)) return;
  for (;;)
  {
    if (match(comp, RAK_TOKEN_KIND_EQEQ))
    {
      next(comp, err);
      compile_cmp_expr(comp, chunk, err);
      if (!rak_is_ok(err)) return;
      emit_instr(chunk, rak_eq_instr(), err);
      if (!rak_is_ok(err)) return;
      continue;
    }
    if (match(comp, RAK_TOKEN_KIND_BANGEQ))
    {
      next(comp, err);
      compile_cmp_expr(comp, chunk, err);
      if (!rak_is_ok(err)) return;
      emit_instr(chunk, rak_eq_instr(), err);
      if (!rak_is_ok(err)) return;
      emit_instr(chunk, rak_not_instr(), err);
      if (!rak_is_ok(err)) return;
      continue;
    }
    break;
  }
}

static inline void compile_cmp_expr(Compiler *comp, RakChunk *chunk, RakError *err)
{
  compile_range_expr(comp, chunk, err);
  if (!rak_is_ok(err)) return;
  for (;;)
  {
    if (match(comp, RAK_TOKEN_KIND_GTEQ))
    {
      next(comp, err);
      compile_range_expr(comp, chunk, err);
      if (!rak_is_ok(err)) return;
      emit_instr(chunk, rak_lt_instr(), err);
      if (!rak_is_ok(err)) return;
      emit_instr(chunk, rak_not_instr(), err);
      if (!rak_is_ok(err)) return;
      continue;
    }
    if (match(comp, RAK_TOKEN_KIND_GT))
    {
      next(comp, err);
      compile_range_expr(comp, chunk, err);
      if (!rak_is_ok(err)) return;
      emit_instr(chunk, rak_gt_instr(), err);
      if (!rak_is_ok(err)) return;
      continue;
    }
    if (match(comp, RAK_TOKEN_KIND_LTEQ))
    {
      next(comp, err);
      compile_range_expr(comp, chunk, err);
      if (!rak_is_ok(err)) return;
      emit_instr(chunk, rak_gt_instr(), err);
      if (!rak_is_ok(err)) return;
      emit_instr(chunk, rak_not_instr(), err);
      if (!rak_is_ok(err)) return;
      continue;
    }
    if (match(comp, RAK_TOKEN_KIND_LT))
    {
      next(comp, err);
      compile_range_expr(comp, chunk, err);
      if (!rak_is_ok(err)) return;
      emit_instr(chunk, rak_lt_instr(), err);
      if (!rak_is_ok(err)) return;
      continue;
    }
    break;
  }
}

static inline void compile_range_expr(Compiler *comp, RakChunk *chunk, RakError *err)
{
  compile_add_expr(comp, chunk, err);
  if (!rak_is_ok(err)) return;
  if (!match(comp, RAK_TOKEN_KIND_DOTDOT)) return;
  next(comp, err);
  compile_add_expr(comp, chunk, err);
  if (!rak_is_ok(err)) return;
  emit_instr(chunk, rak_new_range_instr(), err);
}

static inline void compile_add_expr(Compiler *comp, RakChunk *chunk, RakError *err)
{
  compile_mul_expr(comp, chunk, err);
  if (!rak_is_ok(err)) return;
  for (;;)
  {
    if (match(comp, RAK_TOKEN_KIND_PLUS))
    {
      next(comp, err);
      compile_mul_expr(comp, chunk, err);
      if (!rak_is_ok(err)) return;
      emit_instr(chunk, rak_add_instr(), err);
      if (!rak_is_ok(err)) return;
      continue;
    }
    if (match(comp, RAK_TOKEN_KIND_MINUS))
    {
      next(comp, err);
      compile_mul_expr(comp, chunk, err);
      if (!rak_is_ok(err)) return;
      emit_instr(chunk, rak_sub_instr(), err);
      if (!rak_is_ok(err)) return;
      continue;
    }
    break;
  }
}

static inline void compile_mul_expr(Compiler *comp, RakChunk *chunk, RakError *err)
{
  compile_unary_expr(comp, chunk, err);
  if (!rak_is_ok(err)) return;
  for (;;)
  {
    if (match(comp, RAK_TOKEN_KIND_STAR))
    {
      next(comp, err);
      compile_unary_expr(comp, chunk, err);
      if (!rak_is_ok(err)) return;
      emit_instr(chunk, rak_mul_instr(), err);
      if (!rak_is_ok(err)) return;
      continue;
    }
    if (match(comp, RAK_TOKEN_KIND_SLASH))
    {
      next(comp, err);
      compile_unary_expr(comp, chunk, err);
      if (!rak_is_ok(err)) return;
      emit_instr(chunk, rak_div_instr(), err);
      if (!rak_is_ok(err)) return;
      continue;
    }
    if (match(comp, RAK_TOKEN_KIND_PERCENT))
    {
      next(comp, err);
      compile_unary_expr(comp, chunk, err);
      if (!rak_is_ok(err)) return;
      emit_instr(chunk, rak_mod_instr(), err);
      if (!rak_is_ok(err)) return;
      continue;
    }
    break;
  }
}

static inline void compile_unary_expr(Compiler *comp, RakChunk *chunk, RakError *err)
{
  if (match(comp, RAK_TOKEN_KIND_BANG))
  {
    next(comp, err);
    compile_unary_expr(comp, chunk, err);
    if (!rak_is_ok(err)) return;
    emit_instr(chunk, rak_not_instr(), err);
    return;
  }
  if (match(comp, RAK_TOKEN_KIND_MINUS))
  {
    next(comp, err);
    compile_unary_expr(comp, chunk, err);
    if (!rak_is_ok(err)) return;
    emit_instr(chunk, rak_neg_instr(), err);
    return;
  }
  compile_call_expr(comp, chunk, err);
}

static inline void compile_call_expr(Compiler *comp, RakChunk *chunk, RakError *err)
{
  compile_prim_expr(comp, chunk, err);
  if (!rak_is_ok(err)) return;
  for (;;)
  {
    bool ok;
    compile_call(comp, chunk, &ok, err);
    if (!rak_is_ok(err)) return;
    if (ok) continue;
    compile_subscr(comp, chunk, &ok, err);
    if (!rak_is_ok(err)) return;
    if (ok) continue;
    break;
  }
}

static inline void compile_call(Compiler *comp, RakChunk *chunk, bool *ok, RakError *err)
{
  if (match(comp, RAK_TOKEN_KIND_LPAREN))
  {
    next(comp, err);
    if (match(comp, RAK_TOKEN_KIND_RPAREN))
    {
      next(comp, err);
      emit_instr(chunk, rak_call_instr(0), err);
      if (!rak_is_ok(err)) return;
      *ok = true;
      return;
    }
    int nargs = 0;
    compile_expr_list(comp, chunk, &nargs, err);
    consume(comp, RAK_TOKEN_KIND_RPAREN, err);
    if (nargs > UINT8_MAX)
    {
      rak_error_set(err, "too many arguments to call at %d:%d",
        comp->lex->tok.ln, comp->lex->tok.col);
      return;
    }
    emit_instr(chunk, rak_call_instr((uint8_t) nargs), err);
    if (!rak_is_ok(err)) return;
    *ok = true;
    return;
  }
  *ok = false;
}

static inline void compile_expr_list(Compiler *comp, RakChunk *chunk, int *n, RakError *err)
{
  compile_expr(comp, chunk, err);
  if (!rak_is_ok(err)) return;
  int _n = 1;
  while (match(comp, RAK_TOKEN_KIND_COMMA))
  {
    next(comp, err);
    compile_expr(comp, chunk, err);
    if (!rak_is_ok(err)) return;
    ++_n;
  }
  *n = _n;
}

static inline void compile_subscr(Compiler *comp, RakChunk *chunk, bool *ok, RakError *err)
{
  if (match(comp, RAK_TOKEN_KIND_LBRACKET))
  {
    next(comp, err);
    compile_expr(comp, chunk, err);
    if (!rak_is_ok(err)) return;
    consume(comp, RAK_TOKEN_KIND_RBRACKET, err);
    emit_instr(chunk, rak_get_element_instr(), err);
    if (!rak_is_ok(err)) return;
    *ok = true;
    return;
  }
  if (match(comp, RAK_TOKEN_KIND_DOT))
  {
    next(comp, err);
    if (!match(comp, RAK_TOKEN_KIND_IDENT))
    {
      expected_token_error(err, RAK_TOKEN_KIND_IDENT, comp->lex->tok);
      return;
    }
    RakToken tok = comp->lex->tok;
    next(comp, err);
    RakString *str = rak_string_new_from_cstr(tok.len, tok.chars, err);
    if (!rak_is_ok(err)) return;
    RakValue val = rak_string_value(str);
    uint8_t idx = rak_chunk_append_const(chunk, val, err);
    if (!rak_is_ok(err))
    {
      rak_string_free(str);
      return;
    }
    emit_instr(chunk, rak_get_field_instr(idx), err);
    if (!rak_is_ok(err)) return;
    *ok = true;
    return;
  }
  *ok = false;
}

static inline void compile_prim_expr(Compiler *comp, RakChunk *chunk, RakError *err)
{
  if (match(comp, RAK_TOKEN_KIND_NIL_KW))
  {
    next(comp, err);
    emit_instr(chunk, rak_push_nil_instr(), err);
    return;
  }
  if (match(comp, RAK_TOKEN_KIND_FALSE_KW))
  {
    next(comp, err);
    emit_instr(chunk, rak_push_false_instr(), err);
    return;
  }
  if (match(comp, RAK_TOKEN_KIND_TRUE_KW))
  {
    next(comp, err);
    emit_instr(chunk, rak_push_true_instr(), err);
    return;
  }
  if (match(comp, RAK_TOKEN_KIND_NUMBER))
  {
    RakToken tok = comp->lex->tok;
    next(comp, err);
    RakValue val = rak_number_value_from_cstr(tok.len, tok.chars, err);
    if (!rak_is_ok(err)) return;
    if (rak_is_integer(val)
     && rak_as_integer(val) >= 0
     && rak_as_integer(val) <= UINT16_MAX)
    {
      uint16_t data = (uint16_t) rak_as_integer(val);
      emit_instr(chunk, rak_push_int_instr(data), err);
      return;
    }
    uint8_t idx = rak_chunk_append_const(chunk, val, err);
    if (!rak_is_ok(err)) return;
    emit_instr(chunk, rak_load_const_instr(idx), err);
    return;
  }
  if (match(comp, RAK_TOKEN_KIND_STRING))
  {
    RakToken tok = comp->lex->tok;
    next(comp, err);
    RakString *str = rak_string_new_from_cstr(tok.len, tok.chars, err);
    if (!rak_is_ok(err)) return;
    RakValue val = rak_string_value(str);
    uint8_t idx = rak_chunk_append_const(chunk, val, err);
    if (!rak_is_ok(err))
    {
      rak_string_free(str);
      return;
    }
    emit_instr(chunk, rak_load_const_instr(idx), err);
    return;
  }
  if (match(comp, RAK_TOKEN_KIND_IDENT))
  {
    RakToken tok = comp->lex->tok;
    next(comp, err);
    int idx = resolve_local(comp, tok);
    if (idx != -1)
    {
      emit_instr(chunk, rak_load_local_instr((uint8_t) idx), err);
      return;
    }
    idx = rak_builtin_resolve_global(tok.len, tok.chars);
    if (idx != -1)
    {
      emit_instr(chunk, rak_load_global_instr((uint8_t) idx), err);
      return;
    }
    rak_error_set(err, "undefined symbol '%.*s' at %d:%d", tok.len, tok.chars,
      tok.ln, tok.col);
    return;
  }
  if (match(comp, RAK_TOKEN_KIND_LBRACKET))
  {
    compile_array(comp, chunk, err);
    return;
  }
  if (match(comp, RAK_TOKEN_KIND_LBRACE))
  {
    compile_record(comp, chunk, err);
    return;
  }
  if (match(comp, RAK_TOKEN_KIND_FN_KW))
  {
    compile_fn(comp, chunk, err);
    return;
  }
  if (match(comp, RAK_TOKEN_KIND_IF_KW))
  {
    compile_if_expr(comp, chunk, NULL, err);
    return;
  }
  if (match(comp, RAK_TOKEN_KIND_LPAREN))
  {
    compile_group(comp, chunk, err);
    return;
  }
  unexpected_token_error(err, comp->lex->tok);
}

static inline void compile_array(Compiler *comp, RakChunk *chunk, RakError *err)
{
  next(comp, err);
  if (match(comp, RAK_TOKEN_KIND_RBRACKET))
  {
    next(comp, err);
    emit_instr(chunk, rak_new_array_instr(0), err);
    return;
  }
  int len = 0;
  compile_expr_list(comp, chunk, &len, err);
  if (!rak_is_ok(err)) return;
  consume(comp, RAK_TOKEN_KIND_RBRACKET, err);
  if (len > UINT8_MAX)
  {
    rak_error_set(err, "array length too long at %d:%d",
      comp->lex->tok.ln, comp->lex->tok.col);
    return;
  }
  emit_instr(chunk, rak_new_array_instr((uint8_t) len), err);
}

static inline void compile_record(Compiler *comp, RakChunk *chunk, RakError *err)
{
  next(comp, err);
  if (match(comp, RAK_TOKEN_KIND_RBRACE))
  {
    next(comp, err);
    emit_instr(chunk, rak_new_record_instr(0), err);
    return;
  }
  compile_field(comp, chunk, err);
  if (!rak_is_ok(err)) return;
  int len = 1;
  while (match(comp, RAK_TOKEN_KIND_COMMA))
  {
    next(comp, err);
    compile_field(comp, chunk, err);
    if (!rak_is_ok(err)) return;
    ++len;
  }
  consume(comp, RAK_TOKEN_KIND_RBRACE, err);
  if (len > UINT8_MAX)
  {
    rak_error_set(err, "record length too long at %d:%d",
      comp->lex->tok.ln, comp->lex->tok.col);
    return;
  }
  emit_instr(chunk, rak_new_record_instr((uint8_t) len), err);
}

static inline void compile_fn(Compiler *comp, RakChunk *chunk, RakError *err)
{
  next(comp, err);
  Compiler _comp;
  compiler_init(&_comp, comp, comp->lex, NULL, 0, err);
  if (!rak_is_ok(err)) return;
  begin_scope(&_comp);
  RakToken tok = {
    .len = 1,
    .chars = "_"
  };
  append_local(&_comp, tok);
  compile_params(&_comp, err);
  if (!rak_is_ok(err)) goto fail;
  if (!match(&_comp, RAK_TOKEN_KIND_LBRACE))
  {
    expected_token_error(err, RAK_TOKEN_KIND_LBRACE, _comp.lex->tok);
    goto fail;
  }
  RakChunk *_chunk = &_comp.fn->chunk;
  compile_block(&_comp, _chunk, err);
  if (!rak_is_ok(err)) goto fail;
  emit_instr(_chunk, rak_push_nil_instr(), err);
  if (!rak_is_ok(err)) goto fail;
  emit_instr(_chunk, rak_return_instr(), err);
  if (!rak_is_ok(err)) goto fail;
  end_scope(&_comp, _chunk, err);
  if (!rak_is_ok(err)) return;
  uint8_t idx = rak_function_append_nested(comp->fn, _comp.fn, err);
  if (!rak_is_ok(err)) goto fail;
  emit_instr(chunk, rak_new_closure_instr(idx), err);
  return;
fail:
  compiler_deinit(&_comp);
}

static inline void compile_field(Compiler *comp, RakChunk *chunk, RakError *err)
{
  if (!match(comp, RAK_TOKEN_KIND_IDENT))
  {
    expected_token_error(err, RAK_TOKEN_KIND_IDENT, comp->lex->tok);
    return;
  }
  RakToken tok = comp->lex->tok;
  next(comp, err);
  consume(comp, RAK_TOKEN_KIND_COLON, err);
  RakString *name = rak_string_new_from_cstr(tok.len, tok.chars, err);
  if (!rak_is_ok(err)) return;
  RakValue val = rak_string_value(name);
  uint8_t idx = rak_chunk_append_const(chunk, val, err);
  if (!rak_is_ok(err))
  {
    rak_string_free(name);
    return;
  }
  emit_instr(chunk, rak_load_const_instr(idx), err);
  if (!rak_is_ok(err)) return;
  compile_expr(comp, chunk, err);
}

static inline void compile_if_expr(Compiler *comp, RakChunk *chunk, uint16_t *off, RakError *err)
{
  next(comp, err);
  compile_expr(comp, chunk, err);
  if (!rak_is_ok(err)) return;
  uint16_t jump1 = emit_instr(chunk, rak_nop_instr(), err);
  if (!rak_is_ok(err)) return;
  emit_instr(chunk, rak_pop_instr(), err);
  if (!rak_is_ok(err)) return;
  compile_block_expr(comp, chunk, err);
  if (!rak_is_ok(err)) return;
  uint16_t jump2 = emit_instr(chunk, rak_nop_instr(), err);
  if (!rak_is_ok(err)) return;
  uint32_t instr = rak_jump_if_false_instr((uint16_t) chunk->instrs.len);
  patch_instr(chunk, jump1, instr);
  uint16_t _off;
  compile_if_expr_cont(comp, chunk, &_off, err);
  if (!rak_is_ok(err)) return;
  patch_instr(chunk, jump2, rak_jump_instr(_off));
  if (off) *off = _off;
}

static inline void compile_block_expr(Compiler *comp, RakChunk *chunk, RakError *err)
{
  consume(comp, RAK_TOKEN_KIND_LBRACE, err);
  compile_expr(comp, chunk, err);
  if (!rak_is_ok(err)) return;
  consume(comp, RAK_TOKEN_KIND_RBRACE, err);
}

static inline void compile_if_expr_cont(Compiler *comp, RakChunk *chunk, uint16_t *off, RakError *err)
{
  emit_instr(chunk, rak_pop_instr(), err);
  if (!rak_is_ok(err)) return;
  if (!match(comp, RAK_TOKEN_KIND_ELSE_KW))
  {
    emit_instr(chunk, rak_push_nil_instr(), err);
    if (!rak_is_ok(err)) return;
    *off = (uint16_t) chunk->instrs.len;
    return;
  }
  next(comp, err);
  if (match(comp, RAK_TOKEN_KIND_IF_KW))
  {
    compile_if_expr(comp, chunk, off, err);
    return;
  }
  compile_block_expr(comp, chunk, err);
  if (!rak_is_ok(err)) return;
  *off = (uint16_t) chunk->instrs.len;
}

static inline void compile_group(Compiler *comp, RakChunk *chunk, RakError *err)
{
  next(comp, err);
  compile_expr(comp, chunk, err);
  if (!rak_is_ok(err)) return;
  consume(comp, RAK_TOKEN_KIND_RPAREN, err);
}

static inline void begin_scope(Compiler *comp)
{
  ++comp->scopeDepth;
}

static inline void end_scope(Compiler *comp, RakChunk *chunk, RakError *err)
{
  int len = comp->symbols.len;
  int n = 0;
  for (int i = len - 1; i >= 0; --i)
  {
    Symbol sym = rak_slice_get(&comp->symbols, i);
    if (sym.depth != comp->scopeDepth) break;
    emit_instr(chunk, rak_pop_instr(), err);
    if (!rak_is_ok(err)) return;
    ++n;
  }
  comp->symbols.len -= n;
  --comp->scopeDepth;
}

static inline void begin_loop(Compiler *comp, RakChunk *chunk, Loop *loop)
{
  loop->parent = comp->loop;
  loop->off = (uint16_t) chunk->instrs.len;
  rak_static_slice_init(&loop->jumps);
  comp->loop = loop;
}

static inline void end_loop(Compiler *comp, RakChunk *chunk)
{
  int len = comp->loop->jumps.len;
  uint32_t instr = rak_jump_instr((uint16_t) chunk->instrs.len);
  for (int i = 0; i < len; ++i)
  {
    uint16_t jump = rak_slice_get(&comp->loop->jumps, i);
    patch_instr(chunk, jump, instr);
  }
  comp->loop = comp->loop->parent;
}

static inline void define_local(Compiler *comp, RakToken tok, RakError *err)
{
  int len = comp->symbols.len;
  for (int i = len - 1; i >= 0; --i)
  {
    Symbol sym = rak_slice_get(&comp->symbols, i);
    if (sym.depth != comp->scopeDepth) break;
    if (!ident_equals(sym.tok, tok)) continue;
    rak_error_set(err, "duplicate local variable '%.*s' at %d:%d",
      tok.len, tok.chars, tok.ln, tok.col);
    return;
  }
  if (len > UINT8_MAX)
  {
    rak_error_set(err, "too many local variables at %d:%d",
      tok.ln, tok.col);
    return;
  }
  append_local(comp, tok);
}

static inline void append_local(Compiler *comp, RakToken tok)
{
  uint8_t idx = (uint8_t) comp->symbols.len;
  Symbol sym = {
    .tok = tok,
    .idx = idx,
    .depth = comp->scopeDepth
  };
  rak_slice_append(&comp->symbols, sym);
}

static inline int resolve_local(Compiler *comp, RakToken tok)
{
  if (is_blank_ident(tok)) return -1;
  int len = comp->symbols.len;
  for (int i = len - 1; i >= 0; --i)
  {
    Symbol sym = rak_slice_get(&comp->symbols, i);
    if (!ident_equals(sym.tok, tok)) continue;
    return sym.idx;
    break;
  }
  return -1;
}

static inline bool ident_equals(RakToken tok1, RakToken tok2)
{
  int len = tok1.len;
  if (len != tok2.len) return false;
  return !memcmp(tok1.chars, tok2.chars, len);
}

static inline void emit_return(RakChunk *chunk, RakError *err)
{
  int off = chunk->instrs.len - 1;
  uint32_t instr = rak_slice_get(&chunk->instrs, off);
  RakOpcode op = rak_instr_opcode(instr);
  if (op == RAK_OP_CALL)
  {
    uint8_t nargs = rak_instr_a(instr);
    instr = rak_tail_call_instr(nargs);
    rak_slice_set(&chunk->instrs, off, instr);
    return;
  }
  emit_instr(chunk, rak_return_instr(), err);
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

RakFunction *rak_compile(char *source, RakError *err)
{
  RakLexer lex;
  rak_lexer_init(&lex, source, err);
  if (!rak_is_ok(err)) return NULL;
  RakString *fnName = rak_string_new_from_cstr(-1, "main", err);
  if (!rak_is_ok(err)) return NULL;
  Compiler comp;
  compiler_init(&comp, NULL, &lex, fnName, 0, err);
  if (!rak_is_ok(err))
  {
    rak_string_free(fnName);
    return NULL;
  }
  RakToken tok = {
    .len = rak_string_len(fnName),
    .chars = rak_string_chars(fnName)
  };
  append_local(&comp, tok);
  compile_chunk(&comp, &comp.fn->chunk, err);
  if (rak_is_ok(err)) return comp.fn;
  compiler_deinit(&comp);
  return NULL;
}
