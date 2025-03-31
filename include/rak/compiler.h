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

typedef struct
{
  RakChunk chunk;
  RakLexer lex;
} RakCompiler;

void rak_compiler_init(RakCompiler *comp, RakError *err);
void rak_compiler_deinit(RakCompiler *comp);
void rak_compiler_compile_chunk(RakCompiler *comp, char *source, RakError *err);
void rak_compiler_eompile_expr(RakCompiler *comp, char *source, RakError *err);

#endif // RAK_COMPILER_H
