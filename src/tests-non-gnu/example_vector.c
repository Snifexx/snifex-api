#include "../snifex-api.h"
#include "bits/stdint-uintn.h"

DefineVec(uint16_t);

void vector_usage() {
  //-
  //- Create Vector
  //-
  Vec(uint16_t) int_vec;
  vec_create(int_vec, uint16_t, 1);

  //-
  //- Pushing, indexing and popping from vector
  //-
  assert(int_vec.cap == 1);
  vec_push(uint16_t, &int_vec, 123);
  assert(int_vec.cap == 1);
  vec_push(uint16_t, &int_vec, 456);
  assert(int_vec.cap == 4);  // capacity gets doubled after pushing

  uint16_t* indexed_elem_ptr;
  vec_idx(indexed_elem_ptr, uint16_t, int_vec, int_vec.len - 1);
  uint16_t indexed_elem = *indexed_elem_ptr;

  uint16_t* last_elem_ptr;
  vec_last(last_elem_ptr, uint16_t, int_vec);
  uint16_t last_elem = *last_elem_ptr;

  vec_pop(uint16_t, &int_vec);
  vec_pop(uint16_t, &int_vec);

  assert(int_vec.len == 0);
  assert(last_elem == 456 && last_elem == indexed_elem);

  Vec(uint16_t) int_vec_front;
  vec_from(int_vec_front, uint16_t, 1, 2, 3);
  Vec(uint16_t) int_vec_back;
  vec_from(int_vec_back, uint16_t, 4, 5, 6);

  vec_append(uint16_t, &int_vec_front, int_vec_back);
  assert(int_vec_front.len == 6);

  uint16_t to_check[6] = {1, 2, 3, 4, 5, 6};
  for (size_t i = 0; i < int_vec_front.len; i++) {
    uint16_t* x_ptr;
    vec_idx(x_ptr, uint16_t, int_vec_front, i);
    assert(*x_ptr == to_check[i]);
  }

  //-
  //- Freeing the vector
  //-
  vec_free(&int_vec_front);
  vec_free(&int_vec_back);
  vec_free(&int_vec);
}
