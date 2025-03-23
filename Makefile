CURR_DIR = $(shell pwd)
OUT = $(CURR_DIR)/out
BIN_NAME = test_examples
export OUT
export BIN_NAME

CC = clang
COMMON_ARGS = -std=c99 -Wall -Werror -fstrict-aliasing -Wstrict-aliasing -Wno-unused \
							-fsanitize=address -fno-omit-frame-pointer -fstandalone-debug
DEPS =

.PHONY: docs

docs:
	@doxygen ./Doxyfile.gnu
	@doxygen ./Doxyfile.nongnu


# Tests
default: test

b: build
build: src/tests-gcc-clang/*.c src/snifex-api.h
	@-rm $(OUT)/$(BIN_NAME)
	bear -- $(CC) src/tests-gcc-clang/*.c -g $(DEPS) $(COMMON_ARGS) -o $(OUT)/$(BIN_NAME)

bng: build-non-gnu
build-non-gnu: src/tests-non-gnu/*.c src/snifex-api.h
	@-rm $(OUT)/$(BIN_NAME)_nongnu
	bear -- $(CC) src/tests-non-gnu/*.c -g $(DEPS) $(COMMON_ARGS) -o $(OUT)/$(BIN_NAME)_nongnu -D NO_GNU_SNIFEX_API_TESTS

t: test
test: build
	$(OUT)/$(BIN_NAME)

tng: test-non-gnu
test-non-gnu: build-non-gnu
	$(OUT)/$(BIN_NAME)_nongnu

clean:
	-rm $(OUT)/* -r
