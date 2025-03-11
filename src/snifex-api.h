#ifndef SNIFEX_API_H
#define SNIFEX_API_H

#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//-
//- Macro utils
//-

#define _PRIV_TOKENPASTE(x, y) x##y
#define _PRIV_TOKENPASTE2(x, y) _PRIV_TOKENPASTE(x, y)
#define UNIQUE _PRIV_TOKENPASTE2(Unique_, __LINE__)

//-
//-  Numbers
//-

// I use macros because I want these to be the same for all number types
// without having 10 different implementation for each number type I use
#define min(t, a, b)                                                           \
  ({                                                                           \
    t min0 = a;                                                                \
    t min1 = b;                                                                \
    (min0 < min1 ? min0 : min1);                                               \
  })

#define max(t, a, b)                                                           \
  ({                                                                           \
    t max0 = a;                                                                \
    t max1 = b;                                                                \
    (max0 > max1 ? max0 : max1);                                               \
  })

#define sign(t, a)                                                             \
  ({                                                                           \
    t num = a;                                                                 \
    (num > 0 ? 1 : ((num < 0) ? -1 : 0));                                      \
  })

static inline bool is_power_of_two(const size_t x) {
  return (x & (x - 1)) == 0;
}

uint32_t ceil_powtwo(uint32_t v) {
  v--;
  v |= v >> 1;
  v |= v >> 2;
  v |= v >> 4;
  v |= v >> 8;
  v |= v >> 16;
  return v + 1;
}

//-
//- Arenas
//-

typedef struct dyn_arena {
  char *buf;
  size_t cap;
  size_t top;
} DynArena;

typedef struct arena {
  char *buf;
  size_t size;
  size_t top;
} Arena;

void dyn_arena_init(DynArena *const dyn_arena, const size_t init_cap) {
  dyn_arena->buf = (char *)malloc(init_cap);
  dyn_arena->cap = init_cap;
  dyn_arena->top = 0;
  assert(dyn_arena->buf != NULL);
}

void arena_init(Arena *const arena, const size_t size) {
  arena->buf = (char *)malloc(size);
  arena->size = size;
  arena->top = 0;
  assert(arena->buf != NULL);
}

DynArena dyn_arena_create(const size_t init_cap) {
  DynArena dyn_arena = {0};
  dyn_arena_init(&dyn_arena, init_cap);
  return dyn_arena;
}

Arena arena_create(const size_t init_cap) {
  Arena arena = {0};
  arena_init(&arena, init_cap);
  return arena;
}

size_t dyn_arena_alloc(DynArena *const dyn_arena, const size_t size,
                       const size_t alignment) {
  assert(!(size == 0 || alignment == 0 ||
           (alignment != 1 && !is_power_of_two(alignment)) ||
           size % alignment != 0));

  size_t top = dyn_arena->top;
  size_t padding = (top + alignment - 1) & ~(alignment - 1);

  if (top + padding + size > dyn_arena->cap) {
    dyn_arena->cap += top + padding + size;
    dyn_arena->cap *= 2;
    dyn_arena->buf = (char *)realloc(dyn_arena->buf, dyn_arena->cap);
    assert(dyn_arena->buf != NULL);
  }

  dyn_arena->top += size + padding;
  return top;
}

void *arena_alloc(Arena *const arena, const size_t size,
                  const size_t alignment) {
  if (size == 0) {
    return NULL;
  }
  assert(!(alignment == 0 || (alignment != 1 && !is_power_of_two(alignment)) ||
           size % alignment != 0));

  size_t top = arena->top;
  size_t padding = (top + alignment - 1) & ~(alignment - 1);

  if (top + padding + size > arena->size) {
    return NULL;
  }

  void *ret = &arena->buf[top];
  arena->top += size + padding;
  return ret;
}

#define dyn_arena_idx(t, dyn_arena, idx)                                       \
  ({                                                                           \
    const DynArena macro_dyn_arena = dyn_arena;                                \
    const size_t macro_idx = idx;                                              \
    (t *)&macro_dyn_arena.buf[macro_idx];                                      \
  })

void dyn_arena_reserve(DynArena *const dyn_arena, const size_t min_cap) {
  if (dyn_arena->cap < min_cap) {
    dyn_arena->cap *= min_cap;
    dyn_arena->buf = (char *)realloc(dyn_arena->buf, dyn_arena->cap);
    assert(dyn_arena->buf != NULL);
  }
}

void dyn_arena_free(DynArena *const dyn_arena) { free(dyn_arena->buf); }
void arena_free(Arena *const arena) { free(arena->buf); }

//-
//-  General type dynamic arrays:
//-

// Data structure that owns its own data (that is they just have to be freed).
// They basically are an alternative `Arena`. In other words... my
// implementation of a dynamically-growing array, I.E. ArrayList, Vector or
// however you might call it.
//
// The way I decided to have generic like feeling is by a `DeclareVec` macro
// that takes in the type and declares the struct-vec with the specified
// type `t`. I decided not to pollute the symbol table so I just used typedef
// on an anonymous struct, meaning you can create your struct named Array_{t}
// and make your own alias with typedef (that must be different from Array_{t}
// though).

#define Vec(t) Vec_##t

#define vec_idx(vec, i)                                                        \
  ({                                                                           \
    const size_t macro_i = i;                                                  \
    assert(vec.len > 0 && macro_i < vec.len && vec.ptr != NULL);               \
    &vec.ptr[macro_i];                                                         \
  })

#define DeclareVec(t)                                                          \
  typedef struct {                                                             \
    t *ptr;                                                                    \
    size_t cap;                                                                \
    size_t len;                                                                \
  } Vec(t);                                                                    \
                                                                               \
  Vec(t) vec_create_##t(const size_t init_cap) {                               \
    assert(init_cap > 0);                                                      \
                                                                               \
    return (Vec(t)){                                                           \
        .ptr = (t *)malloc(init_cap * sizeof(t)),                              \
        .cap = init_cap,                                                       \
        .len = 0,                                                              \
    };                                                                         \
  }                                                                            \
                                                                               \
  void vec_push_##t(Vec(t) *const vec, const t val) {                          \
    assert(vec != NULL);                                                       \
    if (vec->len + 1 > vec->cap) {                                             \
      vec->cap += 1;                                                           \
      vec->cap *= 2;                                                           \
      vec->ptr = (t *)realloc(vec->ptr, vec->cap * sizeof(t));                 \
      assert(vec->ptr != NULL);                                                \
    }                                                                          \
    vec->ptr[vec->len] = val;                                                  \
    vec->len += 1;                                                             \
  }                                                                            \
                                                                               \
  void vec_free_##t(Vec(t) *const vec) { free(vec->ptr); }                     \
                                                                               \
  t *const vec_last_##t(Vec(t) vec) { return vec_idx(vec, vec.len - 1); }      \
                                                                               \
  void vec_pop_##t(Vec(t) * vec) {                                             \
    assert(vec != NULL && vec->len > 0);                                       \
    vec->len -= 1;                                                             \
  }

//-
//-  Strings
//-

typedef struct string {
  char *ptr;
  size_t len;
} string;

DeclareVec(string);

string strlit(char const *s) {
  return (string){.ptr = (char *)s, .len = strlen(s)};
}
bool str_is_empty(const string str) {
  return (str.ptr == NULL || str.len == 0);
}

string str_alloc(Arena *const arena, const size_t len) {
  assert(arena != NULL);

  return (string){
      .ptr = (char *)arena_alloc(arena, len, 1),
      .len = len,
  };
}

string str_copy(Arena *const arena, const string str) {
  if (str.len == 0) {
    return str;
  }
  assert(arena != NULL);

  char *dest = (char *)arena_alloc(arena, str.len, 1);
  return (string){
      .ptr = (char *)memcpy(dest, str.ptr, str.len),
      .len = str.len,
  };
}

char *const str_idx(const string str, const size_t i) {
  assert(i < str.len && str.ptr != NULL);
  return &str.ptr[i];
}

// Short-circuing if the length is equal and is 0 is because memcmp with NULL
// pointers is U.B. in c99, even if the number of bytes copied is 0, and in this
// library empty strings can have a NULL pointer.
//
// See `str_join` for more info
bool str_eq(const string a, const string b) {
  return a.len == b.len && (a.len == 0 || memcmp(a.ptr, b.ptr, a.len) == 0);
}

// Note! Can execute memcpy(ptr, NULL, 0), where ptr is guaranteed not to be
// NULL, which is U.B. before c2y. Clang and GCC apparently handle this
// gratefully while MSVC does not so, to conform to c99 we check for 0-len
// before memcpy-ing. This isn't not really a performance issue in release mode
// as it should just be optimised out (and gcc/clang with -O1 surely does)
//
// See
// https://stackoverflow.com/questions/5243012/is-it-guaranteed-to-be-safe-to-perform-memcpy0-0-0
string str_concat(Arena *const arena, const string a, const string b) {
  assert(arena != NULL);

  size_t new_len = a.len + b.len;
  string buf = str_alloc(arena, new_len);
  if (a.len != 0) {
    memcpy(buf.ptr, a.ptr, a.len);
  }
  if (b.len != 0) {
    memcpy(buf.ptr + a.len, b.ptr, b.len);
  }
  return buf;
}

string str_join(Arena *const arena, Vec(string) to_join) {
  assert(arena != NULL);

  size_t new_len = 0;
  for (size_t i = 0; i < to_join.len; i++) {
    new_len += vec_idx(to_join, i)->len;
  }
  string buf = str_alloc(arena, new_len);

  char *dest = buf.ptr;
  for (size_t i = 0; i < to_join.len; i++) {
    string str = *vec_idx(to_join, i);
    memcpy(dest, str.ptr, str.len);
    dest += str.len;
  }
  return buf;
}

string str_fmt(Arena *const arena, const char *fmt, ...) {
  assert(arena != NULL && fmt != NULL);

  va_list args;
  va_start(args, fmt);

  va_list args_temp;
  va_copy(args_temp, args);
  size_t needed = vsnprintf(NULL, 0, fmt, args_temp);
  va_end(args_temp);

  char *const buf = (char *)malloc(needed + 1);
  vsprintf(buf, fmt, args);
  string str = str_copy(arena, strlit(buf));
  memcpy(str.ptr, buf, needed);
  va_end(args);
  free(buf);
  return str;
}

string str_slice(const string str, const size_t start, const size_t end) {
  assert(start < str.len && end <= str.len && start <= end);

  return (string){
      .ptr = str.ptr + start,
      .len = end - start,
  };
}

string str_trim(const string str) {
  size_t start = 0;
  size_t end = str.len;

  while (start < end && isspace(*str_idx(str, start))) {
    start++;
  }
  if (start == end) {
    return (string){0};
  }
  while (end > start && isspace(*str_idx(str, end - 1))) {
    end--;
  }

  return str_slice(str, start, end);
}

#endif
