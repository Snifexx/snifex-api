#include "snifex-api.h"

DeclareVec(uint16_t);

typedef struct Vec_uint16_t {
  int different_struct;
} my_Vec_uint16_t;  // using 'Vec_uint16_t' would give a
                    // redefinition_different_typedef' compiler error!

void demonstrate_unpolluted_symtable() {
  Vec(uint16_t) intVec = vec_create_uint16_t(sizeof(uint16_t));
  vec_push_uint16_t(&intVec, 13);

  my_Vec_uint16_t my_vec = {102};

#ifdef __GNUC__
  assert(*vec_idx(intVec, 0) == 12);
#else
  uint16_t* ptr;
  vec_idx(ptr, intVec, 0);
  assert(*ptr == 12);
#endif
  assert(my_vec.different_struct == 102);

  vec_free_uint16_t(&intVec);
}

void vector_usage() {
  //-
  //- Create Vector
  //-
  Vec(uint16_t) intVec = vec_create_uint16_t(1);

  //-
  //- Pushing, indexing and popping from vector
  //-
  assert(intVec.cap == 1);
  vec_push_uint16_t(&intVec, 123);
  assert(intVec.cap == 1);
  vec_push_uint16_t(&intVec, 456);
  assert(intVec.cap == 4);  // capacity gets doubled after pushing

#ifdef __GNUC__
  uint16_t indexedElem = *vec_idx(intVec, intVec.len - 1);
  uint16_t lastElem = *vec_last(intVec);
#else
  uint16_t* ptr;
  vec_idx(ptr, intVec, intVec.len - 1);
  uint16_t indexedElem = *ptr;
  vec_last(ptr, intVec);
  uint16_t lastElem = *ptr;
#endif
  vec_pop_uint16_t(&intVec);
  vec_pop_uint16_t(&intVec);

  assert(intVec.len == 0);
  assert(lastElem == 456 && lastElem == indexedElem);

  //-
  //- Freeing the vector
  //-
  vec_free_uint16_t(&intVec);
}

void vector_macro_init() {
#ifdef __GNUC__
  Vec(uint16_t) vec = vec_from(uint16_t, 10, 20, 30);
  assert(*vec_idx(vec, 0) == 10);
  assert(*vec_idx(vec, 1) == 20);
  assert(*vec_idx(vec, 2) == 30);
#else
  Vec(uint16_t) vec;
  vec_from(vec, uint16_t, 10, 20, 30);
  uint16_t* ptr;
  vec_idx(ptr, vec, 0);
  assert(*ptr == 10);
  vec_idx(ptr, vec, 1);
  assert(*ptr == 20);
  vec_idx(ptr, vec, 2);
  assert(*ptr == 30);
#endif
}
