#include "snifex-api.h"
#include <assert.h>
#include <stdint.h>

DeclareVec(uint16_t);

typedef struct Vec_uint16_t {
  int different_struct;
} my_Vec_uint16_t; // using 'Vec_uint16_t' would give a redefinition_different_typedef' compiler error!


void demonstrate_unpolluted_symtable() {
  Vec(uint16_t) intVec = vec_create(10);
  vec_push(&intVec, 10);

  my_Vec_uint16_t my_dynArr = {102};

  assert(*vec_idx(intVec, 0) == 10);
  assert(my_dynArr.different_struct == 102);

  vec_free(&intVec);
}

void vector_usage() {
  Vec(uint16_t) intVec = vec_create(1);

  assert(intVec.cap == 1);
  vec_push(&intVec, 123);
  assert(intVec.cap == 1);
  vec_push(&intVec, 456);
  assert(intVec.cap == 2); // capacity gets doubled

  int lastElem = vec_last(intVec);
  vec_pop(&intVec);
  vec_pop(&intVec);

  assert(intVec.len == 0);
  assert(lastElem == 456);

  vec_free(&intVec);
}
