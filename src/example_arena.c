#include "snifex-api.h"
#include <assert.h>
#include <stdint.h>

void arena_usage() {
  size_t fitting_size = sizeof(uint16_t) + sizeof(float);

  //-
  //- Creating arena
  //-
  Arena arena1;
  arena_init(&arena1, fitting_size);
  // or macro:
  Arena arena2 = arena_create(fitting_size);

  // NOTE: For the time being there is no alignment on powers of 2,
  // will change in the future
  assert(arena1.cap == fitting_size && arena2.cap == fitting_size);

  //-
  //- Allocate object on arena
  //-
  uint16_t* my_int_obj = arena_alloc(&arena1, sizeof(uint16_t));
  *my_int_obj = 10;
  float* my_float_obj = arena_alloc(&arena1, sizeof(float));
  *my_float_obj = 10.0;

  //-
  //- Reserving at least x bytes in capacity
  //-
  arena_reserve(&arena1, fitting_size + 10);

  // Note: Again, for the time being, this equation is for always going to be equal.
  // In the future, when I implement alignment on power of 2, this inequality
  // being equal is not guaranteed, but it surely will have 'at least' 
  // `fitting_size + 10` bytes reserved, so the statement stil holds truth.
  assert(arena1.cap >= fitting_size + 10);


  //-
  //- Freeing the arenas
  //-
  arena_free(&arena1);
  arena_free(&arena2);
}

