#include "../snifex-api.h"

void dyn_arena_usage() {
  size_t fitting_size = sizeof(uint16_t) + sizeof(float);

  //-
  //- Creating arena
  //-
  DynArena arena1;
  dyn_arena_init(&arena1, fitting_size);
  // or:
  DynArena arena2 = dyn_arena_create(fitting_size);

  assert(arena1.cap == fitting_size && arena2.cap == fitting_size);

  //-
  //- Allocate object on arena and set it
  //-
  size_t my_int_obj =
      dyn_arena_alloc(&arena1, sizeof(uint16_t), sizeof(uint16_t));
  *dyn_arena_get(uint16_t, arena1, my_int_obj) = 10;

  size_t my_float_obj = dyn_arena_alloc(&arena1, sizeof(float), sizeof(float));
  *dyn_arena_get(float, arena1, my_float_obj) = 10;

  //-
  //- Reserving at least x bytes in capacity
  //-
  dyn_arena_reserve(&arena1, fitting_size + 10);

  assert(arena1.cap >= fitting_size + 10);

  //-
  //- Freeing the arenas
  //-
  dyn_arena_free(&arena1);
  dyn_arena_free(&arena2);
}

void arena_usage() {
  Arena arena = arena_create(4096);  // I arbitrarily chose 4KB

  // Everything is the same as a `DynArena` except you cannot change it's
  // capacity, hence why it's renamed size. When you allocate if it returns a
  // null pointer, then the allocation does not fit in the arena.
  void* successful_allocation = arena_alloc(&arena, 4000, 1);
  assert(successful_allocation != NULL);
  void* exceeding_allocation = arena_alloc(&arena, 97, 1);
  assert(exceeding_allocation == NULL);

  arena_free(&arena);
}
