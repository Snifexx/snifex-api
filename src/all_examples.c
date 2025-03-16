#undef _FORTIFY_SOURCE

void dyn_arena_usage();
void arena_usage();
void string_usage();
void vector_usage();

int main() {
  dyn_arena_usage();
  arena_usage();
  string_usage();
  vector_usage();
  return 0;
}
