// HEADER

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

/// @defgroup macro_utils Macro utils
/// @brief Macro Utilities.
///
/// All examples are <a
/// href="https://github.com/Snifexx/snifex-api/tree/docs/src/examples-and-tests">here</a>
///
/// These include:
///   - The OS_* utility macros, for identifying OSs and platforms
///   - The asserting macros
/// @{

#if !defined(NO_GNU_SNIFEX_API_TESTS)
#if defined(__GNUC__)
#define SNIFEX_API_GNU_EXTENSIONS
#endif
#endif

// Here "OS" is also used for non Operating Systems like the Linux Kernel.
// I did so just to have them all in an encompassing group of macros.
#if defined(__ANDROID__)
#define OS_ANDROID
#elif defined(__gnu_linux__)
#define OS_GNULINUX
#elif defined(_WIN64)
#define OS_WIN64
#define OS_WIN
#elif defined(_WIN32)
#define OS_WIN32
#define OS_WIN
#elif defined(__CYGWIN__)
#define OS_CYGWIN
#define OS_WIN
#elif defined(__APPLE__) && defined(__MACH__)
#define OS_MACOSX
#elif defined(__FreeBSD__)
#define OS_FREEBSD
#endif

#if defined(__linux__)
#define OS_LINUX
#endif

#if defined(unix) || defined(__unix__) || defined(__unix) || defined(OS_MACOSX)
#define OS_UNIX
#endif

#ifdef OS_LINUX
#include <execinfo.h>
#endif  // OS_LINUX

#if defined(OS_WIN) && !defined(SNIFEX_API_NO_ASSERT)
#include <DbgHelp.h>
#include <windows.h>
#pragma comment(lib, "DbgHelp.lib")
#endif  // OS_WIN

/// @cond EXCLUDE_DOC
void __snifex_api_assert_fail(const char* expr,
                              const char* file,
                              const int line,
                              const char* func);
/// @endcond

#ifndef SNIFEX_API_NO_ASSERT
#undef assert
/// @brief Snifex API assert
///
/// Custom implementation of an assert. The fundamental changes are the
/// following:
///   - Persists in release mode
///   - Prints stack trace when possible
///   - Is just generally cleaner
/// @param expr Any `bool` expression that must be true
/// @hideinitializer
#define assert(expr)  \
  ((expr) ? ((void)0) \
          : __snifex_api_assert_fail(#expr, __FILE__, __LINE__, __func__))

#endif  // !SNIFEX_API_NO_ASSERT

/// @}

/// @defgroup number Numbers
/// @brief Utilities for numbers
///
/// All examples are <a
/// href="https://github.com/Snifexx/snifex-api/tree/docs/src/examples-and-tests">here</a>
///
/// All numerical functions and macros
/// @{

// I use macros because I want these to be the same for all number types
// without having 10 different implementation for each number type I use
#ifdef SNIFEX_API_GNU_EXTENSIONS

/// @brief Get the sign of a number
///
/// Get the normalized sign of any numerical type or 0
/// @param num Any numerical type value
/// @return -1 if `num` is negative, 1 if it's positive, 0 if `num` is 0
/// @hideinitializer
#define sign(num)                             \
  ({                                          \
    __auto_type s_num = num;                  \
    (s_num > 0 ? 1 : ((s_num < 0) ? -1 : 0)); \
  })
#else

/// @brief Get the sign of a number
///
/// Get the normalized sign of any numerical type or 0
/// @param lval_result_t An lvalue of type `t` to which the result is going to
/// be set
/// @param t The type of the `num` parameter
/// @param num Any numerical type value of type `t`
/// @return -1 if `num` is negative, 1 if it's positive, 0 if `num` is 0
/// @hideinitializer
#define sign(lval_result_t, t, num)                           \
  do {                                                        \
    t s_num = num;                                            \
    lval_result_t = (s_num > 0 ? 1 : ((s_num < 0) ? -1 : 0)); \
  } while (0)
#endif

static inline bool __snifex_api_is_power_of_two(const size_t x);

// extern uint32_t ceil_powtwo(uint32_t v);

/// @}

/// @defgroup arena Arena
/// @brief Fixed-sized and growing arenas
///
/// My implementation of an Arena allocator.
/// I've implemented both a fixed size Arena, and a dynamically growing one.
///
/// All examples are <a
/// href="https://github.com/Snifexx/snifex-api/tree/docs/src/examples-and-tests">here</a>
///
/// @{

/// @brief A dinamically growing arena
///
/// An arena that is not fixed-sized, and dynamically grows.
/// Because of this property, @ref dyn_arena_alloc does not return a pointer, it
/// returns an index with which we can access the pointer temporarily (with @ref
/// dyn_arena_get)
typedef struct dyn_arena {
  char* buf;   ///< @brief The arena buffer
  size_t cap;  ///< @brief The capacity of the arena, I.E. the amount of memory
               /// allocated
  size_t top;  ///< @brief The amount of bytes used by allocated objects, so the
               /// offset at which the newly allocated objects will start
} DynArena;

/// @brief A fixed-sized arena
///
/// A fixed-sized arena, that allocates a certain amount of memory when
/// initialized and never gets reallocated again. Because of this allocations
/// can fail when there is not enough memory allocated.
/// Since reallocations never happen, @ref arena_alloc return pointers, which is
/// VERY useful for strings or scratch arenas
typedef struct arena {
  char* buf;    ///< @brief The arena buffer
  size_t size;  ///< @brief The size of the arena
  size_t top;  ///< @brief The amount of allocated bytes, so the offset at which
               /// the newly allocated objects will start
} Arena;

/// @brief Initializes a @ref DynArena
/// @post `dyn_arena->buf != NULL` if `malloc` did not fail
extern void dyn_arena_init(DynArena* const dyn_arena, const size_t init_cap);
/// @brief Initializes an @ref Arena
/// @post `arena->buf != NULL` if `malloc` did not fail
extern void arena_init(Arena* const arena, const size_t size);
/// @brief Creates a @ref DynArena
/// @post `dyn_arena->buf != NULL` if `malloc` did not fail
extern DynArena dyn_arena_create(const size_t init_cap);
/// @brief Creates an @ref Arena
/// @post `arena->buf != NULL` if `malloc` did not fail
extern Arena arena_create(const size_t init_cap);
/// @brief Allocates an object into a @ref DynArena
///
/// Allocates object of size '`size`' aligned to '`alignment`'. If unsure of
/// what to use as `alignment`, just use the `size`
/// @return Returns an index from `dyn_arena->buf`. To get the obect at that
/// index refer to @ref dyn_arena_get
extern size_t dyn_arena_alloc(DynArena* const dyn_arena,
                              const size_t size,
                              const size_t alignment);

/// @brief Allocates an object into an @ref Arena
///
/// Allocates object of size '`size`' aligned to '`alignment`'. If unsure of
/// what to use as `alignment`, just use the `size`
/// @return Returns an pointer to the object that can be used directly
extern void* arena_alloc(Arena* const arena,
                         const size_t size,
                         const size_t alignment);

#ifdef SNIFEX_API_GNU_EXTENSIONS
/// @brief Get a temporary pointer from relative pointer of a @ref DynArena
///
/// Get a pointer to the object at from `rel_ptr`. The returning pointer should
/// not be stored or reused after a @ref dyn_arena_alloc allocation, because an
/// allocation could trigger a resizing rendering that pointer dangerous.
/// Instead, store, pass and work with the relative pointer retuned from @ref
/// dyn_arena_alloc and then, when working on the object in the @ref DynArena,
/// get a temporary "absolute" pointer with this macro
///
/// @param t The type of the item we're casting the pointer to
/// @param rel_ptr The relative pointer to the object in the @ref DynArena
/// @return An "absolute" pointer to the object of type `t` allocated in the
/// @ref DynArena at offset `rel_ptr`
/// @hideinitializer
#define dyn_arena_get(t, dyn_arena, rel_ptr)  \
  ({                                          \
    const DynArena dag_dyn_arena = dyn_arena; \
    const size_t dag_rel_ptr = rel_ptr;       \
    (t*)&dag_dyn_arena.buf[dag_rel_ptr];      \
  })
#else

/// @brief Get a temporary pointer from relative pointer of a @ref DynArena
///
/// Get a pointer to the object at from `rel_ptr`. The returning pointer should
/// not be stored or reused after a @ref dyn_arena_alloc allocation, because an
/// allocation could trigger a resizing rendering that pointer dangerous.
/// Instead, store, pass and work with the relative pointer retuned from @ref
/// dyn_arena_alloc and then, when working on the object in the @ref DynArena,
/// get a temporary "absolute" pointer with this macro
///
/// @param lval_result_t_ptr An lvalue of type `t*` to which the result is going
/// to be set
/// @param t The type of the item we're casting the pointer to
/// @param rel_ptr The relative pointer to the object in the @ref DynArena
/// @return An "absolute" pointer to the object of type `t` allocated in the
/// @ref DynArena at offset `rel_ptr`
/// @hideinitializer
#define dyn_arena_get(lval_result_t_ptr, t, dyn_arena, rel_ptr) \
  do {                                                          \
    const DynArena dag_dyn_arena = dyn_arena;                   \
    const size_t dag_rel_ptr = rel_ptr;                         \
    lval_result_t_ptr = (t*)&dag_dyn_arena.buf[dag_rel_ptr];    \
  } while (0)
#endif

/// @brief Reserves at least `min_cap` bytes in the arena
///
/// Allocates enough bytes of memory for the @ref DynArena capacity to be at
/// least `min_cap`. It is not a guaranteed realloc: it COULD trigger one, but
/// only if `dyn_arena->cap < min_cap`.
/// @post `dyn_arena->buf != NULL` if `malloc` did not fail
extern void dyn_arena_reserve(DynArena* const dyn_arena, const size_t min_cap);
/// @brief Frees a @ref DynArena
extern void dyn_arena_free(DynArena* const dyn_arena);
/// @brief Frees an @ref Arena
extern void arena_free(Arena* const arena);

/// @}

/// @defgroup vector Vector
/// @brief General type dynamically-growing arrays.
///
/// Data structure that owns its own data.
/// Basically... my implementation of a dynamically-growing array, ArrayList,
/// Vector or however you might call it.
///
/// All examples are <a
/// href="https://github.com/Snifexx/snifex-api/tree/docs/src/examples-and-tests">here</a>
/// @{

/// @brief Macro to declare a specifically typed Vector
///
/// The way I implemented generic vectors is by having macro work on vectors
/// instead of functions. Instead of the stb-db approach of having hidden
/// metadata, I prefer packing the metadata with the data itself, so that using
/// different allocating strategies is easier, E.G. @ref DynArena.
///
/// The downside is having to declare all the vector types used in the project.
/// This macro makes that process stupid simple. Take a look at this example:
/// @code
/// DefineVec(int);
///
/// int main() {
///   Vec(int) vector;
///   return 0;
/// }
/// @endcode
///
/// This is a general documentation for the structs generated by this macro:
/// @code
/// typedef struct {
///   t* ptr;     // Pointer to the buffer of the internal C array
///   size_t cap; // Capacity of the vector, I.E. size of the internal C array
///   size_t len; // Actual length of the vector, I.E. amount of elements in the
///   vector
/// } Vec_t; // Where `t` is any type passed to the macro
/// @endcode
///
/// @param t The type of the elements in the vector
/// @see - @ref Vec
#define DefineVec(t) \
  typedef struct {   \
    t* ptr;          \
    size_t cap;      \
    size_t len;      \
  } Vec_##t

/// @brief Macro to get the struct type of a vector of `t`s
///
/// @param t The type of the elements in the vector
/// @see @ref DefineVec for more info
#define Vec(t) Vec_##t

#ifdef SNIFEX_API_GNU_EXTENSIONS
/// @brief Create a vector of `t`s with an initial capacity of `init_cap`
///
/// @param t The type of the elements in the vector
/// @param init_cap The initial capacity of the vector
/// @hideinitializer
#define vec_create(t, init_cap)                  \
  ({                                             \
    const size_t vecc_init_cap = (init_cap);     \
    assert(vecc_init_cap > 0);                   \
                                                 \
    (Vec(t)){                                    \
        .ptr = calloc(vecc_init_cap, sizeof(t)), \
        .cap = vecc_init_cap,                    \
        .len = 0,                                \
    };                                           \
  })

/// @brief Create a vector from a list of initial elements
///
/// @param t The type of the elements in the vector
/// @param item1 The first element of the vector. Must be of type `t`
/// @param ... More following optional elements. All must be of type `t`
/// @pre `item1` and varargs `...` must be of type `t`
/// @hideinitializer
#define vec_from(t, item1, ...)               \
  ({                                          \
    t items[] = {(item1), __VA_ARGS__};       \
    size_t count = sizeof(items) / sizeof(t); \
    __auto_type vec = vec_create(t, count);   \
    vec.len = count;                          \
    memcpy(vec.ptr, items, sizeof(items));    \
    vec;                                      \
  })

/// @brief Get pointer to element of vector at specific index
///
/// @param vec The vector we're indexing
/// @param i The index of the element
/// @return Pointer to the indexed element
/// @pre `i >= 0` (since it's of type `size_t`)
/// @pre `vec.len > 0`
/// @pre `i < vec.len`
/// @pre `vec.ptr != NULL` (which should be true if the user did not mess with
/// the `ptr` directly)
/// @hideinitializer
#define vec_idx(vec, i)                                                        \
  ({                                                                           \
    __typeof(vec) veci_vec = (vec);                                            \
    const size_t veci_i = (i);                                                 \
    assert(veci_vec.len > 0 && veci_i < veci_vec.len && veci_vec.ptr != NULL); \
    &(veci_vec).ptr[veci_i];                                                   \
  })

/// @brief Get pointer to last element of vector
///
/// @param vec The vector we're taking the element from
/// @return Pointer to the last element
/// @pre `vec.ptr != NULL` (which should be true if the user did not mess with
/// the `ptr` directly)
/// @hideinitializer
#define vec_last(vec) vec_idx((vec), veci_vec.len - 1);

/// @brief Pushes value to the end of the vector
///
/// @note
/// Could trigger reallocation
///
/// @param vec_ptr Pointer to the vector we're pushing to
/// @param val Value to push
/// @pre `val` must be the same type of the vector elements
/// @pre `vec_ptr != NULL`
/// @post `vec_ptr->ptr != NULL` if `realloc` did not fail
/// @hideinitializer
#define vec_push(vec_ptr, val)                                              \
  do {                                                                      \
    const __typeof(*(vec_ptr)->ptr) vecp_val = (val);                       \
    __typeof(vec_ptr) vecp_vec_ptr = (vec_ptr);                             \
    assert(vecp_vec_ptr != NULL);                                           \
    if (vecp_vec_ptr->len + 1 > vecp_vec_ptr->cap) {                        \
      vecp_vec_ptr->cap += 1;                                               \
      vecp_vec_ptr->cap *= 2;                                               \
      vecp_vec_ptr->ptr =                                                   \
          realloc(vecp_vec_ptr->ptr, vecp_vec_ptr->cap * sizeof(vecp_val)); \
      assert(vecp_vec_ptr->ptr != NULL);                                    \
    }                                                                       \
    *(vecp_vec_ptr->ptr + vecp_vec_ptr->len) = vecp_val;                    \
    vecp_vec_ptr->len += 1;                                                 \
  } while (0)

/// @brief Pops value from the end of the vector, reducing it's length by 1 (if
/// possible)
///
/// @param vec_ptr Pointer to the vector we're popping from
/// @pre `vec_ptr != NULL`
/// @hideinitializer
#define vec_pop(vec_ptr)                                   \
  do {                                                     \
    __typeof(vec_ptr) vecpop_vec_ptr = (vec_ptr);          \
    assert(vecpop_vec_ptr != NULL);                        \
    if (vecpop_vec_ptr->len > 0) vecpop_vec_ptr->len -= 1; \
  } while (0)

/// @brief Appends items from a vector to the back of another vector
///
/// @note
/// Could trigger reallocation
///
/// @param front_ptr Pointer to the vector we're appending to.
/// @param back Vector we're taking the items from. The items will be copied.
/// @pre `front_ptr != NULL`
/// @post `front_ptr->ptr != NULL` if `realloc` did not fail
/// @hideinitializer
#define vec_append(front_ptr, back)                                    \
  do {                                                                 \
    __typeof(front_ptr) veca_front_ptr = (front_ptr);                  \
    __typeof(*front_ptr) veca_back = (back);                           \
    assert(veca_front_ptr != NULL);                                    \
                                                                       \
    if (veca_front_ptr->len + veca_back.len > veca_front_ptr->cap) {   \
      veca_front_ptr->cap += veca_back.len;                            \
      veca_front_ptr->cap *= 1.5;                                      \
      veca_front_ptr->ptr =                                            \
          realloc(veca_front_ptr->ptr,                                 \
                  veca_front_ptr->cap * sizeof(*veca_front_ptr->ptr)); \
      assert(veca_front_ptr->ptr != NULL);                             \
    }                                                                  \
    memcpy(veca_front_ptr->ptr + veca_front_ptr->len, veca_back.ptr,   \
           veca_back.len * sizeof(*veca_front_ptr->ptr));              \
    veca_front_ptr->len += veca_back.len;                              \
  } while (0)

/// @brief Perform a 'swap remove' on vector
///
/// A 'swap remove' is a specific type of deletion on arrays that is constant
/// time. It consists of moving the last element into the slot of the element
/// we're removing, and then decreasing the vector length by one.
/// As you probably have noticed, it does NOT maintain the elements' order
///
/// @param vec_ptr Pointer to the vector we're working with
/// @param idx Index of the element to remove
/// @pre `vec_ptr != NULL`
/// @pre `vec_ptr->len > 0`
/// @hideinitializer
#define vec_swap_remove(vec_ptr, idx)                           \
  do {                                                          \
    __auto_type vecsr_vec_ptr = (vec_ptr);                      \
    size_t vecsr_idx = (idx);                                   \
    assert(vecsr_vec_ptr != NULL && vecsr_vec_ptr->len > 0);    \
                                                                \
    __auto_type remove_at = vec_idx(*vecsr_vec_ptr, vecsr_idx); \
    __auto_type to_replace = *vec_last(*vecsr_vec_ptr);         \
    *remove_at = to_replace;                                    \
    vecsr_vec_ptr->len--;                                       \
  } while (0)

#else  // NON SNIFEX_API_GNU_EXTENSIONS

/// @brief Create a vector of `t`s with an initial capacity of `init_cap`
///
/// @param lval_result_vec An lvalue of type `Vec(t)` to which the result is
/// going to be set
/// @param t The type of the elements in the vector
/// @param init_cap The initial capacity of the vector
/// @hideinitializer
#define vec_create(lval_result_vec, t, init_cap)  \
  do {                                            \
    const size_t vecc_init_cap = (init_cap);      \
    assert(vecc_init_cap > 0);                    \
                                                  \
    lval_result_vec = (Vec(t)){                   \
        .ptr = malloc(vecc_init_cap * sizeof(t)), \
        .cap = vecc_init_cap,                     \
        .len = 0,                                 \
    };                                            \
  } while (0)

/// @brief Create a vector from a list of initial elements
///
/// @param lval_result_vec An lvalue of type `Vec(t)` to which the result is
/// going to be set
/// @param t The type of the elements in the vector
/// @param item1 The first element of the vector. Must be of type `t`
/// @param ... More following optional elements. All must be of type `t`
/// @pre `item1` and varargs `...` must be of type `t`
/// @hideinitializer
#define vec_from(lval_result_vec, t, item1, ...) \
  do {                                           \
    t items[] = {(item1), __VA_ARGS__};          \
    size_t count = sizeof(items) / sizeof(t);    \
    Vec(t) vec;                                  \
    vec_create(vec, t, count);                   \
    vec.len = count;                             \
    memcpy(vec.ptr, items, sizeof(items));       \
    lval_result_vec = vec;                       \
  } while (0)

/// @brief Get pointer to element of vector at specific index
///
/// @param lval_result_elem_ptr An lvalue of type `t*` to which the result is
/// going to be set
/// @param t The type of the elements in the vector
/// @param vec The vector we're indexing
/// @param i The index of the element
/// @return Pointer to the indexed element
/// @pre `i >= 0` (since it's of type `size_t`)
/// @pre `vec.len > 0`
/// @pre `i < vec.len`
/// @pre `vec.ptr != NULL` (which should be true if the user did not mess with
/// the `ptr` directly)
/// @hideinitializer
#define vec_idx(lval_result_elem_ptr, t, vec, i)                               \
  do {                                                                         \
    Vec(t) veci_vec = (vec);                                                   \
    const size_t veci_i = (i);                                                 \
    assert(veci_vec.len > 0 && veci_i < veci_vec.len && veci_vec.ptr != NULL); \
    lval_result_elem_ptr = &(veci_vec).ptr[veci_i];                            \
  } while (0)

/// @brief Get pointer to last element of vector
///
/// @param lval_result_elem_ptr An lvalue of type `t*` to which the result is
/// going to be set
/// @param t The type of the elements in the vector
/// @param vec The vector we're taking the element from
/// @return Pointer to the last element
/// @pre `vec.ptr != NULL` (which should be true if the user did not mess with
/// the `ptr` directly)
/// @hideinitializer
#define vec_last(lval_result_elem_ptr, t, vec) \
  vec_idx(lval_result_elem_ptr, t, (vec), veci_vec.len - 1);

/// @brief Pushes value to the end of the vector
///
/// @note
/// Could trigger reallocation
///
/// @param t The type of the elements in the vector
/// @param vec_ptr Pointer to the vector we're pushing to
/// @param val Value to push
/// @pre `val` must be the same type of the vector elements
/// @pre `vec_ptr != NULL`
/// @post `vec_ptr->ptr != NULL` if `realloc` did not fail
/// @hideinitializer
#define vec_push(t, vec_ptr, val)                                    \
  do {                                                               \
    assert(vec_ptr != NULL);                                         \
    const t vecp_val = (val);                                        \
    Vec(t)* vecp_vec_ptr = (vec_ptr);                                \
    if (vecp_vec_ptr->len + 1 > vecp_vec_ptr->cap) {                 \
      vecp_vec_ptr->cap += 1;                                        \
      vecp_vec_ptr->cap *= 2;                                        \
      vecp_vec_ptr->ptr =                                            \
          realloc(vecp_vec_ptr->ptr, vecp_vec_ptr->cap * sizeof(t)); \
      assert(vecp_vec_ptr->ptr != NULL);                             \
    }                                                                \
    *(vecp_vec_ptr->ptr + vecp_vec_ptr->len) = vecp_val;             \
    vecp_vec_ptr->len += 1;                                          \
  } while (0)

/// @brief Pops value from the end of the vector, reducing it's length by 1 (if
/// possible)
///
/// @param t The type of the elements in the vector
/// @param vec_ptr Pointer to the vector we're popping from
/// @pre `vec_ptr != NULL`
/// @hideinitializer
#define vec_pop(t, vec_ptr)                                \
  do {                                                     \
    assert(vec_ptr != NULL);                               \
    Vec(t)* vecpop_vec_ptr = (vec_ptr);                    \
    if (vecpop_vec_ptr->len > 0) vecpop_vec_ptr->len -= 1; \
  } while (0)

/// @brief Appends items from a vector to the back of another vector
///
/// @note
/// Could trigger reallocation
///
/// @param t The type of the elements in the vector
/// @param front_ptr Pointer to the vector we're appending to.
/// @param back Vector we're taking the items from. The items will be copied.
/// @pre `front_ptr != NULL`
/// @post `front_ptr->ptr != NULL` if `realloc` did not fail
/// @hideinitializer
#define vec_append(t, front_ptr, back)                                   \
  do {                                                                   \
    Vec(t)* veca_front_ptr = (front_ptr);                                \
    Vec(t) veca_back = (back);                                           \
                                                                         \
    if (veca_front_ptr->len + veca_back.len > veca_front_ptr->cap) {     \
      veca_front_ptr->cap += veca_back.len;                              \
      veca_front_ptr->cap *= 1.5;                                        \
      veca_front_ptr->ptr =                                              \
          realloc(veca_front_ptr->ptr, veca_front_ptr->cap * sizeof(t)); \
      assert(veca_front_ptr->ptr != NULL);                               \
    }                                                                    \
    memcpy(veca_front_ptr->ptr + veca_front_ptr->len, veca_back.ptr,     \
           veca_back.len * sizeof(t));                                   \
    veca_front_ptr->len += veca_back.len;                                \
  } while (0)

/// @brief Perform a 'swap remove' on vector
///
/// A 'swap remove' is a specific type of deletion on arrays that is constant
/// time. It consists of moving the last element into the slot of the element
/// we're removing, and then decreasing the vector length by one.
/// As you probably have noticed, it does NOT maintain the elements' order
///
/// @param t The type of the elements in the vector
/// @param vec_ptr Pointer to the vector we're working with
/// @param idx Index of the element to remove
/// @pre `vec_ptr != NULL`
/// @pre `vec_ptr->len > 0`
/// @hideinitializer
#define vec_swap_remove(t, vec_ptr, idx)                     \
  do {                                                       \
    Vec(t)* vecsr_vec_ptr = (vec_ptr);                       \
    size_t vecsr_idx = (idx);                                \
    assert(vecsr_vec_ptr != NULL && vecsr_vec_ptr->len > 0); \
                                                             \
    t* remove_at;                                            \
    vec_idx(remove_at, t, *vecsr_vec_ptr, vecsr_idx);        \
    t* to_replace;                                           \
    vec_last(to_replace, t, *vecsr_vec_ptr);                 \
    *remove_at = *to_replace;                                \
    vecsr_vec_ptr->len--;                                    \
  } while (0)
#endif

/// @brief Frees the vector
/// @hideinitializer
#define vec_free(vec_ptr) free((vec_ptr)->ptr)

/// @}

/// @defgroup string String
/// @brief Just... Strings 🙂
///
/// All examples are <a
/// href="https://github.com/Snifexx/snifex-api/tree/docs/src/examples-and-tests">here</a>
/// @{

/// @brief A sized string view.
///
/// This a simple sized string view... Nothing else.
/// For the uninitiated:
/// - 'sized' means that we carry the length of the data with the data (the
/// char*)
/// - 'view' because the memory is not owned. That means that it is the user
/// responsibility to manage the memory the string view is pointing to. The
/// recommended way is using an arena.
///
/// @note
/// These strings are NOT zero tailing (since they're string-views anyways)
typedef struct string {
  char* ptr;   ///< Internal string buffer
  size_t len;  ///< Length of string
} string;

DefineVec(string);

/// @brief Returns a string literal (char const*) to a @ref string format
extern string strlit(char const* s);
/// @brief Allocates a string of length `len` in an arena
/// @pre `arena != NULL`
extern string str_alloc(Arena* const arena, const size_t len);
/// @brief Gets pointer to character at specific index
///
/// @pre `i >= 0` (since it's of type `size_t`)
/// @pre `i < str.len`
/// @pre `str.ptr != NULL` which should be true if the user did not mess with
/// the `ptr` directly
char* str_idx(const string str, const size_t i);
/// @brief Returns whether the string is empty
///
/// An empty @ref string means that either the length is 0 or the internal `ptr`
/// is NULL
extern bool str_is_empty(const string str);

/// @brief Returns whether two @ref strings are equal
///
/// @par Implementation details
/// Short-circuing if the length is equal and is 0. This is because memcmp with
/// NULL pointers is U.B. in c99, even if the number of bytes copied is 0,
/// and in this library empty strings can have a NULL pointer.
///
/// @see @ref str_join for more info
extern bool str_eq(const string a, const string b);

/// @brief Returns result of concatination of string `b` into `a`
///
/// @par Implementation details
/// If we are not careful we could execute memcpy(ptr, NULL, 0), where ptr is
/// guaranteed not to be NULL, which is U.B. before c2y. Clang and GCC
/// apparently handle this gratefully while MSVC does not. So to conform to c99
/// we check for 0-len before memcpy-ing. This isn't not really a performance
/// issue in release mode as it should just be optimised out (and gcc/clang with
/// -O1 surely does)
///
/// @pre `arena != NULL`
/// @see
/// https://stackoverflow.com/questions/5243012/is-it-guaranteed-to-be-safe-to-perform-memcpy0-0-0
extern string str_concat(Arena* const arena, const string a, const string b);
/// @brief Returns copy of a string
///
/// @pre `arena != NULL`
extern string str_copy(Arena* const arena, const string str);
/// @brief Joins a list of strings together and returns the returning string
///
/// @pre `arena != NULL`
extern string str_join(Arena* const arena, Vec(string) to_join);
/// @brief Returns a formatted string
///
/// @pre `arena != NULL`
/// @pre `fmt != NULL`
extern string str_fmt(Arena* const arena, const char* fmt, ...);
/// @brief Returns a string slice
///
/// @pre `start >= 0` (since it's of type `size_t`)
/// @pre `end >= 0` (since it's of type `size_t`)
/// @pre `start < str.len`
/// @pre `end <= str.len`
/// @pre `start <= end`
extern string str_slice(const string str, const size_t start, const size_t end);
/// @brief Returns a trimmed string
extern string str_trim(const string str);

/// @}

/// @defgroup dict Dictionary
/// @brief General type hashmaps
///
/// This implementation of hashmap is somewhat reminiscent of stb-db's one. The
/// inner workings are the same: an hashmap that uses open addressing (or closed
/// hashing) where buckets do not cointain the entries them self but contain an
/// index to a vector where we store entries contigiously.
/// Of course, since they mimic the generic-like feeling of my @ref Vec, we have
/// a different way of implementing parametric polymorphism, but the core
/// hashmap implementation details are pretty much the same.
/// I also implemented a default very simplistic hashing function, but added a
/// way for users to use a custom hashing algorithm, and I highly suggest you do
/// so.
///
/// All examples are <a
/// href="https://github.com/Snifexx/snifex-api/tree/docs/src/examples-and-tests">here</a>
/// @{

/// @cond EXCLUDE_DOC
typedef struct bucket {
  uint64_t hash;
  size_t index;
} Bucket;
/// @endcond

/// @brief Macro to get the entry type of a dictionary mapping `K`s to `V`s
///
/// @param K Type of keys of the dictionary
/// @param V Type of values of the dictionary
/// @see @ref DefineDict for more info
#define Entry(K, V) Entry_##K##_##V
/// @brief Macro to get the type of a dictionary mapping `K`s to `V`s
///
/// @param K Type of keys of the dictionary
/// @param V Type of values of the dictionary
/// @see @ref DefineDict for more info
#define Dict(K, V) Dictionary_##K##_##V
/// @brief Macro to get type of vector of entries from a dictionary mapping `K`s
/// to `V`s
///
/// @param K Type of keys of the dictionary
/// @param V Type of values of the dictionary
/// @see @ref DefineDict for more info
#define VecEntry(K, V) Vec_Entry_##K##_##V

/// @brief Macro to declare a specifically typed dictionary
///
/// The way I implemented generic dictionary is similar to that of vectors:
/// by having macros instead of functions.
///
/// This macro is the equivalent of @ref DefineVec for dictionaries.
/// Take a look at this example:
/// @code
/// DefineDict(int, float);
///
/// int main() {
///   Dict(int, float) dictionary;
///   return 0;
/// }
/// @endcode
///
/// This is a general documentation for the structs generated by this macro:
/// @code
/// typedef struct {
///   K key;
///   V value;
/// } Entry_K_V;
///
/// typedef struct {
///   Vec_Entry_K_V entries; /* Vector of all entries in the dictionary. Order
///                             of entries is not guaranteed to be stable */
///   uint64_t key[2];       /* This key is used for hashing. When creating a
///                             dictionary it gets set to {0, 0}.
///                             It's a uint128_t where the first uint64_t in the
///                             array is the least significant one.
///                             Unless your hashing function depends on one
///                             (which by default it does not), you can ignore
///                             the key */
///   ...                    // internal stuff
/// } Dictionary_K_V; // Where `K` and `V` are the types of keys and values
/// @endcode
///
/// @param t The type of the elements in the vector
/// @see - @ref DefineVec
/// @see - @ref Dict
/// @hideinitializer
#define DefineDict(K, V)      \
  typedef struct {            \
    K key;                    \
    V value;                  \
  } Entry(K, V);              \
                              \
  DefineVec(Entry_##K##_##V); \
                              \
  typedef struct {            \
    VecEntry(K, V) entries;   \
    Bucket* buckets;          \
    size_t b_len;             \
    size_t b_cap;             \
    uint64_t key[2];          \
  } Dict(K, V);

// This is my way of implementing custom hashing algorithms. Honestly just
// looking at the examples in the repo is the best way to see it in action
#ifndef HASHFUNC
#define hash_num(in_ptr, inlen, k0_u64, k1_u64) \
  (snifex_api_hash_num_func(in_ptr, inlen))
#endif

/// @cond EXCLUDE_DOC
uint64_t snifex_api_hash_num_func(const void* in, const size_t inlen);

Bucket* snifex_api_find_bucket(Bucket* buckets,
                               size_t bucket_cap,
                               void* key,
                               uint64_t hashed_key,
                               void* entries,
                               size_t entry_size,
                               size_t key_size,
                               bool include_tombs);
void snifex_api_dict_grow(Bucket** buckets,
                          size_t* bucket_cap,
                          size_t* bucket_len);
/// @endcond

#ifdef SNIFEX_API_GNU_EXTENSIONS

/// @brief Create a dictionary of `K`s to `V`s
///
/// @par Implementation details
/// The number of initial buckets is 8
///
/// @param K The type of the keys of dictionary's entries
/// @param V The type of the values of dictionary's entries
/// @hideinitializer
#define dict_create(K, V)                    \
  ((Dict(K, V)){                             \
      .entries = vec_create(Entry(K, V), 8), \
      .buckets = calloc(8, sizeof(Bucket)),  \
      .b_cap = 8,                            \
      .b_len = 0,                            \
      .key = {0, 0},                         \
  })

/// @brief Inserts an entry in the hashmap
///
/// @note
/// Could trigger bucket resizing
///
/// @param dict_ptr Pointer to the dictionary
/// @param k Key of the entry we're inserting
/// @param v Value of the entry we're inserting
/// @param old_value_ptr If it's non-null, the value of this pointer will be set
/// to the old value associated with the key, if there was one. Otherwise
/// nothing happens.
/// @hideinitializer
#define dict_put(dict_ptr, k, v, old_value_ptr)                               \
  ({                                                                          \
    __auto_type dp_dict_ptr = (dict_ptr);                                     \
    __typeof(dp_dict_ptr->entries.ptr->key) dp_k = (k);                       \
    __typeof(dp_dict_ptr->entries.ptr->value) dp_v = (v);                     \
    __typeof(&dp_v) _old_value_ptr = (old_value_ptr);                         \
    uint64_t hasheddp_k =                                                     \
        hash_num(&dp_k, sizeof(dp_k), dp_dict_ptr->key[0], dp_dict_ptr->[1]); \
    Bucket* b = snifex_api_find_bucket(                                       \
        dp_dict_ptr->buckets, dp_dict_ptr->b_cap, &dp_k, hasheddp_k,          \
        dp_dict_ptr->entries.ptr, sizeof(*(dp_dict_ptr->entries.ptr)),        \
        sizeof(dp_dict_ptr->entries.ptr->key), true);                         \
    if (b->index > 1) {                                                       \
      __auto_type e = vec_idx(dp_dict_ptr->entries, b->index - 2);            \
      if (_old_value_ptr != NULL) { *_old_value_ptr = e->value; }             \
      e->value = dp_v;                                                        \
      true;                                                                   \
    } else {                                                                  \
      if (b->index == 0) dp_dict_ptr->b_len++;                                \
      b->hash = hasheddp_k;                                                   \
      b->index = dp_dict_ptr->entries.len + 2;                                \
                                                                              \
      if (dp_dict_ptr->b_len >= dp_dict_ptr->b_cap * 0.75) {                  \
        snifex_api_dict_grow(&dp_dict_ptr->buckets, &dp_dict_ptr->b_cap,      \
                             &dp_dict_ptr->b_len);                            \
      }                                                                       \
      __typeof(*dp_dict_ptr->entries.ptr) to_push_entry = {.key = dp_k,       \
                                                           .value = dp_v};    \
      vec_push((&dp_dict_ptr->entries), to_push_entry);                       \
      false;                                                                  \
    }                                                                         \
  })

/// @brief Searches key in the dictionary, and returns pointer to the entry if
/// found
///
/// @param dict_ptr Pointer to the dictionary
/// @param k Key of the entry we're searching
/// @return `NULL` if there is no entry with associated key, entry pointer
/// otherwise
/// @hideinitializer
#define dict_get(dict_ptr, k)                                            \
  ({                                                                     \
    __auto_type dg_dict_ptr = (dict_ptr);                                \
    __typeof(dg_dict_ptr->entries.ptr->key) dg_k = (k);                  \
    __typeof(&dg_dict_ptr->entries.ptr->value) res;                      \
                                                                         \
    if (dg_dict_ptr->b_len == 0) {                                       \
      res = NULL;                                                        \
    } else {                                                             \
      Bucket* b = snifex_api_find_bucket(                                \
          dg_dict_ptr->buckets, dg_dict_ptr->b_cap, &dg_k,               \
          hash_num(&dg_k, sizeof(dg_k), dg_dict_ptr->key[0],             \
                   dg_dict_ptr->key[1]),                                 \
          dg_dict_ptr->entries.ptr, sizeof(*(dg_dict_ptr->entries.ptr)), \
          sizeof(dg_dict_ptr->entries.ptr->key), false);                 \
      if (b->index == 0) {                                               \
        res = NULL;                                                      \
      } else {                                                           \
        res = &vec_idx(dg_dict_ptr->entries, (b->index - 2))->value;     \
      }                                                                  \
    }                                                                    \
    res;                                                                 \
  })

/// @brief Deletes entry from a dictionary
///
/// @par Implementation details
/// It uses @ref vec_swap_remove to remove an entry. Because of that it does not
/// guarantee order of the entry vector
///
/// @param dict_ptr Pointer to the dictionary
/// @param k Key of the entry we're deleting
/// @return Whether entry with associated key exists and was removed
/// @hideinitializer
#define dict_del(dict_ptr, k)                                              \
  ({                                                                       \
    __auto_type dd_dict_ptr = (dict_ptr);                                  \
    __typeof(dd_dict_ptr->entries.ptr->key) dd_k = (k);                    \
                                                                           \
    if (dd_dict_ptr->b_len == 0) {                                         \
      false;                                                               \
    } else {                                                               \
      Bucket* b = snifex_api_find_bucket(                                  \
          dd_dict_ptr->buckets, dd_dict_ptr->b_cap, &dd_k,                 \
          hash_num(&dd_k, sizeof(dd_k), dd_dict_ptr->key[0],               \
                   dd_dict_ptr->key[1]),                                   \
          dd_dict_ptr->entries.ptr, sizeof(*(dd_dict_ptr->entries.ptr)),   \
          sizeof(dd_dict_ptr->entries.ptr->key), false);                   \
      if (b->index == 0) {                                                 \
        false;                                                             \
      } else {                                                             \
        __auto_type last_entry = *vec_last(dd_dict_ptr->entries);          \
        __auto_type le_k = last_entry.key;                                 \
        Bucket* last_b = snifex_api_find_bucket(                           \
            dd_dict_ptr->buckets, dd_dict_ptr->b_cap, &le_k,               \
            hash_num(&le_k, sizeof(le_k), dd_dict_ptr->key[0],             \
                     dd_dict_ptr->key[1]),                                 \
            dd_dict_ptr->entries.ptr, sizeof(*(dd_dict_ptr->entries.ptr)), \
            sizeof(dd_dict_ptr->entries.ptr->key), false);                 \
        last_b->index = b->index;                                          \
        /* Remove and place tombstone */                                   \
        vec_swap_remove(&dd_dict_ptr->entries, last_b->index - 2);         \
        b->index = 1;                                                      \
        true;                                                              \
      }                                                                    \
    }                                                                      \
  })

/// @brief Frees the dictionary
///
/// @param dict_ptr Pointer to the dictionary
/// @hideinitializer
#define dict_free(dict_ptr)               \
  do {                                    \
    __auto_type df_dict_ptr = (dict_ptr); \
    free(df_dict_ptr->buckets);           \
    vec_free(&df_dict_ptr->entries);      \
  } while (0)

#else  // !SNIFEX_API_GNU_EXTENSIONS

/// @brief Create a dictionary of `K`s to `V`s
///
/// @par Implementation details
/// The number of initial buckets is 8
///
/// @param lval_result_dict An lvalue of type `Dict(K, V)` to which the result
/// is going to be set
/// @param K The type of the keys of dictionary's entries
/// @param V The type of the values of dictionary's entries
/// @hideinitializer
#define dict_create(lval_result_dict, K, V)   \
  do {                                        \
    Vec(Entry_##K##_##V) e;                   \
    vec_create(e, Entry(K, V), 8);            \
    lval_result_dict = (Dict(K, V)){          \
        .entries = e,                         \
        .buckets = calloc(8, sizeof(Bucket)), \
        .b_cap = 8,                           \
        .b_len = 0,                           \
        .key = {0, 0},                        \
    };                                        \
  } while (0)

/// @brief Inserts an entry in the hashmap
///
/// @note
/// Could trigger bucket resizing
///
/// @param dict_ptr Pointer to the dictionary
/// @param k_type The type of the keys in the dictionary
/// @param v_type The type of the values in the dictionary
/// @param k Key of the entry we're inserting
/// @param v Value of the entry we're inserting
/// @param old_value_ptr If it's non-null, the value of this pointer will be set
/// to the old value associated with the key, if there was one. Otherwise
/// nothing happens.
/// @hideinitializer
#define dict_put(k_type, v_type, dict_ptr, k, v, old_value_ptr)                \
  ({                                                                           \
    Dict(k_type, v_type)* dp_dict_ptr = (dict_ptr);                            \
    k_type dp_k = (k);                                                         \
    v_type dp_v = (v);                                                         \
    v_type* _old_value_ptr = (old_value_ptr);                                  \
    uint64_t hasheddp_k =                                                      \
        hash_num(&dp_k, sizeof(dp_k), dp_dict_ptr->key[0], dp_dict_ptr->[1]);  \
    Bucket* b = snifex_api_find_bucket(                                        \
        dp_dict_ptr->buckets, dp_dict_ptr->b_cap, &dp_k, hasheddp_k,           \
        dp_dict_ptr->entries.ptr, sizeof(*(dp_dict_ptr->entries.ptr)),         \
        sizeof(dp_dict_ptr->entries.ptr->key), true);                          \
    if (b->index > 1) {                                                        \
      Entry(k_type, v_type) * e;                                               \
      vec_idx(e, Entry(k_type, v_type), dp_dict_ptr->entries, b->index - 2);   \
      if (_old_value_ptr != NULL) { *_old_value_ptr = e->value; }              \
      e->value = dp_v;                                                         \
      true;                                                                    \
    } else {                                                                   \
      if (b->index == 0) dp_dict_ptr->b_len++;                                 \
      b->hash = hasheddp_k;                                                    \
      b->index = dp_dict_ptr->entries.len + 2;                                 \
                                                                               \
      if (dp_dict_ptr->b_len >= dp_dict_ptr->b_cap * 0.75) {                   \
        snifex_api_dict_grow(&dp_dict_ptr->buckets, &dp_dict_ptr->b_cap,       \
                             &dp_dict_ptr->b_len);                             \
      }                                                                        \
      Entry(k_type, v_type) to_push_entry = {.key = dp_k, .value = dp_v};      \
      vec_push(Entry(k_type, v_type), (&dp_dict_ptr->entries), to_push_entry); \
      false;                                                                   \
    }                                                                          \
  })

/// @brief Searches key in the dictionary, and returns pointer to the value if
/// found
///
/// @param lval_result_val_ptr An lvalue of type `v_type*` to which the result
/// is going to be set
/// @param k_type The type of the keys in the dictionary
/// @param v_type The type of the values in the dictionary
/// @param dict_ptr Pointer to the dictionary
/// @param k Key of the entry we're searching
/// @hideinitializer
#define dict_get(lval_result_val_ptr, k_type, v_type, dict_ptr, k)       \
  do {                                                                   \
    Dict(k_type, v_type)* dg_dict_ptr = (dict_ptr);                      \
    k_type dg_k = (k);                                                   \
                                                                         \
    if (dg_dict_ptr->b_len == 0) {                                       \
      lval_result_val_ptr = NULL;                                        \
    } else {                                                             \
      Bucket* b = snifex_api_find_bucket(                                \
          dg_dict_ptr->buckets, dg_dict_ptr->b_cap, &dg_k,               \
          hash_num(&dg_k, sizeof(dg_k), dg_dict_ptr->key[0],             \
                   dg_dict_ptr->key[1]),                                 \
          dg_dict_ptr->entries.ptr, sizeof(*(dg_dict_ptr->entries.ptr)), \
          sizeof(dg_dict_ptr->entries.ptr->key), false);                 \
      if (b->index == 0) {                                               \
        lval_result_val_ptr = NULL;                                      \
      } else {                                                           \
        Entry(k_type, v_type) * e;                                       \
        vec_idx(e, Entry(k_type, v_type), dg_dict_ptr->entries,          \
                (b->index - 2));                                         \
        lval_result_val_ptr = &e->value;                                 \
      }                                                                  \
    }                                                                    \
  } while (0)

/// @brief Deletes entry from a dictionary
///
/// @par Implementation details
/// It uses @ref vec_swap_remove to remove an entry. Because of that it does not
/// guarantee order of the entry vector
///
/// @param lval_result_bool An lvalue of type `bool` to which the we set whether
/// entry with associated key exists and was removed
/// @param k_type The type of the keys in the dictionary
/// @param v_type The type of the values in the dictionary
/// @param dict_ptr Pointer to the dictionary
/// @param k Key of the entry we're deleting
/// @hideinitializer
#define dict_del(lval_result_bool, k_type, v_type, dict_ptr, k)          \
  ({                                                                     \
    Dict(k_type, v_type)* dd_dict_ptr = (dict_ptr);                      \
    k_type dd_k = (k);                                                   \
                                                                         \
    if (dd_dict_ptr->b_len == 0) {                                       \
      lval_result_bool = false;                                          \
    } else {                                                             \
      Bucket* b = snifex_api_find_bucket(                                \
          dd_dict_ptr->buckets, dd_dict_ptr->b_cap, &dd_k,               \
          hash_num(&dd_k, sizeof(dd_k), dd_dict_ptr->key[0],             \
                   dd_dict_ptr->key[1]),                                 \
          dd_dict_ptr->entries.ptr, sizeof(*(dd_dict_ptr->entries.ptr)), \
          sizeof(dd_dict_ptr->entries.ptr->key), false);                 \
      if (b->index == 0) {                                               \
        lval_result_bool = false;                                        \
      } else {                                                           \
        /* Remove and place tombstone */                                 \
        vec_swap_remove(Entry(k_type, v_type), &dd_dict_ptr->entries,    \
                        b->index - 2);                                   \
        b->index = 1;                                                    \
        lval_result_bool = true;                                         \
      }                                                                  \
    }                                                                    \
  })

/// @brief Frees the dictionary
///
/// @param k_type The type of the keys in the dictionary
/// @param v_type The type of the values in the dictionary
/// @param dict_ptr Pointer to the dictionary
/// @hideinitializer
#define dict_free(k_type, v_type, dict_ptr)         \
  do {                                              \
    Dict(k_type, v_type)* df_dict_ptr = (dict_ptr); \
    free(df_dict_ptr->buckets);                     \
    vec_free(&df_dict_ptr->entries);                \
  } while (0)
#endif  // SNIFEX_API_GNU_EXTENSIONS

/// @}

#endif  // SNIFEX_API_H

// IMPLEMENTATION

#ifdef SNIFEX_API_IMPLEMENTATION

#ifndef SNIFEX_API_NO_ASSERT
void __snifex_api_assert_fail(const char* expr,
                              const char* file,
                              const int line,
                              const char* func) {
  fprintf(stderr,
          "\33[0;31m%s\33[0m:\33[0;33m%d\33[0m in func "
          "\33[0;32m%s\33[0m():\n\t\33[0;91mAssert failed\33[0m -> %s\n",
          file, line, func, expr);
#ifdef OS_LINUX
  fprintf(stderr, "\33[38;5;124mStacktrace:\33[0m\n");
  char** strings;
  size_t size;
  void* array[1024];
  size = backtrace(array, 1024);
  strings = backtrace_symbols(array, size);
  for (size_t i = 0; i < size; i++) printf("\33[0;37m%s\n", strings[i]);
  printf("\33[0m\n");
  free(strings);
#endif
#ifdef OS_WIN
  fprintf(stderr, "\33[38;5;124mStacktrace:\33[0m\n");

  const DWORD options = SymGetOptions();
  SymSetOptions(options | SYMOPT_LOAD_LINES);
  SymInitialize(GetCurrentProcess(), NULL, true);

  void* array[48];
  size_t size = CaptureStackBackTrace(0, 48, array, NULL);

  for (size_t i = 0; i < size; i++) {
    DWORD64 address = (DWORD64)(array[i]);
    CHAR buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(CHAR)];
    PSYMBOL_INFO symbol = (PSYMBOL_INFO)buffer;
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
    symbol->MaxNameLen = MAX_SYM_NAME;

    if (SymFromAddr(GetCurrentProcess(), address, 0, symbol)) {
      fprintf(stderr, "\33[0;37mFrame %u: %s\n", i, symbol->Name);
    } else {
      fprintf(stderr,
              "\33[0;37mFrame %u: Address = 0x%p (No debug info, if using MSVC "
              "remember the /Zi flag)\n",
              i, (void*)address);
    }
  }
  printf("\33[0m\n");
  SymCleanup(GetCurrentProcess());
#endif  // OS_WIN
  exit(1);
}
#endif  // SNIFEX_API_NO_ASSERT

inline bool __snifex_api_is_power_of_two(const size_t x) {
  return (x & (x - 1)) == 0;
}

// uint32_t ceil_powtwo(uint32_t v) {
//   v--;
//   v |= v >> 1;
//   v |= v >> 2;
//   v |= v >> 4;
//   v |= v >> 8;
//   v |= v >> 16;
//   return v + 1;
// }

void dyn_arena_init(DynArena* const dyn_arena, const size_t init_cap) {
  dyn_arena->buf = (char*)malloc(init_cap);
  dyn_arena->cap = init_cap;
  dyn_arena->top = 0;
  assert(dyn_arena->buf != NULL);
}

void arena_init(Arena* const arena, const size_t size) {
  arena->buf = (char*)malloc(size);
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

size_t dyn_arena_alloc(DynArena* const dyn_arena,
                       const size_t size,
                       const size_t alignment) {
  assert(!(size == 0 || alignment == 0 ||
           (alignment != 1 && !__snifex_api_is_power_of_two(alignment)) ||
           size % alignment != 0));

  size_t top = dyn_arena->top;
  size_t padding = (top + alignment - 1) & ~(alignment - 1);

  if (top + padding + size > dyn_arena->cap) {
    dyn_arena->cap += top + padding + size;
    dyn_arena->cap *= 2;
    dyn_arena->buf = (char*)realloc(dyn_arena->buf, dyn_arena->cap);
    assert(dyn_arena->buf != NULL);
  }

  dyn_arena->top += size + padding;
  return top;
}

void* arena_alloc(Arena* const arena,
                  const size_t size,
                  const size_t alignment) {
  if (size == 0) { return NULL; }
  assert(!(alignment == 0 ||
           (alignment != 1 && !__snifex_api_is_power_of_two(alignment)) ||
           size % alignment != 0));

  size_t top = arena->top;
  size_t padding = (top + alignment - 1) & ~(alignment - 1);

  if (top + padding + size > arena->size) { return NULL; }

  void* ret = &arena->buf[top];
  arena->top += size + padding;
  return ret;
}

void dyn_arena_reserve(DynArena* const dyn_arena, const size_t min_cap) {
  if (dyn_arena->cap < min_cap) {
    dyn_arena->cap *= min_cap;
    dyn_arena->buf = (char*)realloc(dyn_arena->buf, dyn_arena->cap);
    assert(dyn_arena->buf != NULL);
  }
}

void dyn_arena_free(DynArena* const dyn_arena) { free(dyn_arena->buf); }
void arena_free(Arena* const arena) { free(arena->buf); }

string strlit(char const* s) {
  return (string){.ptr = (char*)s, .len = strlen(s)};
}
bool str_is_empty(const string str) {
  return (str.ptr == NULL || str.len == 0);
}

string str_alloc(Arena* const arena, const size_t len) {
  assert(arena != NULL);

  return (string){
      .ptr = (char*)arena_alloc(arena, len, 1),
      .len = len,
  };
}

string str_copy(Arena* const arena, const string str) {
  if (str.len == 0) { return str; }
  assert(arena != NULL);

  char* dest = (char*)arena_alloc(arena, str.len, 1);
  return (string){
      .ptr = (char*)memcpy(dest, str.ptr, str.len),
      .len = str.len,
  };
}

char* str_idx(const string str, const size_t i) {
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
string str_concat(Arena* const arena, const string a, const string b) {
  assert(arena != NULL);

  size_t new_len = a.len + b.len;
  string buf = str_alloc(arena, new_len);
  if (a.len != 0) { memcpy(buf.ptr, a.ptr, a.len); }
  if (b.len != 0) { memcpy(buf.ptr + a.len, b.ptr, b.len); }
  return buf;
}

string str_join(Arena* const arena, Vec(string) to_join) {
  assert(arena != NULL);

  size_t new_len = 0;
  for (size_t i = 0; i < to_join.len; i++) {
#ifdef SNIFEX_API_GNU_EXTENSIONS
    new_len += vec_idx(to_join, i)->len;
#else
    string* str;
    vec_idx(str, string, to_join, i);
    new_len += str->len;
#endif
  }
  string buf = str_alloc(arena, new_len);

  char* dest = buf.ptr;
  for (size_t i = 0; i < to_join.len; i++) {
#ifdef SNIFEX_API_GNU_EXTENSIONS
    string str = *vec_idx(to_join, i);
#else
    string* str_ptr;
    vec_idx(str_ptr, string, to_join, i);
    string str = *str_ptr;
#endif
    memcpy(dest, str.ptr, str.len);
    dest += str.len;
  }
  return buf;
}

string str_fmt(Arena* const arena, const char* fmt, ...) {
  assert(arena != NULL && fmt != NULL);

  va_list args;
  va_start(args, fmt);

  va_list args_temp;
  va_copy(args_temp, args);
  size_t needed = vsnprintf(NULL, 0, fmt, args_temp);
  va_end(args_temp);

  char* const buf = (char*)malloc(needed + 1);
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

  while (start < end && isspace(*str_idx(str, start))) { start++; }
  if (start == end) { return (string){0}; }
  while (end > start && isspace(*str_idx(str, end - 1))) { end--; }

  return str_slice(str, start, end);
}

Bucket* snifex_api_find_bucket(Bucket* buckets,
                               size_t bucket_cap,
                               void* key,
                               uint64_t hashed_key,
                               void* entries,
                               size_t entry_size,
                               size_t key_size,
                               bool include_tombs) {
  /* find spot (either empty or entry itself) */
  size_t index = hashed_key % bucket_cap;
  Bucket* b;
  for (;;) {
    b = &buckets[index];
    bool replaced;
    if (b->index == 0 || (b->index == 1 && include_tombs) ||
        (replaced = (b->index > 1 && b->hash == hashed_key &&
                     memcmp(((char*)entries + (b->index - 2) * entry_size), key,
                            key_size) == 0))) {
      return b;
    }

    index = (index + 1) % bucket_cap;
  }
}

void snifex_api_dict_grow(Bucket** buckets,
                          size_t* bucket_cap,
                          size_t* bucket_len) {
  size_t old_cap = *bucket_cap;
  size_t new_cap = (*bucket_cap *= 2);

  Bucket* new_bucks = calloc(new_cap, sizeof(Bucket));
  assert(new_bucks != NULL);

  *bucket_len = 0;
  for (size_t i = 0; i < old_cap; i++) {
    Bucket to_put = (*buckets)[i];
    if (to_put.index == 0) { continue; }

    size_t index = to_put.hash % new_cap;
    Bucket* b;
    for (;;) {
      b = &new_bucks[index];

      if (b->index == 0) {
        *b = to_put;
        *bucket_len += 1;
        break;
      }

      index = (index + 1) % new_cap;
    }
  }
  free(*buckets);
  *buckets = new_bucks;
}

uint64_t snifex_api_hash_num_func(const void* in, const size_t inlen) {
#define ROTL(a, b) (((a) << (b)) | ((a) >> (8 - (b))))
  const uint8_t* v = (const uint8_t*)in;

  size_t i = 0;
  uint64_t ret = 0;

  for (; i + 8 <= inlen; i += 8) {
    uint8_t a = v[i + 0];
    uint8_t b = v[i + 1];
    uint8_t c = v[i + 2];
    uint8_t d = v[i + 3];

    uint8_t e = v[i + 4];
    uint8_t f = v[i + 5];
    uint8_t g = v[i + 6];
    uint8_t h = v[i + 7];
    a += b, d ^= a, d = ROTL(d, 4);
    c += d, b ^= c, b = ROTL(b, 4);
    e += f, h ^= e, h = ROTL(h, 4);
    g += h, f ^= g, f = ROTL(f, 4);
    ret ^= ((uint64_t)a << 56 | (uint64_t)b << 48 | (uint64_t)c << 40 |
            (uint64_t)d << 32 | (uint64_t)e << 24 | (uint64_t)f << 16 |
            (uint64_t)g << 8 | (uint64_t)h);
  }
  for (; i < inlen; i++) {
    ret += v[i];
    ret ^= v[i];
    ret = ROTL(ret, 8);
  }
  return ret;
#undef ROTL
}

#endif  // SNIFEX_API_IMPLEMENTATION
