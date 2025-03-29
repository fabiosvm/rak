//
// lexer.c
//
// Copyright 2025 Fábio de Souza Villaça Medeiros
//
// This file is part of the Rak Project.
// For detailed license information, please refer to the LICENSE file
// located in the root directory of this project.
//

#include "rak/lexer.h"
#include <ctype.h>
#include <string.h>

#define char_at(l, i)   ((l)->curr[(i)])
#define current_char(l) char_at(l, 0)

static inline void skip_whitespace(RakLexer *lex);
static inline void next_char(RakLexer *lex);
static inline void next_chars(RakLexer *lex, int len);
static inline bool match_char(RakLexer *lex, char c, RakTokenKind kind);
static inline bool match_number(RakLexer *lex, RakError *err);
static inline bool match_keyword(RakLexer *lex, const char *kw, RakTokenKind kind);
static inline bool match_ident(RakLexer *lex);
static inline RakToken token(RakLexer *lex, RakTokenKind kind, int len, char *chars);
static inline void unexpected_character_error(RakError *err, char c, int ln, int col);

static inline void skip_whitespace(RakLexer *lex)
{
  while (isspace(current_char(lex)))
    next_char(lex);
}

static inline void next_char(RakLexer *lex)
{
  if (current_char(lex) == '\n')
  {
    ++lex->ln;
    lex->col = 1;
    ++lex->curr;
    return;
  }
  ++lex->col;
  ++lex->curr;
}

static inline void next_chars(RakLexer *lex, int len)
{
  for (int i = 0; i < len; ++i)
    next_char(lex);
}

static inline bool match_char(RakLexer *lex, char c, RakTokenKind kind)
{
  if (current_char(lex) != c) return false;
  lex->token = token(lex, kind, 1, lex->curr);
  next_char(lex);
  return true;
}

static inline bool match_number(RakLexer *lex, RakError *err)
{
  if (!isdigit(current_char(lex)))
    return false;
  int len = 1;
  while (isdigit(char_at(lex, len)))
    ++len;
  RakTokenKind kind = RAK_TOKEN_KIND_INTEGER;
  if (char_at(lex, len) == '.')
  {
    ++len;
    char c = char_at(lex, len);
    if (!isdigit(c))
    {
      int col = lex->col + len;
      unexpected_character_error(err, c, lex->ln, col);
      return false;
    }
    ++len;
    while (isdigit(char_at(lex, len)))
      ++len;
    kind = RAK_TOKEN_KIND_NUMBER;
  }
  if (char_at(lex, len) == 'e' || char_at(lex, len) == 'E')
  {
    ++len;
    if (char_at(lex, len) == '+' || char_at(lex, len) == '-')
      ++len;
    char c = char_at(lex, len);
    if (!isdigit(c))
    {
      int col = lex->col + len;
      unexpected_character_error(err, c, lex->ln, col);
      return false;
    }
    ++len;
    while (isdigit(char_at(lex, len)))
      ++len;
  }
  char c = char_at(lex, len);
  if (isalpha(c) || c == '_')
  {
    int col = lex->col + len;
    unexpected_character_error(err, c, lex->ln, col);
    return false;
  }
  lex->token = token(lex, kind, len, lex->curr);
  next_chars(lex, len);
  return true;
}

static inline bool match_keyword(RakLexer *lex, const char *kw, RakTokenKind kind)
{
  int len = (int) strlen(kw);
  if (strncmp(lex->curr, kw, len)
   || (isalnum(char_at(lex, len)))
   || (char_at(lex, len) == '_'))
    return false;
  lex->token = token(lex, kind, len, lex->curr);
  next_chars(lex, len);
  return true;
}

static inline bool match_ident(RakLexer *lex)
{
  if (current_char(lex) != '_' && !isalpha(current_char(lex)))
    return false;
  int len = 1;
  while (isalnum(char_at(lex, len)) || char_at(lex, len) == '_')
    ++len;
  lex->token = token(lex, RAK_TOKEN_KIND_IDENT, len, lex->curr);
  next_chars(lex, len);
  return true;
}

static inline RakToken token(RakLexer *lex, RakTokenKind kind, int len, char *chars)
{
  return (RakToken) {
    .kind = kind,
    .ln = lex->ln,
    .col = lex->col,
    .len = len,
    .chars = chars
  };
}

static inline void unexpected_character_error(RakError *err, char c, int ln, int col)
{
  if (c == '\0')
  {
    rak_error_set(err, "unexpected end of file at %d:%d", ln, col);
    return;
  }
  if (c == '\n')
  {
    rak_error_set(err, "unexpected newline at %d:%d", ln, col);
    return;
  }
  c = isprint(c) ? c : '?';
  rak_error_set(err, "unexpected character '%c' at %d:%d", c, ln, col);
}

const char *rak_token_kind_name(RakTokenKind kind)
{
  char *name = "Eof";
  switch (kind)
  {
  case RAK_TOKEN_KIND_EOF:
    break;
  case RAK_TOKEN_KIND_SEMICOLON:
    name = "Semicolon";
    break;
  case RAK_TOKEN_KIND_LPAREN:
    name = "LParen";
    break;
  case RAK_TOKEN_KIND_RPAREN:
    name = "RParen";
    break;
  case RAK_TOKEN_KIND_PLUS:
    name = "Plus";
    break;
  case RAK_TOKEN_KIND_MINUS:
    name = "Minus";
    break;
  case RAK_TOKEN_KIND_STAR:
    name = "Star";
    break;
  case RAK_TOKEN_KIND_SLASH:
    name = "Slash";
    break;
  case RAK_TOKEN_KIND_PERCENT:
    name = "Percent";
    break;
  case RAK_TOKEN_KIND_INTEGER:
    name = "Integer";
    break;
  case RAK_TOKEN_KIND_NUMBER:
    name = "Number";
    break;
  case RAK_TOKEN_KIND_NIL_KW:
    name = "Nil";
    break;
  case RAK_TOKEN_KIND_IDENT:
    name = "Ident";
    break;
  }
  return name;
}

void rak_lexer_init(RakLexer *lex, char *source, RakError *err)
{
  lex->source = source;
  lex->curr = lex->source;
  lex->ln = 1;
  lex->col = 1;
  rak_lexer_next(lex, err);
}

void rak_lexer_next(RakLexer *lex, RakError *err)
{
  skip_whitespace(lex);
  if (match_char(lex, '\0', RAK_TOKEN_KIND_EOF)) return;
  if (match_char(lex, ';', RAK_TOKEN_KIND_SEMICOLON)) return;
  if (match_char(lex, '(', RAK_TOKEN_KIND_LPAREN)) return;
  if (match_char(lex, ')', RAK_TOKEN_KIND_RPAREN)) return;
  if (match_char(lex, '+', RAK_TOKEN_KIND_PLUS)) return;
  if (match_char(lex, '-', RAK_TOKEN_KIND_MINUS)) return;
  if (match_char(lex, '*', RAK_TOKEN_KIND_STAR)) return;
  if (match_char(lex, '/', RAK_TOKEN_KIND_SLASH)) return;
  if (match_char(lex, '%', RAK_TOKEN_KIND_PERCENT)) return;
  if (match_number(lex, err) || !rak_is_ok(err)) return;
  if (match_keyword(lex, "nil", RAK_TOKEN_KIND_NIL_KW)) return;
  if (match_ident(lex)) return;
  unexpected_character_error(err, current_char(lex), lex->ln, lex->col);
}
