#include "snifex-api.h"
#include <assert.h>
#include <stdint.h>

DeclareVec(uint16_t);

typedef struct Vec_uint16_t {
  int different_struct;
} my_Vec_uint16_t; // using 'Vec_uint16_t' would give a
                   // redefinition_different_typedef' compiler error!

void demonstrate_unpolluted_symtable() {
  Vec(uint16_t) intVec = vec_create(sizeof(uint16_t));
  vec_push(&intVec, 12);

  my_Vec_uint16_t my_dynArr = {102};

  assert(*vec_idx(intVec, 0) == 12);
  assert(my_dynArr.different_struct == 102);

  vec_free(&intVec);
}

void vector_usage() {
  //-
  //- Create Vector
  //-
  Vec(uint16_t) intVec = vec_create(1);

  //-
  //- Pushing, indexing and popping from vector
  //-
  assert(intVec.cap == 1);
  vec_push(&intVec, 123);
  assert(intVec.cap == 1);
  vec_push(&intVec, 456);
  assert(intVec.cap == 4); // capacity gets doubled after pushing

  uint16_t indexedElem = *vec_idx(intVec, intVec.len - 1);
  uint16_t lastElem = *vec_last(intVec);
  vec_pop(&intVec);
  vec_pop(&intVec);

  assert(intVec.len == 0);
  assert(lastElem == 456 && lastElem == indexedElem);

  //-
  //- Freeing the vector
  //-
  vec_free(&intVec);
}
