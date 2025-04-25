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
#include <assert.h>
#include <ctype.h>
#include <string.h>

#define char_at(l, i)   ((l)->curr[(i)])
#define current_char(l) char_at(l, 0)

static inline void skip_whitespace(RakLexer *lex);
static inline void next_char(RakLexer *lex);
static inline void next_chars(RakLexer *lex, int len);
static inline bool match_char(RakLexer *lex, char c, RakTokenKind kind);
static inline bool match_chars(RakLexer *lex, const char *chars, RakTokenKind kind);
static inline bool match_number(RakLexer *lex, RakError *err);
static inline bool match_string(RakLexer *lex, RakError *err);
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
  lex->tok = token(lex, kind, 1, lex->curr);
  next_char(lex);
  return true;
}

static inline bool match_chars(RakLexer *lex, const char *chars, RakTokenKind kind)
{
  int len = (int) strlen(chars);
  if (memcmp(lex->curr, chars, len))
    return false;
  lex->tok = token(lex, kind, len, lex->curr);
  next_chars(lex, len);
  return true;
}

static inline bool match_number(RakLexer *lex, RakError *err)
{
  if (!isdigit(current_char(lex)))
    return false;
  int len = 1;
  while (isdigit(char_at(lex, len)))
    ++len;
  if (char_at(lex, len) == '.')
  {
    if (!isdigit(char_at(lex, len + 1))) goto end;
    len += 2;
    while (isdigit(char_at(lex, len)))
      ++len;
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
end:
  lex->tok = token(lex, RAK_TOKEN_KIND_NUMBER, len, lex->curr);
  next_chars(lex, len);
  return true;
}

static inline bool match_string(RakLexer *lex, RakError *err)
{
  if (current_char(lex) != '\"') return false;
  int len = 1;
  for (;;)
  {
    if (char_at(lex, len) == '\"')
    {
      ++len;
      break;
    }
    if (char_at(lex, len) == '\0')
    {
      rak_error_set(err, "unterminated string in %d,%d", lex->ln, lex->col);
      return false;
    }
    ++len;
  }
  lex->tok = token(lex, RAK_TOKEN_KIND_STRING, len - 2, &lex->curr[1]);
  next_chars(lex, len);
  return true;
}

static inline bool match_keyword(RakLexer *lex, const char *kw, RakTokenKind kind)
{
  int len = (int) strlen(kw);
  if (memcmp(lex->curr, kw, len)
   || (isalnum(char_at(lex, len)))
   || (char_at(lex, len) == '_'))
    return false;
  lex->tok = token(lex, kind, len, lex->curr);
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
  lex->tok = token(lex, RAK_TOKEN_KIND_IDENT, len, lex->curr);
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

const char *rak_token_kind_to_cstr(RakTokenKind kind)
{
  char *cstr = NULL;
  switch (kind)
  {
  case RAK_TOKEN_KIND_EOF:       cstr = "eof";        break;
  case RAK_TOKEN_KIND_COMMA:     cstr = "','";        break;
  case RAK_TOKEN_KIND_COLON:     cstr = "':'";        break;
  case RAK_TOKEN_KIND_SEMICOLON: cstr = "';'";        break;
  case RAK_TOKEN_KIND_LPAREN:    cstr = "'('";        break;
  case RAK_TOKEN_KIND_RPAREN:    cstr = "')'";        break;
  case RAK_TOKEN_KIND_LBRACKET:  cstr = "'['";        break;
  case RAK_TOKEN_KIND_RBRACKET:  cstr = "']'";        break;
  case RAK_TOKEN_KIND_LBRACE:    cstr = "'{'";        break;
  case RAK_TOKEN_KIND_RBRACE:    cstr = "'}'";        break;
  case RAK_TOKEN_KIND_PIPEPIPE:  cstr = "'||'";       break;
  case RAK_TOKEN_KIND_AMPAMP:    cstr = "'&&'";       break;
  case RAK_TOKEN_KIND_AMP:       cstr = "'&'";        break;
  case RAK_TOKEN_KIND_EQEQ:      cstr = "'=='";       break;
  case RAK_TOKEN_KIND_EQ:        cstr = "'='";        break;
  case RAK_TOKEN_KIND_BANGEQ:    cstr = "'!='";       break;
  case RAK_TOKEN_KIND_BANG:      cstr = "'!'";        break;
  case RAK_TOKEN_KIND_GTEQ:      cstr = "'>='";       break;
  case RAK_TOKEN_KIND_GT:        cstr = "'>'";        break;
  case RAK_TOKEN_KIND_LTEQ:      cstr = "'<='";       break;
  case RAK_TOKEN_KIND_LT:        cstr = "'<'";        break;
  case RAK_TOKEN_KIND_DOTDOT:    cstr = "'..'";       break;
  case RAK_TOKEN_KIND_DOT:       cstr = "'.'";        break;
  case RAK_TOKEN_KIND_PLUS:      cstr = "'+'";        break;
  case RAK_TOKEN_KIND_MINUS:     cstr = "'-'";        break;
  case RAK_TOKEN_KIND_STAR:      cstr = "'*'";        break;
  case RAK_TOKEN_KIND_SLASH:     cstr = "'/'";        break;
  case RAK_TOKEN_KIND_PERCENT:   cstr = "'%'";        break;
  case RAK_TOKEN_KIND_NUMBER:    cstr = "number";     break;
  case RAK_TOKEN_KIND_STRING:    cstr = "string";     break;
  case RAK_TOKEN_KIND_BREAK_KW:  cstr = "break";      break;
  case RAK_TOKEN_KIND_DO_KW:     cstr = "do";         break;
  case RAK_TOKEN_KIND_ECHO_KW:   cstr = "echo";       break;
  case RAK_TOKEN_KIND_ELSE_KW:   cstr = "else";       break;
  case RAK_TOKEN_KIND_FALSE_KW:  cstr = "false";      break;
  case RAK_TOKEN_KIND_IF_KW:     cstr = "if";         break;
  case RAK_TOKEN_KIND_LET_KW:    cstr = "let";        break;
  case RAK_TOKEN_KIND_NIL_KW:    cstr = "nil";        break;
  case RAK_TOKEN_KIND_TRUE_KW:   cstr = "true";       break;
  case RAK_TOKEN_KIND_WHILE_KW:  cstr = "while";      break;
  case RAK_TOKEN_KIND_IDENT:     cstr = "identifier"; break;
  }
  assert(cstr);
  return cstr;
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
  if (match_char(lex, ',', RAK_TOKEN_KIND_COMMA)) return;
  if (match_char(lex, ':', RAK_TOKEN_KIND_COLON)) return;
  if (match_char(lex, ';', RAK_TOKEN_KIND_SEMICOLON)) return;
  if (match_char(lex, '(', RAK_TOKEN_KIND_LPAREN)) return;
  if (match_char(lex, ')', RAK_TOKEN_KIND_RPAREN)) return;
  if (match_char(lex, '[', RAK_TOKEN_KIND_LBRACKET)) return;
  if (match_char(lex, ']', RAK_TOKEN_KIND_RBRACKET)) return;
  if (match_char(lex, '{', RAK_TOKEN_KIND_LBRACE)) return;
  if (match_char(lex, '}', RAK_TOKEN_KIND_RBRACE)) return;
  if (match_chars(lex, "||", RAK_TOKEN_KIND_PIPEPIPE)) return;
  if (match_chars(lex, "&&", RAK_TOKEN_KIND_AMPAMP)) return;
  if (match_char(lex, '&', RAK_TOKEN_KIND_AMP)) return;
  if (match_chars(lex, "==", RAK_TOKEN_KIND_EQEQ)) return;
  if (match_char(lex, '=', RAK_TOKEN_KIND_EQ)) return;
  if (match_chars(lex, "!=", RAK_TOKEN_KIND_BANGEQ)) return;
  if (match_char(lex, '!', RAK_TOKEN_KIND_BANG)) return;
  if (match_chars(lex, ">=", RAK_TOKEN_KIND_GTEQ)) return;
  if (match_char(lex, '>', RAK_TOKEN_KIND_GT)) return;
  if (match_chars(lex, "<=", RAK_TOKEN_KIND_LTEQ)) return;
  if (match_char(lex, '<', RAK_TOKEN_KIND_LT)) return;
  if (match_chars(lex, "..", RAK_TOKEN_KIND_DOTDOT)) return;
  if (match_char(lex, '.', RAK_TOKEN_KIND_DOT)) return;
  if (match_char(lex, '+', RAK_TOKEN_KIND_PLUS)) return;
  if (match_char(lex, '-', RAK_TOKEN_KIND_MINUS)) return;
  if (match_char(lex, '*', RAK_TOKEN_KIND_STAR)) return;
  if (match_char(lex, '/', RAK_TOKEN_KIND_SLASH)) return;
  if (match_char(lex, '%', RAK_TOKEN_KIND_PERCENT)) return;
  if (match_number(lex, err) || !rak_is_ok(err)) return;
  if (match_string(lex, err) || !rak_is_ok(err)) return;
  if (match_keyword(lex, "break", RAK_TOKEN_KIND_BREAK_KW)) return;
  if (match_keyword(lex, "do", RAK_TOKEN_KIND_DO_KW)) return;
  if (match_keyword(lex, "echo", RAK_TOKEN_KIND_ECHO_KW)) return;
  if (match_keyword(lex, "else", RAK_TOKEN_KIND_ELSE_KW)) return;
  if (match_keyword(lex, "false", RAK_TOKEN_KIND_FALSE_KW)) return;
  if (match_keyword(lex, "if", RAK_TOKEN_KIND_IF_KW)) return;
  if (match_keyword(lex, "let", RAK_TOKEN_KIND_LET_KW)) return;
  if (match_keyword(lex, "nil", RAK_TOKEN_KIND_NIL_KW)) return;
  if (match_keyword(lex, "true", RAK_TOKEN_KIND_TRUE_KW)) return;
  if (match_keyword(lex, "while", RAK_TOKEN_KIND_WHILE_KW)) return;
  if (match_ident(lex)) return;
  unexpected_character_error(err, current_char(lex), lex->ln, lex->col);
}
