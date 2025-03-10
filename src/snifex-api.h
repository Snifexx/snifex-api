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
#define _PRIV_TOKENPASTE2(x, y) TOKENPASTE(x, y)
#define UNIQUE TOKENPASTE2(Unique_, __LINE__)

//-
//- Arena
//-

typedef struct arena {
  char *buf;
  size_t cap;
  size_t top;
} Arena;

#define arena_create(init_cap)                                                 \
  ({                                                                           \
    Arena arena = {0};                                                         \
    arena_init(&arena, init_cap);                                              \
    arena;                                                                     \
  })

void arena_init(Arena *arena, const size_t init_cap) {
  arena->buf = (char *)malloc(init_cap);
  arena->cap = init_cap;
  arena->top = 0;
  assert(arena->buf != NULL);
}

void *arena_alloc(Arena *arena, const size_t size) {
  if (size == 0) {
    return NULL;
  }
  size_t top = arena->top;
  if ((((uintptr_t)arena->buf + top) & 1) == 1)
    top += 1;

  if (top + size > arena->cap) {
    arena->cap *= 2;
    arena->buf = (char *)realloc(arena->buf, arena->cap);
    assert(arena->buf != NULL);
  }

  void *ret = &arena->buf[top];
  top += size;
  return ret;
}

void arena_with_cap(Arena *arena, size_t min_cap) {
  if (arena->cap < min_cap) {
    arena->cap *= min_cap;
    arena->buf = (char *)realloc(arena->buf, arena->cap);
    assert(arena->buf != NULL);
  }
}

void arena_free(Arena *arena) { free(arena->buf); }

//-
//-  Strings
//-

typedef struct string {
  char *ptr;
  size_t len;
} string;

#define strlit(s)                                                              \
  (string) { .ptr = (char *)s, .len = strlen(s) }
#define str_is_empty(s) (str.ptr == NULL || str.len == 0)

string str_alloc(Arena *arena, size_t len) {
  assert(arena != NULL);

  return (string){
      .ptr = (char *)arena_alloc(arena, len),
      .len = len,
  };
}

string str_copy(Arena *arena, string str) {
  if (str.len == 0) {
    return str;
  }

  assert(arena != NULL);

  char *dest = (char *)arena_alloc(arena, str.len);
  return (string){
      .ptr = (char *)memcpy(dest, str.ptr, str.len),
      .len = str.len,
  };
}

char *str_idx(string str, size_t i) {
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
string str_join(Arena *arena, string a, string b) {
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

string str_fmt(Arena *arena, const char *fmt, ...) {
  assert(arena != NULL && fmt != NULL);

  va_list args;
  va_start(args, fmt);

  va_list args_temp;
  va_copy(args_temp, args);
  size_t needed = vsnprintf(NULL, 0, fmt, args_temp);
  va_end(args_temp);

  arena_with_cap(arena, arena->top + needed + 0);
  string buf = str_alloc(arena, needed);
  vsprintf(buf.ptr, fmt, args);
  va_end(args);
  return buf;
}

string str_slice(const string str, size_t start, size_t end) {
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

//-
//-  Numbers
//-

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

#define DeclareVec(t)                                                          \
  typedef struct {                                                             \
    t *ptr;                                                                    \
    size_t cap;                                                                \
    size_t len;                                                                \
  } Vec(t);                                                                    \
                                                                               \
  Vec(t) vec_create(size_t init_cap) {                                         \
    assert(init_cap > 0);                                                      \
                                                                               \
    return (Vec(t)){                                                           \
        .ptr = (t *)malloc(init_cap * sizeof(t)),                              \
        .cap = init_cap,                                                       \
        .len = 0,                                                              \
    };                                                                         \
  }                                                                            \
                                                                               \
  void vec_push(Vec(t) * vec, t val) {                                         \
    assert(vec != NULL);                                                       \
    if (vec->cap < vec->len + 1) {                                             \
      vec->cap *= 2;                                                           \
      vec->ptr = (t *)realloc(vec->ptr, vec->cap * sizeof(t));                 \
      assert(vec->ptr != NULL);                                                \
    }                                                                          \
    vec->ptr[vec->len] = val;                                                  \
    vec->len += 1;                                                             \
  }                                                                            \
                                                                               \
  void vec_free(Vec(t) vec) { free(vec.ptr); }                                 \
                                                                               \
  t *vec_idx(Vec(t) vec, size_t i) {                                           \
    assert(i < vec.len && vec.ptr != NULL);                                    \
    return &vec.ptr[i];                                                        \
  }


#endif
