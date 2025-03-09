
This is my quick and easy to use API for C.

For now, its only uses are for strings and arena allocators, which are essential for me, but in the future I might add more features.

Honestly, I'll implement shit as I need to, but if you feel like contributing drop in a pull request in another feature branch and I'll take a look!<br>
So don't take the TODO too seriously.

This project uses C99 and I have my tests with clang (as you can see in the [Makefile](./Makefile)) but should work with any c99 compiler.

# My guidelines
This is a small definition of all the rules I've imposed on myself when writing C, my own little standard.
Obviously, these rules can be broken when interfacing with other works that are not my own.
Some of these might sound like simple good practise instead of all and out hard rules but I use these as a frame of reference and try
to stick to these as much as I can (ideally always).

-  Always use the most correct, descriptive and well-defined integer type. That means:
    - using only exact-width integer types (`intN_t`/`uintN_t`);
    - using `size_t` for sizes, lengths, array indices or anything that is pointer arithmetic related;
    - using `ptrdiff_t` for signed pointer differences (the very few times they are needed...);
    - using `bool` (or `_Bool`) for booleans.

    This entails never make use of normal integer types such as `int` or `long` with the only exception of `char`,
    for ASCII characters or bytes.

-  For generic/unknown pointers predilect the use of `void*` (or `char*` only for single byte pointer arithmetic)
    instead of `uintptr_t` or `intptr_t`. If you really have to use one of the two prefer the former.

# TODO

-  Count substring occurrences
-  Debug API to replace malloc, realloc, etc... with macro hooks to check that all allocations are deallocated
-  Find first occurence of substring
-  Find last occurence of substring
-  Replace all occurrences of substring
-  Arrays: I was unsure whether to refactor string to be charstr or not. I have come to the conclusion that
Strings are just too important to be simple arrays. Especially because arrays will be implemented as macros.
