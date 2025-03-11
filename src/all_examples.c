#undef _FORTIFY_SOURCE

#include "./example_arena.c"
#include "./example_string.c"
#include "./example_vector.c"

int main() {
  arena_usage();
  string_usage();
  demonstrate_unpolluted_symtable();
  vector_usage();
  return 0;
}
