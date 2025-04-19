//
// compiler.h
//
// Copyright 2025 Fábio de Souza Villaça Medeiros
//
// This file is part of the Rak Project.
// For detailed license information, please refer to the LICENSE file
// located in the root directory of this project.
//

#ifndef RAK_COMPILER_H
#define RAK_COMPILER_H

#include "chunk.h"
#include "lexer.h"
#include "string.h"

typedef struct
{
  int      len;
  char    *chars;
  uint8_t  idx;
  int      depth;
} RakSymbol;

typedef struct
{
  RakChunk            chunk;
  RakLexer            lex;
  RakSlice(RakSymbol) symbols;
  int                 scopeDepth;
} RakCompiler;

void rak_compiler_init(RakCompiler *comp, RakError *err);
void rak_compiler_deinit(RakCompiler *comp);
void rak_compiler_compile(RakCompiler *comp, char *source, RakError *err);

#endif // RAK_COMPILER_H
