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
static RakString *read_from_stdin(RakError *err);
static RakString *read_from_file(const char *path, RakError *err);
static FILE *open_file(const char *path, RakError *err);
static int file_size(FILE *fp);
static RakFunction *compile_from_stdin(RakError *err);
static RakFunction *compile_from_file(const char *path, RakError *err);

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

static RakString *read_from_stdin(RakError *err)
{
  rak_error_init(err);
  RakString *source = rak_string_new(err);
  if (!rak_is_ok(err)) return NULL;
  char c = (char) fgetc(stdin);
  while (c != EOF)
  {
    rak_string_inplace_append_cstr(source, 1, &c, err);
    if (!rak_is_ok(err))
    {
      rak_string_free(source);
      return NULL;
    }
    c = (char) fgetc(stdin);
  }
  rak_string_inplace_append_cstr(source, 1, "\0", err);
  if (rak_is_ok(err)) return source;
  rak_string_free(source);
  return NULL;
}

static RakString *read_from_file(const char *path, RakError *err)
{
  FILE *fp = open_file(path, err);
  if (!rak_is_ok(err)) return NULL;
  int len = file_size(fp);
  RakString *source = rak_string_new_with_capacity(len, err);
  if (!rak_is_ok(err))
  {
    fclose(fp);
    return NULL;
  }
  int _len = (int) fread(rak_string_chars(source), 1, len, fp);
  if (_len != len)
  {
    rak_error_set(err, "cannot read file %s", path);
    fclose(fp);
    rak_string_free(source);
    return NULL;
  }
  fclose(fp);
  source->slice.len = len;
  rak_string_inplace_append_cstr(source, 1, "\0", err);
  if (rak_is_ok(err)) return source;
  rak_string_free(source);
  return NULL;
}

static FILE *open_file(const char *path, RakError *err)
{
  FILE *fp = NULL;
#ifdef _WIN32
  fopen_s(&fp, path, "rb");
#else
  fp = fopen(path, "r");
#endif
  if (!fp)
    rak_error_set(err, "cannot open file %s", path);
  return fp;
}

static int file_size(FILE *fp)
{
  fseek(fp, 0, SEEK_END);
  int size = (int) ftell(fp);
  fseek(fp, 0, SEEK_SET);
  return size;
}

static RakFunction *compile_from_stdin(RakError *err)
{
  RakString *file = rak_string_new_from_cstr(-1, "<stdin>", err);
  if (!rak_is_ok(err)) return NULL;
  RakString *source = read_from_stdin(err);
  if (!rak_is_ok(err))
  {
    rak_string_free(file);
    return NULL;
  }
  return rak_compile(file, source, err);
}

static RakFunction *compile_from_file(const char *path, RakError *err)
{
  RakString *file = rak_string_new_from_cstr(-1, path, err);
  if (!rak_is_ok(err)) return NULL;
  RakString *source = read_from_file(path, err);
  if (!rak_is_ok(err))
  {
    rak_string_free(file);
    return NULL;
  }
  return rak_compile(file, source, err);
}

int main(int argc, const char *argv[])
{
  signal(SIGINT, shutdown);
  RakError err;
  rak_error_init(&err);
  const char *path = get_arg(argc, argv, 0);
  RakFunction *fn;
  if (path)
    fn = compile_from_file(path, &err);
  else
    fn = compile_from_stdin(&err);
  if (!rak_is_ok(&err))
  {
    rak_error_print(&err);
    return EXIT_FAILURE;
  }
  if (has_opt(argc, argv, "-c"))
  {
    rak_dump_function(fn);
    rak_function_free(fn);
    return EXIT_SUCCESS;
  }
  RakVM vm;
  rak_vm_init(&vm, RAK_VM_VSTK_DEFAULT_SIZE, RAK_VM_CSTK_DEFAULT_SIZE, &err);
  if (!rak_is_ok(&err))
  {
    rak_error_print(&err);
    rak_function_free(fn);
    return EXIT_FAILURE;
  }
  rak_builtin_load_globals(&vm, &err);
  if (!rak_is_ok(&err))
  {
    rak_error_print(&err);
    rak_function_free(fn);
    rak_vm_deinit(&vm);
    return EXIT_FAILURE;
  }
  rak_vm_run(&vm, fn, &err);
  if (!rak_is_ok(&err))
  {
    rak_error_print(&err);
    rak_vm_deinit(&vm);
    return EXIT_FAILURE;
  }
  rak_vm_deinit(&vm);
  return EXIT_SUCCESS;
}
