
cmake_minimum_required(VERSION 3.15)

project(rak
  VERSION 0.1.0
  DESCRIPTION "The Rak Programming Language"
  LANGUAGES C)

set(CMAKE_C_STANDARD 11)

if(MSVC)
  add_compile_options(/W4 /WX)
else()
  add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

add_executable("${PROJECT_NAME}"
  "src/array.c"
  "src/builtin.c"
  "src/callable.c"
  "src/chunk.c"
  "src/closure.c"
  "src/compiler.c"
  "src/dump.c"
  "src/error.c"
  "src/fiber.c"
  "src/function.c"
  "src/lexer.c"
  "src/main.c"
  "src/memory.c"
  "src/native.c"
  "src/range.c"
  "src/record.c"
  "src/string.c"
  "src/value.c"
  "src/vm.c"
)

if(UNIX AND NOT APPLE)
  target_link_libraries("${PROJECT_NAME}" m)
endif()
