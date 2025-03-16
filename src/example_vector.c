#undef _FORTIFY_SOURCE
#include "snifex-api.h"

void vector_usage() {
  //-
  //- Create Vector
  //-
#ifdef __GNUC__
  Vec int_vec = vec_create(uint16_t, 1);
#else
  Vec int_vec;
  vec_create(int_vec, uint16_t, 1);
#endif  //__GNUC__

  //-
  //- Pushing, indexing and popping from vector
  //-
  assert(int_vec.cap == 1);
  vec_push(uint16_t, &int_vec, 123);
  assert(int_vec.cap == 1);
  vec_push(uint16_t, &int_vec, 456);
  assert(int_vec.cap == 4);  // capacity gets doubled after pushing

#ifdef __GNUC__
  uint16_t indexedElem = *vec_idx(int16_t, int_vec, int_vec.len - 1);
  uint16_t lastElem = *vec_last(int16_t, int_vec);
#else
  uint16_t* ptr;
  vec_idx(ptr, int_vec, int_vec.len - 1);
  uint16_t indexedElem = *ptr;
  vec_last(ptr, int_vec);
  uint16_t lastElem = *ptr;
#endif
  vec_pop(uint16_t, &int_vec);
  vec_pop(uint16_t, &int_vec);

  assert(int_vec.len == 0);
  assert(lastElem == 456 && lastElem == indexedElem);

#ifdef __GNUC__
  Vec int_vec_front = vec_from(uint16_t, 1, 2, 3);
  Vec int_vec_back = vec_from(uint16_t, 4, 5, 6);
#else
  Vec int_vec_front;
  vec_from(int_vec_front, uint16_t, 1, 2, 3);
  Vec int_vec_back;
  vec_from(int_vec_back, uint16_t, 4, 5, 6);
#endif  //__GNUC__
  vec_append(uint16_t, &int_vec_front, int_vec_back);
  assert(int_vec_front.len == 6);
  assert(*vec_idx(uint16_t, int_vec_front, 0) == 1);
  assert(*vec_idx(uint16_t, int_vec_front, 1) == 2);
  assert(*vec_idx(uint16_t, int_vec_front, 2) == 3);
  assert(*vec_idx(uint16_t, int_vec_front, 3) == 4);
  assert(*vec_idx(uint16_t, int_vec_front, 4) == 5);
  assert(*vec_idx(uint16_t, int_vec_front, 5) == 6);

  //-
  //- Freeing the vector
  //-
  vec_free(&int_vec_front);
  vec_free(&int_vec_back);
  vec_free(&int_vec);
}

void vector_macro_init() {
#ifdef __GNUC__
  Vec vec = vec_from(uint16_t, 10, 20, 30);
  assert(*vec_idx(uint16_t, vec, 0) == 10);
  assert(*vec_idx(uint16_t, vec, 1) == 20);
  assert(*vec_idx(uint16_t, vec, 2) == 30);
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
