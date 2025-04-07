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
#include <stdlib.h>
#include <string.h>
#include <rak.h>

#define SOURCE_MAX_LEN ((int) 1 << 12)

static bool dump = false;
static RakError err;
static RakString source;
static RakCompiler comp;
static RakVM vm;

static inline void check_error(void);
static inline void init(void);
static inline void deinit(void);
static inline void repl(void);
static inline bool read(void);
static inline void eval(void);
static void shutdown(int sig);

static inline void check_error(void)
{
  if (rak_is_ok(&err)) return;
  rak_error_print(&err);
  exit(EXIT_FAILURE);
}

static inline void init(void)
{
  err = rak_ok();
  rak_string_init_with_capacity(&source, SOURCE_MAX_LEN, &err);
  check_error();
  rak_compiler_init(&comp, &err);
  check_error();
  rak_vm_init(&vm, RAK_VM_VSTK_DEFAULT_SIZE, &err);
  check_error();
  signal(SIGINT, shutdown);
}

static inline void deinit(void)
{
  rak_string_deinit(&source);
  rak_compiler_deinit(&comp);
  rak_vm_deinit(&vm);
}

static inline void repl(void)
{
  for (;;)
  {
    printf("> ");
    if (!read()) break;
    if (source.slice.len == 1) continue;
    eval();
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

static inline void eval(void)
{
  err = rak_ok();
  rak_compiler_compile_chunk(&comp, rak_string_chars(&source), &err);
  if (!rak_is_ok(&err))
  {
    rak_error_print(&err);
    return;
  }
  if (dump)
    rak_dump_chunk(&comp.chunk);
  rak_vm_run(&vm, &comp.chunk, &err);
  if (rak_is_ok(&err)) return;
  rak_error_print(&err);
}

static void shutdown(int sig)
{
  (void) sig;
  printf("\n");
  deinit();
  exit(EXIT_SUCCESS);
}

int main(int argc, const char *argv[])
{
  if (argc > 1)
    dump = !strcmp(argv[1], "-d");
  init();
  repl();
  return EXIT_SUCCESS;
}
