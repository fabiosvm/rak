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

static void shutdown(int sig);
static bool has_opt(int argc, const char *argv[], const char *opt);
static const char *get_arg(int argc, const char *argv[], int idx);
static void check_error(RakError *err);
static void read_from_stdin(RakString *source);
static void read_from_file(RakString *source, const char *filename, RakError *err);
static FILE *open_file(const char *filename, RakError *err);
static int file_size(FILE *fp);
static void run(RakVM *vm, RakFunction *fn, RakError *err);

static void shutdown(int sig)
{
  (void) sig;
  printf("\n");
  exit(EXIT_SUCCESS);
}

static bool has_opt(int argc, const char *argv[], const char *opt)
{
  for (int i = 1; i < argc; ++i)
    if (!strcmp(argv[i], opt))
      return true;
  return false;
}

static const char *get_arg(int argc, const char *argv[], int idx)
{
  int j = 0;
  for (int i = 1; i < argc; ++i)
  {
    if (argv[i][0] == '-') continue;
    if (j == idx) return argv[i];
    ++j;
  }
  return NULL;
}

static void check_error(RakError *err)
{
  if (rak_is_ok(err)) return;
  rak_error_print(err);
  exit(EXIT_FAILURE);
}

static void read_from_stdin(RakString *source)
{
  RakError err = rak_ok();
  rak_string_init(source, &err);
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

static void read_from_file(RakString *source, const char *filename, RakError *err)
{
  FILE *fp = open_file(filename, err);
  check_error(err);
  int len = file_size(fp);
  rak_string_init_with_capacity(source, len, err);
  check_error(err);
  int _len = (int) fread(rak_string_chars(source), 1, len, fp);
  if (_len != len)
  {
    rak_error_set(err, "cannot read file %s", filename);
    fclose(fp);
    return;
  }
  source->slice.len = len;
  rak_string_inplace_append_cstr(source, 1, "\0", err);
  check_error(err);
  fclose(fp);
}

static FILE *open_file(const char *filename, RakError *err)
{
  FILE *fp = NULL;
#ifdef _WIN32
  fopen_s(&fp, filename, "rb");
#else
  fp = fopen(filename, "r");
#endif
  if (!fp)
    rak_error_set(err, "cannot open file %s", filename);
  return fp;
}

static int file_size(FILE *fp)
{
  fseek(fp, 0, SEEK_END);
  int size = (int) ftell(fp);
  fseek(fp, 0, SEEK_SET);
  return size;
}

static void run(RakVM *vm, RakFunction *fn, RakError *err)
{
  rak_vm_init(vm, RAK_VM_VSTK_DEFAULT_SIZE, RAK_VM_CSTK_DEFAULT_SIZE, err);
  check_error(err);
  rak_builtin_load_globals(vm, err);
  check_error(err);
  rak_vm_run(vm, fn, err);
  check_error(err);
}

int main(int argc, const char *argv[])
{
  signal(SIGINT, shutdown);
  RakError err = rak_ok();
  RakString source;
  const char *filename = get_arg(argc, argv, 0);
  if (filename)
  {
    read_from_file(&source, filename, &err);
    check_error(&err);
  }
  else
    read_from_stdin(&source);
  RakFunction *fn = rak_compile(rak_string_chars(&source), &err);
  check_error(&err);
  if (has_opt(argc, argv, "-c"))
  {
    rak_dump_chunk(&fn->chunk);
    rak_string_deinit(&source);
    return EXIT_SUCCESS;
  }
  RakVM vm;
  run(&vm, fn, &err);
  check_error(&err);
  rak_vm_deinit(&vm);
  rak_string_deinit(&source);
  return EXIT_SUCCESS;
}
