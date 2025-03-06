#undef _FORTIFY_SOURCE

#include "snifex-api.h"
#include <stdio.h>

int main() {
  Arena arena = arena_create(10);
  string str = str_copy(&arena, strlit("    String to trim  "));
  str = str_trim(str);
  printf("|%.*s|\n", (int) str.len, str.ptr);
  arena_free(&arena);
}
