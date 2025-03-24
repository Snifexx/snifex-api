#include "stdio.h"

void dyn_arena_usage();
void arena_usage();
void string_usage();
void vector_usage();
void dict_usage();
void dict_custom_hashing();

#define SNIFEX_API_IMPLEMENTATION
#include "../snifex-api.h"
#undef SNIFEX_API_IMPLEMENTATION

int main() {
  dyn_arena_usage();
  arena_usage();
  string_usage();
  vector_usage();
  dict_usage();
  dict_custom_hashing();

  printf("\n\33[4;32mAll Tests passed!\33[0m\n");
}
