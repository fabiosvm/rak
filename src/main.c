//
// main.c
//
// Copyright 2025 Fábio de Souza Villaça Medeiros
//
// This file is part of the Rak Project.
// For detailed license information, please refer to the LICENSE file
// located in the root directory of this project.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rak.h>

#define SOURCE_MAX_LEN ((int) 1 << 12)

static RakString source;

static inline void check_error(RakError *err);
static inline void init(void);
static inline void deinit(void);
static inline void repl(void);
static inline bool read(void);
static inline void print_token(RakToken token);

static inline void check_error(RakError *err)
{
  if (rak_is_ok(err)) return;
  rak_error_print(err);
  exit(EXIT_FAILURE);
}

static inline void init(void)
{
  RakError err = rak_ok();
  rak_string_init_with_capacity(&source, SOURCE_MAX_LEN, &err);
  check_error(&err);
}

static inline void deinit(void)
{
  rak_string_deinit(&source);
}

static inline void repl(void)
{
  for (;;)
  {
    printf("> ");
    if (!read()) break;
    if (source.slice.len == 1) continue;
    RakLexer lex;
    RakError err = rak_ok();
    rak_lexer_init(&lex, rak_string_chars(&source), &err);
    do
    {
      if (!rak_is_ok(&err))
      {
        rak_error_print(&err);
        break;
      }
      print_token(lex.token);
      rak_lexer_next(&lex, &err);
    }
    while (lex.token.kind != RAK_TOKEN_KIND_EOF);
  }
}

static inline bool read(void)
{
  char *cstr = source.slice.data;
  if (!fgets(cstr, SOURCE_MAX_LEN, stdin))
    return false;
  source.slice.len = (int) strlen(cstr);
  return true;
 }

 static inline void print_token(RakToken token)
 {
  int kind = token.kind;
  const char *name = rak_token_kind_name(kind);
  int ln = token.ln;
  int col = token.col;
  int len = token.len;
  char *chars = token.chars;
  if (kind == RAK_TOKEN_KIND_INTEGER
   || kind == RAK_TOKEN_KIND_NUMBER
   || kind == RAK_TOKEN_KIND_IDENT)
  {
    printf("%s(%.*s) at %d:%d\n", name, len, chars, ln, col);
    return;
  }
  printf("%s at %d:%d\n", name, ln, col);
 }

int main(void)
{
  init();
  repl();
  deinit();
  return EXIT_SUCCESS;
}
