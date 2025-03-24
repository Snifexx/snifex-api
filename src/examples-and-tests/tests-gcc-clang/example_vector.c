#include "../snifex-api.h"

DefineVec(uint16_t);

void vector_usage() {
  //-
  //- Create Vector
  //-
  Vec(uint16_t) int_vec = vec_create(uint16_t, 1);

  //-
  //- Pushing, indexing and popping from vector
  //-
  assert(int_vec.cap == 1);
  vec_push(&int_vec, 123);
  assert(int_vec.cap == 1);
  vec_push(&int_vec, 456);
  assert(int_vec.cap == 4);  // capacity gets doubled after pushing

  uint16_t indexedElem = *vec_idx(int_vec, int_vec.len - 1);
  uint16_t lastElem = *vec_last(int_vec);
  vec_pop(&int_vec);
  vec_pop(&int_vec);

  assert(int_vec.len == 0);
  assert(lastElem == 456 && lastElem == indexedElem);

  Vec(uint16_t) int_vec_front = vec_from(uint16_t, 1, 2, 3);
  Vec(uint16_t) int_vec_back = vec_from(uint16_t, 4, 5, 6);

  vec_append(&int_vec_front, int_vec_back);
  assert(int_vec_front.len == 6);
  assert(*vec_idx(int_vec_front, 0) == 1);
  assert(*vec_idx(int_vec_front, 1) == 2);
  assert(*vec_idx(int_vec_front, 2) == 3);
  assert(*vec_idx(int_vec_front, 3) == 4);
  assert(*vec_idx(int_vec_front, 4) == 5);
  assert(*vec_idx(int_vec_front, 5) == 6);

  //-
  //- Freeing the vector
  //-
  vec_free(&int_vec_front);
  vec_free(&int_vec_back);
  vec_free(&int_vec);
}
