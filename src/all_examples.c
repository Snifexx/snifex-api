#undef _FORTIFY_SOURCE

#define SNIFEX_API_IMPLEMENTATION
#include "snifex-api.h"
#undef SNIFEX_API_IMPLEMENTATION

#include "hm.h"

DefineDict(uint64_t, float);

int main() {
  Dictionary(uint64_t, float) hm = dict_create(uint64_t, float);
  dict_put(&hm, 4, 420.0, NULL);
  float a;
  dict_put(&hm, 4, 69.0, &a);

  free(hm.buckets);
  vec_free(&hm.entries);
}
