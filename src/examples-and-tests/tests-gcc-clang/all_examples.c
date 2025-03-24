#include <stdint.h>
#include "stdio.h"

void dyn_arena_usage();
void arena_usage();
void string_usage();
void vector_usage();
void dict_usage();
void dict_custom_hashing();

#define SNIFEX_API_IMPLEMENTATION
#include "../../snifex-api.h"
#undef SNIFEX_API_IMPLEMENTATION

int main1() {
  dyn_arena_usage();
  arena_usage();
  string_usage();
  vector_usage();
  dict_usage();
  dict_custom_hashing();

  printf("\n\33[4;32mAll Tests passed!\33[0m\n");
  return 0;
}

DefineDict(uint8_t, float);

int main() {
  Dict(uint8_t, float) dict = dict_create(uint8_t, float);
  dict_put(&dict, 1, 1.0, NULL);
  dict_put(&dict, 2, 2.0, NULL);
  dict_put(&dict, 3, 3.0, NULL);
  dict_put(&dict, 4, 4.0, NULL);
  dict_put(&dict, 5, 5.0, NULL);
  dict_put(&dict, 6, 6.0, NULL);
  printf("Now Deletion\n");
  dict_del(&dict, 1);
  dict_del(&dict, 2);
  dict_del(&dict, 3);
  dict_del(&dict, 4);
  dict_del(&dict, 5);
  dict_del(&dict, 6);
  printf("Buckets and entries %zu, %zu\n", dict.b_len, dict.entries.len);
  dict_free(&dict);
  return 0;
}
