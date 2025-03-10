#include "snifex-api.h"

// Arenas and strings

void arena_and_strings() {
  Arena arena = arena_create(10);
  string str = str_copy(&arena, strlit("    String to trim  "));
  str = str_trim(str);

  assert(str_eq(strlit("String to trim"), str));

  arena_free(&arena);
}


