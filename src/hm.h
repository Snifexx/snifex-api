#ifndef HM_H
#define HM_H

#include <stdlib.h>
#include "bits/stdint-uintn.h"
#include "snifex-api.h"
#include "string.h"

typedef struct bucket {
  uint64_t hash;
  size_t index;
} Bucket;

#define Entry(K, V) Entry_##K##_##V
#define Dictionary(K, V) Dictionary_##K##_##V
#define VecEntry(K, V) Vec_Entry_##K##_##V

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
  } Dictionary(K, V);

#define dict_create(K, V)                    \
  ((Dictionary(K, V)){                       \
      .entries = vec_create(Entry(K, V), 8), \
      .buckets = calloc(8, sizeof(Bucket)),  \
      .b_cap = 8,                            \
      .b_len = 0,                            \
  })

#define hashNum(a) (a)

Bucket* find_bucket(Bucket* buckets,
                    size_t bucket_cap,
                    void* key,
                    uint64_t hashed_key,
                    void* entries,
                    size_t entry_size,
                    size_t key_size) {
  uint64_t _hashed_key = (hashed_key);

  /* find spot (either empty or entry itself)*/
  size_t index = _hashed_key % bucket_cap;
  Bucket* b;
  for (;;) {
    b = &buckets[index];
    bool replaced;
    if (b->index != 0 &&
        !(replaced = (b->hash == _hashed_key &&
                      memcmp(((char*)entries + (b->index - 1) * entry_size),
                             key, key_size) == 0))) {
      index = (index + 1) % bucket_cap;
      continue;
    }
    break;
  }
  return b;
}

// static void dict_grow(Dictionary* dict) {
#define dict_grow(dict_ptr)                                        \
  do {                                                             \
    __auto_type __dict_ptr = (dict_ptr);                           \
                                                                   \
    size_t old_cap = __dict_ptr->b_cap;                            \
    __dict_ptr->b_cap *= 2;                                        \
    Bucket* new_bucks = calloc(__dict_ptr->b_cap, sizeof(Bucket)); \
    assert(new_bucks != NULL);                                     \
                                                                   \
    for (size_t i = 0; i < old_cap; i++) {                         \
      Bucket to_put = __dict_ptr->buckets[i];                      \
      if (to_put.index == 0) { continue; }                         \
                                                                   \
      size_t index = to_put.hash % __dict_ptr->b_cap;              \
      Bucket* b;                                                   \
      for (;;) {                                                   \
        b = &new_bucks[index];                                     \
                                                                   \
        if (b->index == 0) {                                       \
          *b = to_put;                                             \
          break;                                                   \
        }                                                          \
                                                                   \
        index = (index + 1) % __dict_ptr->b_cap;                   \
      }                                                            \
    }                                                              \
    free(__dict_ptr->buckets);                                     \
    __dict_ptr->buckets = new_bucks;                               \
  } while (0)

// bool dict_put(Dictionary* dict, uint64_t key, float value, float* old_value)
// {
#define dict_put(dict_ptr, k, v, old_value_ptr)                                \
  ({                                                                           \
    __auto_type _dict_ptr = (dict_ptr);                                        \
    __typeof(_dict_ptr->entries.ptr->key) _key = (k);                          \
    __typeof(_dict_ptr->entries.ptr->value) _value = (v);                      \
    __typeof(&_value) _old_value_ptr = (old_value_ptr);                        \
    uint64_t hashed_key = hashNum(_key);                                       \
    Bucket* b =                                                                \
        find_bucket(_dict_ptr->buckets, _dict_ptr->b_cap, &_key, hashed_key,   \
                    _dict_ptr->entries.ptr, sizeof(*(_dict_ptr->entries.ptr)), \
                    sizeof(_dict_ptr->entries.ptr->key));                      \
    bool res;                                                                  \
    if (b->index != 0) {                                                       \
      __auto_type e = vec_idx(_dict_ptr->entries, b->index - 1);               \
      if (_old_value_ptr != NULL) { *_old_value_ptr = e->value; }              \
      e->value = _value;                                                       \
      res = true;                                                              \
    } else {                                                                   \
      b->hash = hashed_key;                                                    \
      b->index = _dict_ptr->entries.len + 1;                                   \
      _dict_ptr->b_len++;                                                      \
                                                                               \
      if (_dict_ptr->b_len >= _dict_ptr->b_cap * 0.75) {                       \
        dict_grow(_dict_ptr);                                                  \
      }                                                                        \
      __typeof(*_dict_ptr->entries.ptr) to_push_entry = {.key = _key,          \
                                                         .value = _value};     \
      vec_push((&_dict_ptr->entries), to_push_entry);                          \
      res = false;                                                             \
    }                                                                          \
    res;                                                                       \
  })

/*
float* dict_get(Dictionary* dict, uint64_t key) {
  if (dict->b_len == 0) { return NULL; }

  Bucket* b = find_bucket(dict, key, hashNum(key));
  if (b->index == 0) { return NULL; }

  return &vec_idx(Entry, dict->entries, b->index - 1)->value;
}
*/

#endif  // HM_H
