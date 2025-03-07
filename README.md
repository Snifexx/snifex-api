
This is my quick and easy to use API for C.

For now, its only uses are for strings and arena allocators, which are essential for me, but in the future I might add more features.

Honestly, I'll implement shit as I need to, but if you feel like contributing drop in a pull request in another feature branch and I'll take a look!<br>
So don't take the TODO too seriously.

This project uses C99 and I have my tests with clang (as you can see in the [Makefile](./Makefile)) but should work with any c99 compiler.



# TODO

- [ ] Count substring occurrences
- [ ] Find first occurence of substring
- [ ] Find last occurence of substring
- [ ] Replace all occurrences of substring
- [ ] Get the sign of a float/int
- [ ] In `str_trim`: trim with all whitespaces not just `' '`
