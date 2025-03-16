#include "../snifex-api.h"

void string_usage() {
  Arena scratch = arena_create(4096);

  //-
  //- Creating a string
  //-
  string string_literal = strlit("Normal C string literal");

  // Method 1: allocating and then copying. Less safe
  string str1 = str_alloc(&scratch, string_literal.len);
  memcpy(str1.ptr, string_literal.ptr, string_literal.len);

  // Method 2: using `str_copy`. More safe
  string str2 = str_copy(&scratch, string_literal);

  //-
  //- Equality check on strings
  //-
  assert(str_eq(str1, str2));

  //-
  //- Indexing character in string
  //-
  char* const c = str_idx(str1, 3);
  assert(*c == 'm');
  *c = 'a';
  assert(str_eq(str1, strlit("Noraal C string literal")));

  //-
  //- String Utilities
  //-
  string null_str = {0};
  assert(str_is_empty(null_str));

  string string_concat =
      str_concat(&scratch, strlit("Test j"), strlit("oined string"));
  assert(str_eq(string_concat, strlit("Test joined string")));

  string string_slice = str_slice(strlit("String to be sliced"), 7, 12);
  assert(str_eq(string_slice, strlit("to be")));

  string string_trimmed = str_trim(strlit("    To be trimmed\n\n\t"));
  assert(str_eq(string_trimmed, strlit("To be trimmed")));

  string fmt_str = str_fmt(&scratch, "This is a string '%.*s' and its size %zu",
                           (int)str2.len, str2.ptr, str2.len);
  assert(str_eq(
      strlit("This is a string 'Normal C string literal' and its size 23"),
      fmt_str));

  // If you want to print/format a string longer than INT_MAX, you have to use
  // `str_join`:
  Vec(string) to_join =
      vec_from(string, strlit("This is a string '"), str2,
               str_fmt(&scratch, "' and its size %zu", str2.len));

  string string_joined = str_join(&scratch, to_join);
  assert(str_eq(
      strlit("This is a string 'Normal C string literal' and its size 23"),
      string_joined));

  vec_free(&to_join);
  arena_free(&scratch);
}
