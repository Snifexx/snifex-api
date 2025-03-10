#include "snifex-api.h"

DeclareVec(int);

typedef struct Vec_int {
  int different_struct;
} my_Vec_int; // using 'Vec_int' would be an error!

void generic_vectors() {
  Vec(int) intVec = vec_create(1);
  my_Vec_int my_dynArr = {102};

  vec_push(&intVec, 10);

  assert(*vec_idx(intVec, 0) == 10);
  assert(my_dynArr.different_struct == 102);

  vec_free(intVec);
}
