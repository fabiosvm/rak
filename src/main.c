//
// main.c
//
// Copyright 2025 Fábio de Souza Villaça Medeiros
//
// This file is part of the Rak Project.
// For detailed license information, please refer to the LICENSE file
// located in the root directory of this project.
//

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rak.h>

#define SOURCE_MAX_LEN ((int) 1 << 12)

static void shutdown(int sig);
static void check_error(RakError *err);
static void read(RakString *source);
static void compile(RakCompiler *comp, RakString *source, RakError *err);
static void run(RakVM *vm, RakChunk *chunk, RakError *err);

static void shutdown(int sig)
{
  (void) sig;
  printf("\n");
  exit(EXIT_SUCCESS);
}

static void check_error(RakError *err)
{
  if (rak_is_ok(err)) return;
  rak_error_print(err);
  exit(EXIT_FAILURE);
}

static void read(RakString *source)
{
  RakError err = rak_ok();
  rak_string_init_with_capacity(source, SOURCE_MAX_LEN, &err);
  check_error(&err);
  char c = (char) fgetc(stdin);
  while (c != EOF)
  {
    rak_string_inplace_append_cstr(source, 1, &c, &err);
    check_error(&err);
    c = (char) fgetc(stdin);
  }
  rak_string_inplace_append_cstr(source, 1, "\0", &err);
  check_error(&err);
}

static void compile(RakCompiler *comp, RakString *source, RakError *err)
{
  rak_compiler_init(comp, err);
  check_error(err);
  rak_compiler_compile(comp, rak_string_chars(source), err);
  check_error(err);
}

static void run(RakVM *vm, RakChunk *chunk, RakError *err)
{
  rak_vm_init(vm, RAK_VM_VSTK_DEFAULT_SIZE, err);
  check_error(err);
  rak_builtin_load_globals(vm, err);
  check_error(err);
  rak_vm_run(vm, chunk, err);
  check_error(err);
}

int main(int argc, const char *argv[])
{
  signal(SIGINT, shutdown);
  RakError err = rak_ok();
  RakString source;
  read(&source);
  RakCompiler comp;
  compile(&comp, &source, &err);
  check_error(&err);
  if (argc > 1 && !strcmp(argv[1], "-c"))
  {
    rak_dump_chunk(&comp.chunk);
    goto end;
  }
  RakVM vm;
  run(&vm, &comp.chunk, &err);
  check_error(&err);
  rak_vm_deinit(&vm);
end:
  rak_compiler_deinit(&comp);
  rak_string_deinit(&source);
  return EXIT_SUCCESS;
}
