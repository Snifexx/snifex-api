
This is my quick and easy to use API for C.

For now, its only uses are for strings and arena allocators, which are essential for me, but in the future I might add more features.

Honestly, I'll implement shit as I need to, but if you feel like contributing drop in a pull request in another feature branch and I'll take a look!\
So don't take the TODO too seriously.

This project uses C99 and I have my tests with clang (as you can see in the [Makefile](./Makefile)) but should work with any c99 compiler.

# My guidelines
This is a small definition of all the rules I've imposed on myself when writing C, my own little standard.
Obviously, these rules can be broken when interfacing with other works that are not my own.
Some of these might sound like simple good practise instead of all and out hard rules but I use these as a frame of reference and try
to stick to these as much as I can (ideally always).

- As already stated use C99

-  Always use the most correct, descriptive and well-defined integer type. That means:
    - using only exact-width integer types (`intN_t`/`uintN_t`);
    - using `size_t` for sizes, lengths, array indices or anything that is pointer arithmetic related;
    - using `ptrdiff_t` for signed pointer differences (the very few times they are needed...);
    - using `bool` (or `_Bool`) for booleans.

    This entails never make use of normal integer types such as `int` or `long` with the only exception of `char`,
    for ASCII characters or bytes.
    instead of `uintptr_t` or `intptr_t`. If you really have to use one of the two prefer the former.

- My styling guidelines:
    - *snake_case* for functions and macros (that are intended to be used as funcs),
    - *camelCase* for variables,
    - *CAPS_SNAKE_CASE* for constants and macros (that are intended to be used as constants or utility macros),
    - *PascalCase* for types and macros (that are intended to be used as types) with the exception of the basic `string` type I made.

- Some naming conventions:
    - When naming functions that operate on a type, that could 'feel' method-like, prefix the name with the type,
    maybe abbreviating it. Examples: `str_trim`, `arena_alloc`

- Good practise:
    - Practise Defensive Programming, I.E. spam asserts a lot.

# TODO

- ***IMPORTANT!*** Add Doxygen banners and documentation
-  Count substring occurrences
-  Debug API to replace malloc, realloc, etc... with macro hooks to check that all allocations are deallocated
-  Find first occurence of substring
-  Find last occurence of substring
-  Replace all occurrences of substring

