CURR_DIR = $(shell pwd)
OUT = $(CURR_DIR)/out
BIN_NAME = all_examples
export OUT
export BIN_NAME

CC = clang
COMMON_ARGS = -std=c99 -Wall -Werror -fstrict-aliasing -Wstrict-aliasing -Wno-unused-variable
DEPS = 

# Tests

release: src/all_examples.c src/snifex-api.h
	@-rm $(OUT)/$(BIN_NAME)
	$(CC) src/all_examples.c -O3 $(DEPS) $(COMMON_ARGS) -o $(OUT)/$(BIN_NAME) -flto -DNDEBUG

debug: src/all_examples.c src/snifex-api.h
	@-rm $(OUT)/$(BIN_NAME)_debug
	$(CC) src/all_examples.c -g -O0 $(DEPS) $(COMMON_ARGS) -o $(OUT)/$(BIN_NAME)_debug \
		-fsanitize=address -fno-omit-frame-pointer -fstandalone-debug

run-release: release
	$(OUT)/$(BIN_NAME)

run-debug: debug
	$(OUT)/$(BIN_NAME)_debug

clean:
	-rm $(OUT)/* -r

