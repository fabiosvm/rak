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

#include "value.h"

typedef enum
{
  RAK_TOKEN_KIND_EOF,
  RAK_TOKEN_KIND_COMMA,
  RAK_TOKEN_KIND_COLON,
  RAK_TOKEN_KIND_SEMICOLON,
  RAK_TOKEN_KIND_LPAREN,
  RAK_TOKEN_KIND_RPAREN,
  RAK_TOKEN_KIND_LBRACKET,
  RAK_TOKEN_KIND_RBRACKET,
  RAK_TOKEN_KIND_LBRACE,
  RAK_TOKEN_KIND_RBRACE,
  RAK_TOKEN_KIND_PIPEPIPE,
  RAK_TOKEN_KIND_AMPAMP,
  RAK_TOKEN_KIND_EQEQ,
  RAK_TOKEN_KIND_EQ,
  RAK_TOKEN_KIND_BANGEQ,
  RAK_TOKEN_KIND_BANG,
  RAK_TOKEN_KIND_GTEQ,
  RAK_TOKEN_KIND_GT,
  RAK_TOKEN_KIND_LTEQ,
  RAK_TOKEN_KIND_LT,
  RAK_TOKEN_KIND_DOTDOT,
  RAK_TOKEN_KIND_DOT,
  RAK_TOKEN_KIND_PLUS,
  RAK_TOKEN_KIND_MINUS,
  RAK_TOKEN_KIND_STAR,
  RAK_TOKEN_KIND_SLASH,
  RAK_TOKEN_KIND_PERCENT,
  RAK_TOKEN_KIND_NUMBER,
  RAK_TOKEN_KIND_STRING,
  RAK_TOKEN_KIND_ECHO_KW,
  RAK_TOKEN_KIND_ELSE_KW,
  RAK_TOKEN_KIND_FALSE_KW,
  RAK_TOKEN_KIND_IF_KW,
  RAK_TOKEN_KIND_LET_KW,
  RAK_TOKEN_KIND_NIL_KW,
  RAK_TOKEN_KIND_TRUE_KW,
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
  RakToken  tok;
} RakLexer;

const char *rak_token_kind_to_cstr(RakTokenKind kind);
void rak_lexer_init(RakLexer *lex, char *source, RakError *err);
void rak_lexer_next(RakLexer *lex, RakError *err);

#endif // RAK_LEXER_H
