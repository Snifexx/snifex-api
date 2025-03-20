{ pkgs ? import <nixpkgs> {} }:

pkgs.clangStdenv.mkDerivation {
    name = "clang-nix-shell";
    buildInputs = with pkgs; [
      gnumake bear
      lldb 

      ### Documentation
      doxygen

      ### Other debug tools I sometimes use
      # valgrind heaptrack 
    ];


    shellHook = ''
        if [ ! -e .nix-shell-run ]; then
            touch .nix-shell-run
cat > Makefile << MAKEND
CURR_DIR = \$(shell pwd)
OUT = \$(CURR_DIR)/out
BIN_NAME = main
export OUT
export BIN_NAME

CC = clang
COMMON_ARGS = -std=c99 -Wall -Werror
DEPS = 

default: src/*.c
	-rm \$(OUT)/\$(BIN_NAME)
	\$(CC) src/*.c -O3 \$(DEPS) \$(COMMON_ARGS) -o \$(OUT)/\$(BIN_NAME)

debug: src/*.c
	-rm \$(OUT)/\$(BIN_NAME)_debug
	\$(CC) src/*.c -O0 \$(DEPS) \$(COMMON_ARGS) -o \$(OUT)/\$(BIN_NAME)_debug \\
		-g -fsanitize=address -fno-omit-frame-pointer -fstandalone-debug

clean:
	-rm \$(OUT)/* -r

MAKEND
mkdir src
mkdir out
cat > src/main.c << MAINEND
int main() {
  return 0;
}
MAINEND
        fi
    '';
}
