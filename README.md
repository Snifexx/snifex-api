# Snifex API

This is my quick and easy to use API for C.

For now, its only uses are for strings and arena allocators, which are essential for me, but in the future I might add more features.

Honestly, I'll implement shit as I need to, but if you feel like contributing drop in a pull request in another feature branch and I'll take a look!\
So don't take the TODO too seriously.

This project uses C99 and I have my tests with clang (as you can see in the [Makefile](./Makefile)) but should work with any c99 compiler.
It's an [stb single file header library](https://github.com/nothings/stb).

## Documentation
I wrote documentation and generated it with [Doxygen](https://www.doxygen.nl).
Github page available:
  - [Here](https://snifexx.github.io/snifex-api/) for GCC/Clang or any compiler with Gnu Extensions and `__GNUC__`,
  - or [here](https://snifexx.github.io/snifex-api/nongnu) for any other non gnu compiler, or if `NO_GNU_SNIFEX_API_TESTS` macro is defined.

## Usage
Since this is an stb file header it packs the header file with the implementations all in one. What does that mean?
It simply means that you can treat it as a normal header file, however at least ONCE it must be 'implemented' through a macro.

For examples on all the API functions take a look at the [examples](./src/all_examples.c).
I've also included a Makefile. Its basis is auto-generated, you should only ever need the `debug` and `run-debug` task to test
th examples
```c
// file1.c

#include <file2.c>
#define SNIFEX_API_IMPLEMENTATION
#include <snifex-api.h>
#undef SNIFEX_API_IMPLEMENTATION //not really needed, but just to be sure

int main() {
    my_test();
    return 0;
}

// file2.c

#include <stdio.h>
#include <stdint.h>
#include <snifex-api.h>
#include <inttypes.h>

void my_test() {
#ifdef SNIFEX_API_GNU_EXTENSIONS 
    uint8_t sign = sign(10);
#else
    uint8_t sign; sign(sign, uint8_t, 10);
#endif
    printf("10 is positive! In fact, its sign is: %" PRIu8 "\n", sign);
}
```

## My guidelines
This is a small definition of all the rules I've imposed on myself when writing C, my own little standard.
Obviously, these rules can be broken when interfacing with other works that are not my own.
Some of these might sound like simple good practise instead of all and out hard rules but I use these as a frame of reference and try
to stick to these as much as I can (ideally always).

- These are the assumptions this API makes:
    1. As already stated, it conforms to C99.
    2. As explained later, it supposes IEEE 754 standards are being followed, which is true for any modern system.
    3. This project in particular is compiler agnostic, however usage between GCC/Clang and others may vary, as for more safety
    I have implemented API differences that depend on `__GNUC__`

- Always use the most correct, descriptive and well-defined integer type. That means:
    - using only exact-width integer types (`intN_t`/`uintN_t`);
    - using `size_t` for sizes, lengths, array indices or anything that is pointer arithmetic related;
    - using `ptrdiff_t` for signed pointer differences (the very few times they are needed...);
    - using `bool` (or `_Bool`) for booleans.

    This entails never make use of normal integer types such as `int` or `long` with the only exception of `char`,
    for ASCII characters or bytes.
    instead of `uintptr_t` or `intptr_t`. If you really have to use one of the two prefer the former.

- The same holds truth for floating-point numbers. Since this project assumes IEEE format:
    - `float` is 32-bit sized and...
    - ... `double` is 64-bit sized.
    - Also, because of variance in implementation of `long double` among compilers and system (whether it's x86's
    80-bit extended precision or IEEE 128-bit quadruple precision floats or anything else), avoid usage of `long double`s
    as much as possible, or otherwise use math libraries.

- My styling guidelines:
    - *snake_case* for variables, functions, macros (that are intended to be used as funcs) and structs (that are not aliased by `typedef`);
    - *CAPS_SNAKE_CASE* for constants and macros (that are intended to be used as constants OR utility macros);
    - *PascalCase* for types and macros (that are intended to be used as types) with the exception of the basic `string` type I made.
    Structs that are not aliased with `typedef` do not adhere to this and are written as snake_case, E.G. 
    ```c
    typedef struct MyType { // The name of the struct here is the exception, since I'm aliasing anyways
        uint8_t field;
    } MyType;

    struct my_struct {
        uint8_t field;
    };

    void func() {
        MyType a = { 10 };
        struct my_struct = { 20 };
    }
    ```


- Some naming conventions:
    - When naming functions that operate on a type, that could 'feel' method-like, prefix the name with the type,
    maybe abbreviating it. Examples: `str_trim`, `arena_alloc`.

- Good practise:
    - Practise Defensive Programming, I.E. spam asserts a lot.
    - When making macros for non GCC/Clang compilers use the classical do {} while (0) and when replicating GNU's expression statements,
    have an argument named `lval_result_t` (where t is the type of the lvalue) that is set at the end of the macro.
    - When making macros for GCC or Clang that are NOT expression statements, still use the do {} while (0) hack.

## TODO

- ***IMPORTANT!*** Add alignment in powers of two to capacities
- ***IMPORTANT!*** Separate header to different submodules and have an all including. This mainly for when I have different std stuff.
- Arena allocations for Vectors
- *USEFUL* Debug API to replace malloc, realloc, etc... with macro hooks to check that all allocations are deallocated
- Count substring occurrences
- Find first occurence of substring
- Find last occurence of substring
- Replace all occurrences of substring




