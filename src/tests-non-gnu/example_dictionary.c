#include "bits/stdint-uintn.h"
#define HASHFUNC  // Refer to dict_custom_hashing for explanation
#include "../snifex-api.h"

// Refer to dict_custom_hashing for explanation
#define hash_num(in_ptr, inlen, k0_u64, k1_u64) (my_hash(in_ptr, inlen))

static uint64_t my_hash(const void* in, const size_t inlen);

typedef struct {
  int a_struct_key;
  float cuz_why_not;
} MyStruct;

DefineDict(MyStruct, float);

void dict_usage() {
  Dictionary(MyStruct, float) dict;
  dict_create(dict, MyStruct, float);

  dict_put(MyStruct, float, &dict, ((MyStruct){1, 2.0}), 420.0, NULL);
  float old_value;
  dict_put(MyStruct, float, &dict, ((MyStruct){1, 2.0}), 69.0, &old_value);
  assert(old_value == 420.0);

  float* curr_value_ptr;
  dict_get(curr_value_ptr, MyStruct, float, &dict, ((MyStruct){1, 2.0}));
  assert(*curr_value_ptr == 69.0);

  bool did_delete;
  dict_del(did_delete, MyStruct, float, &dict, ((MyStruct){1, 2.0}));

  dict_get(curr_value_ptr, MyStruct, float, &dict, ((MyStruct){1, 2.0}));
  assert(curr_value_ptr == NULL);

  dict_put(MyStruct, float, &dict, ((MyStruct){1, 2.0}), 69.0, &old_value);
  dict_get(curr_value_ptr, MyStruct, float, &dict, ((MyStruct){1, 2.0}));
  assert(*curr_value_ptr == 69.0);

  dict_free(MyStruct, float, &dict);
}

uint64_t my_hash(const void* in, const size_t inlen) {
  const uint8_t* in_bytes = in;
  uint64_t res = 0x88873830983F3138;
  for (size_t i = 0; i < inlen; i++) { res ^= in_bytes[i]; }
  return res;
}

DefineDict(uint64_t, float);

void dict_custom_hashing() {
  //- To define a custom hashing algorithm one must define the `HASHFUNC` macro before
  //- including the snifex-api header file. Then the macro `hash_num` MUST be defined before
  //- any usage of dictionary macros and functions. The body can be any function that returns
  //- a uint64_t. The definition of the macro `hash_num` must have four arguments and it follows
  //- this rough signature:
  //-     #define hash_num(in_ptr, inlen, k0_u64, k1_u64)
  //-   where:
  //-     - `in_ptr` is of type `const void*`.
  //-     - `inlen` is of type `const size_t`, and obviously it holds
  //-         the size of the data pointed by `in_ptr`.
  //-     - `k0_u64` is of type `uint64_t` (as expressed by the hungarian notation). This holds the least significant part of a u128 key.
  //-     - `k1_u64` is of type `uint64_t` (as expressed by the hungarian notation). This holds the most significant part of a u128 key.
  //-   and returns a uint64_t hash.

  // The following tests assert on the hash being hashed by our custom hashing function `my_hash`

  Dictionary(uint64_t, float) dict;
  dict_create(dict, uint64_t, float);
  dict_put(uint64_t, float, &dict, 3, 10.0, NULL);
  // 3 in hex, little-endian is:
  // 0x03 0x00 0x00 0x00  0x00 0x00 0x00 0x00
  // so if your platform uses little-endian integers:
  //
  // 0x88873830983F3138 ^ 0x03 = 0x88873830983f313b
  //                                 |
  //        /------------------------/
  //        |
  // 0x88873830983f313b ^ 0x00 = 0x88873830983f313b (the same)
  //    ...
  // We should get 0x88873830983f313b. That is 9837893692378001723 in decimal
  //
  // Since dictionaries start with a default capacity of 8 buckets:
  //    0x88873830983f313b % 8 = 3
  // The bucket that is going to be filled is bucket #3 (The fourth).
  
  printf("\33[0;33mRunning a test that assumes Little-endian integers! If assert fails, you know why!\33[0m\n");
  assert(dict.buckets[3].hash == 0x88873830983f313b);
  printf("\33[0;32mTest Passed!\33[0m\n");

  dict_free(uint64_t, float, &dict);
}
