//
// lexer.h
//
// Copyright 2025 Fábio de Souza Villaça Medeiros
//
// This file is part of the Rak Project.
// For detailed license information, please refer to the LICENSE file
// located in the root directory of this project.
//

#ifndef RAK_LEXER_H
#define RAK_LEXER_H

#include "error.h"

typedef enum
{
  RAK_TOKEN_KIND_EOF,
  RAK_TOKEN_KIND_SEMICOLON,
  RAK_TOKEN_KIND_LPAREN,
  RAK_TOKEN_KIND_RPAREN,
  RAK_TOKEN_KIND_PLUS,
  RAK_TOKEN_KIND_MINUS,
  RAK_TOKEN_KIND_STAR,
  RAK_TOKEN_KIND_SLASH,
  RAK_TOKEN_KIND_PERCENT,
  RAK_TOKEN_KIND_INTEGER,
  RAK_TOKEN_KIND_NUMBER,
  RAK_TOKEN_KIND_NIL_KW,
  RAK_TOKEN_KIND_IDENT
} RakTokenKind;

typedef struct
{
  RakTokenKind  kind;
  int           ln;
  int           col;
  int           len;
  char         *chars;
} RakToken;

typedef struct
{
  char     *source;
  char     *curr;
  int       ln;
  int       col;
  RakToken  token;
} RakLexer;

const char *rak_token_kind_name(RakTokenKind kind);
void rak_lexer_init(RakLexer *lex, char *source, RakError *err);
void rak_lexer_next(RakLexer *lex, RakError *err);

#endif // RAK_LEXER_H
