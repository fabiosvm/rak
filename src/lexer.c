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
#include "rak/string.h"
#include <ctype.h>
#include <string.h>

#define char_at(l, i)   ((l)->curr[(i)])
#define current_char(l) char_at(l, 0)

static inline void skip_whitespace_comments(RakLexer *lex);
static inline void next_char(RakLexer *lex);
static inline void next_chars(RakLexer *lex, int len);
static inline bool match_char(RakLexer *lex, char c, RakTokenKind kind);
static inline bool match_chars(RakLexer *lex, const char *chars, RakTokenKind kind);
static inline bool match_number(RakLexer *lex, RakError *err);
static inline unsigned char hex2bin (char c);
static bool handle_hex_escape(RakLexer *lex, RakString *text, RakError *err);
static inline bool insertUnicodeChar(RakLexer *lex, RakString *text, int code, RakError *err);
static bool handle_unicode_escape(RakLexer *lex, RakString *text, RakError *err);
static bool handle_escape_sequence(RakLexer *lex, RakString *text, RakError *err);
static bool parseString(RakLexer *lex, RakError *err);
static inline bool match_string(RakLexer *lex, RakError *err);
static inline bool match_keyword(RakLexer *lex, const char *kw, RakTokenKind kind);
static inline bool match_ident(RakLexer *lex);
static inline RakToken token(RakLexer *lex, RakTokenKind kind, int len, char *chars);
static inline void unexpected_character_error(RakError *err, char c, int ln, int col);

static inline void skip_whitespace_comments(RakLexer *lex)
{
begin:
  while (isspace(current_char(lex)))
    next_char(lex);
  if ((char_at(lex, 0) != '/' || char_at(lex, 1) != '/'))
    return;
  next_chars(lex, 2);
  for (;;)
  {
    if (current_char(lex) == '\0')
      break;
    if (current_char(lex) == '\n')
    {
      next_char(lex);
      goto begin;
    }
    next_char(lex);
  }
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

static inline unsigned char hex2bin (char c)
{
  if (c >= '0' && c <= '9')
    return c - '0' ;
  if (c >= 'A' && c <= 'F')
    return c - 'A' + 10 ;
  if (c >= 'a' && c <= 'f')
    return c - 'a' + 10 ;
  return 0;
}

static bool handle_hex_escape(RakLexer *lex, RakString *text, RakError *err)
{
  if (!isxdigit(current_char(lex)) || !isxdigit(char_at(lex, 1)))
  {
    rak_error_set(
      err,
      "expecting two hexadecimal numbers for escape '\\x' at %d,%d", 
      lex->ln,
      lex->col
    );
    return false;
  }  
  unsigned char code = hex2bin(current_char(lex)) * 16 + hex2bin(char_at(lex, 1));
  if (code > 0x7f)
  {
    rak_error_set(
      err,
      "escape '\\x' is limited to ascii chars (below 0x7f) at %d,%d", 
      lex->ln,
      lex->col
    );
    return false;
  }
  rak_string_inplace_append_cstr(text, 1, (char *) &code, err);
  next_chars(lex, 2);
  return rak_is_ok(err);
}

static inline bool insertUnicodeChar(RakLexer *lex, RakString *text, int code, RakError *err)
{
  unsigned char buffer[5] = {0, 0, 0, 0, 0};
  if (code <= 0x7F) {
    buffer[0] = code;
  } else if (code <= 0x7FF) {
    buffer[0] = 0xC0 | (code >> 6);
    buffer[1] = 0x80 | (code & 0x3F);
  } else if (code <= 0xFFFF) {
    buffer[0] = 0xE0 | (code >> 12);
    buffer[1] = 0x80 | ((code >> 6) & 0x3F);
    buffer[2] = 0x80 | (code & 0x3F);
  } else if (code <= 0x10FFFF) {
    buffer[0] = 0xF0 | (code >> 18);
    buffer[1] = 0x80 | ((code >> 12) & 0x3F);
    buffer[2] = 0x80 | ((code >> 6) & 0x3F);
    buffer[3] = 0x80 | (code & 0x3F);
  } else {
    rak_error_set(
      err,
      "unicode character overflow at %d,%d",
      lex->ln,
      lex->col - 1
    );
    return false;
  }
  rak_string_inplace_append_cstr(text, -1, (char *)buffer, err);
  return rak_is_ok(err);
}

static bool handle_unicode_escape(RakLexer *lex, RakString *text, RakError *err)
{
  if (current_char(lex) != '{')
  {
    rak_error_set(
      err,
      "expecting '{' after escape '\\u' at %d,%d",
      lex->ln,
      lex->col
    );
    return false;
  }
  next_char(lex);
  int code = 0;
  while (current_char(lex) != '}')
  {
    if (!isxdigit(current_char(lex))) {
      rak_error_set(
        err,
        "expecting only hexadecimal numbers inside '{}' at %d,%d", 
        lex->ln,
        lex->col
      );
      return false;
    }
    code = code * 16 + hex2bin(current_char(lex));
    next_char(lex);
  }
  next_char(lex);
  return insertUnicodeChar(lex, text, code, err);
}

static bool handle_escape_sequence(RakLexer *lex, RakString *text, RakError *err)
{
  switch (current_char(lex)) {
  case 'n':
    rak_string_inplace_append_cstr(text, 1, "\n", err);
    break;
  case 'r':
    rak_string_inplace_append_cstr(text, 1, "\r", err);
    break;
  case 't':
    rak_string_inplace_append_cstr(text, 1, "\t", err);
    break;
  case '"':
    rak_string_inplace_append_cstr(text, 1, "\"", err);
    break;
  case '\\':
    rak_string_inplace_append_cstr(text, 1, "\\", err);
    break;
  case '0':
    rak_string_inplace_append_cstr(text, 1, "\0", err);
    break;
  case 'x':
    next_char(lex);
    return handle_hex_escape(lex, text, err);
  case 'u':
    next_char(lex);
    return handle_unicode_escape(lex, text, err);
  default:
    rak_error_set(
      err,
      "unknown escape sequence '\\%c' at %d,%d", 
      isprint(current_char(lex)) ? current_char(lex) : '?', 
      lex->ln,
      lex->col
    );
    return false;
  }
  next_char(lex);
  return rak_is_ok(err);
}

static bool parseString(RakLexer *lex, RakError *err)
{
  RakString *text = rak_string_new(err);
  if (!rak_is_ok(err)) return false;
  while (true)
  {
    switch (current_char(lex))
    {
    case '\0':
    case '\n':
      rak_error_set(err, "unterminated string at %d,%d", lex->ln, lex->col);
      return false;
    case '\"':
      lex->tok = token(lex, RAK_TOKEN_KIND_STRING, rak_string_len(text), rak_string_chars(text));
      next_char(lex);
      return true;
    case '\\':
      next_char(lex);
      if (!handle_escape_sequence(lex, text, err)) return false;
      continue;
    default:
      rak_string_inplace_append_cstr(text, 1, &current_char(lex), err);
      if (!rak_is_ok(err)) return false;
      next_char(lex);
      continue;
    }
  }
}

static inline bool match_string(RakLexer *lex, RakError *err)
{
  if (current_char(lex) != '\"') return false;
  next_char(lex);
  return parseString(lex, err);
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
  case RAK_TOKEN_KIND_EOF:         cstr = "eof";        break;
  case RAK_TOKEN_KIND_COMMA:       cstr = "','";        break;
  case RAK_TOKEN_KIND_COLON:       cstr = "':'";        break;
  case RAK_TOKEN_KIND_SEMICOLON:   cstr = "';'";        break;
  case RAK_TOKEN_KIND_LPAREN:      cstr = "'('";        break;
  case RAK_TOKEN_KIND_RPAREN:      cstr = "')'";        break;
  case RAK_TOKEN_KIND_LBRACKET:    cstr = "'['";        break;
  case RAK_TOKEN_KIND_RBRACKET:    cstr = "']'";        break;
  case RAK_TOKEN_KIND_LBRACE:      cstr = "'{'";        break;
  case RAK_TOKEN_KIND_RBRACE:      cstr = "'}'";        break;
  case RAK_TOKEN_KIND_PIPEPIPE:    cstr = "'||'";       break;
  case RAK_TOKEN_KIND_AMPAMP:      cstr = "'&&'";       break;
  case RAK_TOKEN_KIND_AMP:         cstr = "'&'";        break;
  case RAK_TOKEN_KIND_EQEQ:        cstr = "'=='";       break;
  case RAK_TOKEN_KIND_EQ:          cstr = "'='";        break;
  case RAK_TOKEN_KIND_BANGEQ:      cstr = "'!='";       break;
  case RAK_TOKEN_KIND_BANG:        cstr = "'!'";        break;
  case RAK_TOKEN_KIND_GTEQ:        cstr = "'>='";       break;
  case RAK_TOKEN_KIND_GT:          cstr = "'>'";        break;
  case RAK_TOKEN_KIND_LTEQ:        cstr = "'<='";       break;
  case RAK_TOKEN_KIND_LT:          cstr = "'<'";        break;
  case RAK_TOKEN_KIND_DOTDOT:      cstr = "'..'";       break;
  case RAK_TOKEN_KIND_DOT:         cstr = "'.'";        break;
  case RAK_TOKEN_KIND_PLUSEQ:      cstr = "'+='";       break;
  case RAK_TOKEN_KIND_PLUS:        cstr = "'+'";        break;
  case RAK_TOKEN_KIND_MINUSEQ:     cstr = "'-='";       break;
  case RAK_TOKEN_KIND_MINUS:       cstr = "'-'";        break;
  case RAK_TOKEN_KIND_STAREQ:      cstr = "'*='";       break;
  case RAK_TOKEN_KIND_STAR:        cstr = "'*'";        break;
  case RAK_TOKEN_KIND_SLASHEQ:     cstr = "'/='";       break;
  case RAK_TOKEN_KIND_SLASH:       cstr = "'/'";        break;
  case RAK_TOKEN_KIND_PERCENTEQ:   cstr = "'%='";       break;
  case RAK_TOKEN_KIND_PERCENT:     cstr = "'%'";        break;
  case RAK_TOKEN_KIND_NUMBER:      cstr = "number";     break;
  case RAK_TOKEN_KIND_STRING:      cstr = "string";     break;
  case RAK_TOKEN_KIND_BREAK_KW:    cstr = "break";      break;
  case RAK_TOKEN_KIND_CONTINUE_KW: cstr = "continue";   break;
  case RAK_TOKEN_KIND_DO_KW:       cstr = "do";         break;
  case RAK_TOKEN_KIND_ELSE_KW:     cstr = "else";       break;
  case RAK_TOKEN_KIND_FALSE_KW:    cstr = "false";      break;
  case RAK_TOKEN_KIND_FN_KW:       cstr = "fn";         break;
  case RAK_TOKEN_KIND_IF_KW:       cstr = "if";         break;
  case RAK_TOKEN_KIND_INOUT_KW:    cstr = "inout";      break;
  case RAK_TOKEN_KIND_LET_KW:      cstr = "let";        break;
  case RAK_TOKEN_KIND_LOOP_KW:     cstr = "loop";       break;
  case RAK_TOKEN_KIND_NIL_KW:      cstr = "nil";        break;
  case RAK_TOKEN_KIND_RETURN_KW:   cstr = "return";     break;
  case RAK_TOKEN_KIND_TRUE_KW:     cstr = "true";       break;
  case RAK_TOKEN_KIND_WHILE_KW:    cstr = "while";      break;
  case RAK_TOKEN_KIND_YIELD_KW:    cstr = "yield";      break;
  case RAK_TOKEN_KIND_IDENT:       cstr = "identifier"; break;
  }
  return cstr;
}

void rak_lexer_init(RakLexer *lex, RakString *file, RakString *source, RakError *err)
{
  lex->file = file;
  rak_object_retain(&file->obj);
  lex->source = source;
  rak_object_retain(&source->obj);
  lex->curr = rak_string_chars(lex->source);
  lex->ln = 1;
  lex->col = 1;
  rak_lexer_next(lex, err);
}

void rak_lexer_deinit(RakLexer *lex)
{
  rak_string_release(lex->file);
  rak_string_release(lex->source);
}

void rak_lexer_next(RakLexer *lex, RakError *err)
{
  skip_whitespace_comments(lex);
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
  if (match_chars(lex, "+=", RAK_TOKEN_KIND_PLUSEQ)) return;
  if (match_char(lex, '+', RAK_TOKEN_KIND_PLUS)) return;
  if (match_chars(lex, "-=", RAK_TOKEN_KIND_MINUSEQ)) return;
  if (match_char(lex, '-', RAK_TOKEN_KIND_MINUS)) return;
  if (match_chars(lex, "*=", RAK_TOKEN_KIND_STAREQ)) return;
  if (match_char(lex, '*', RAK_TOKEN_KIND_STAR)) return;
  if (match_chars(lex, "/=", RAK_TOKEN_KIND_SLASHEQ)) return;
  if (match_char(lex, '/', RAK_TOKEN_KIND_SLASH)) return;
  if (match_chars(lex, "%=", RAK_TOKEN_KIND_PERCENTEQ)) return;
  if (match_char(lex, '%', RAK_TOKEN_KIND_PERCENT)) return;
  if (match_number(lex, err) || !rak_is_ok(err)) return;
  if (match_string(lex, err) || !rak_is_ok(err)) return;
  if (match_keyword(lex, "break", RAK_TOKEN_KIND_BREAK_KW)) return;
  if (match_keyword(lex, "continue", RAK_TOKEN_KIND_CONTINUE_KW)) return;
  if (match_keyword(lex, "do", RAK_TOKEN_KIND_DO_KW)) return;
  if (match_keyword(lex, "else", RAK_TOKEN_KIND_ELSE_KW)) return;
  if (match_keyword(lex, "false", RAK_TOKEN_KIND_FALSE_KW)) return;
  if (match_keyword(lex, "fn", RAK_TOKEN_KIND_FN_KW)) return;
  if (match_keyword(lex, "if", RAK_TOKEN_KIND_IF_KW)) return;
  if (match_keyword(lex, "inout", RAK_TOKEN_KIND_INOUT_KW)) return;
  if (match_keyword(lex, "let", RAK_TOKEN_KIND_LET_KW)) return;
  if (match_keyword(lex, "loop", RAK_TOKEN_KIND_LOOP_KW)) return;
  if (match_keyword(lex, "nil", RAK_TOKEN_KIND_NIL_KW)) return;
  if (match_keyword(lex, "return", RAK_TOKEN_KIND_RETURN_KW)) return;
  if (match_keyword(lex, "true", RAK_TOKEN_KIND_TRUE_KW)) return;
  if (match_keyword(lex, "while", RAK_TOKEN_KIND_WHILE_KW)) return;
  if (match_keyword(lex, "yield", RAK_TOKEN_KIND_YIELD_KW)) return;
  if (match_ident(lex)) return;
  unexpected_character_error(err, current_char(lex), lex->ln, lex->col);
}
