#include "snifex-api.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

void string_usage() {
  Arena arena = arena_create(10);

  //-
  //- Creating a string
  //-
  string string_literal = strlit("Normal C string literal");

  // Method 1: allocating and then copying. Less safe
  string str1 = str_alloc(&arena, string_literal.len);
  memcpy(str1.ptr, string_literal.ptr, string_literal.len);

  // Method 2: using `str_copy`. More safe
  string str2 = str_copy(&arena, string_literal);
  
  //-
  //- Equality check on strings
  //-
  assert(str_eq(str1, str2));

  //-
  //- Indexing character in string
  //-
  char* c = str_idx(str1, 3);
  assert(*c == 'm');
  *c = 'a';
  assert(str_eq(str1, strlit("Noraal C string literal")));
  

  //-
  //- String Utilities
  //-
  string nullStr = {0};
  assert(str_is_empty(nullStr));

  string joinedStr = str_join(&arena, strlit("Test j"), strlit("oined string"));
  assert(str_eq(joinedStr, strlit("Test joined string")));

  string stringSlice = str_slice(strlit("String to be sliced"), 7, 12);
  assert(str_eq(stringSlice, strlit("to be")));

  string stringTrimmed = str_trim(strlit("    To be trimmed\n\n\t"));
  assert(str_eq(stringTrimmed, strlit("To be trimmed")));

  string frmStr = str_fmt(&arena, "This is a string %.*s and its size %zu", str2.len, str2.ptr, str2.len);
  printf("%.*s", (int) frmStr.len, frmStr.ptr);

  arena_free(&arena);
}
