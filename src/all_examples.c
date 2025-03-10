#undef _FORTIFY_SOURCE

#include "./example_arena_and_strings.c"
#include "./example_vectors.c"

int main() {
  arena_and_strings();
  generic_vectors();
  return 0;
}
